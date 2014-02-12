#include <EEPROM.h>
#include <avr/pgmspace.h>
#include <DMXSerial2.h>
#include <Bounce.h>
#include <bitlash.h>
#include <Wire.h>

#define DmxModePin 36
#define DmxModeOut LOW
#define DmxModeIn HIGH

#define SR_OUTPUT 39
#define SR_SCK 38
#define SR_RCK 37
#define SR_OE  13

#define NUM_OUTPUTS 32
#define NUM_INPUTS 16

#define I2C_EEPROM_ADDRESS 0x50

#define EEPROM_OUTPUT_NAME_OFFSET 0x00000000
#define EEPROM_INPUT_NAME_OFFSET  EEPROM_OUTPUT_NAME_OFFSET + (MAX_LABEL_LENGTH * NUM_OUTPUTS)
#define EEPROM_INPUT_MODE_OFFSET  EEPROM_INPUT_NAME_OFFSET + (MAX_LABEL_LENGTH * NUM_INPUTS)

#define MAX_LABEL_LENGTH 32

bool outputs[NUM_OUTPUTS];
bool dmxInputBlocked = false;

uint8_t looper = 0;

const uint16_t my_pids[] = { E120_SLOT_DESCRIPTION, E120_DEFAULT_SLOT_VALUE,
		E120_SLOT_INFO };

struct RDMINIT rdmInit = { "raumzeitlabor.de", 1, "Lichtsteuerung",
NUM_OUTPUTS, // footprint
		(sizeof(my_pids) / sizeof(uint16_t)), my_pids,
		{ 0x09, 0x7F, 0x23, 0x42,
						0x00, 0x00 }, E120_PRODUCT_CATEGORY_POWER, // Product category
				0x00000001UL // Software Version

};

PROGMEM const prog_uchar inputPins[] = { 0, 1, 2, 3, 4, 5, 6, 7, 16, 17, 18, 19,
		20, 21, 22, 23 };

Bounce debouncers[16];

/**
 * Initializes the output array and sets up the pins required for the shift register.
 */
void initOutputs() {
	pinMode(SR_OUTPUT, OUTPUT);
	pinMode(SR_SCK, OUTPUT);
	pinMode(SR_RCK, OUTPUT);
	pinMode(SR_OE, OUTPUT);

	digitalWrite(SR_OE, LOW);
	for (int i = 0; i < NUM_OUTPUTS; i++) {
		outputs[i] = LOW;
	}

	sendOutputs();
}

/**
 * Transmits all output states to the output drivers
 */
void sendOutputs() {
	for (int i = NUM_OUTPUTS; i > -1; i--) {
		digitalWrite(SR_OUTPUT, outputs[i]);
		digitalWrite(SR_SCK, HIGH);
		delayMicroseconds(10);
		digitalWrite(SR_SCK, LOW);
		delayMicroseconds(10);
	}

	digitalWrite(SR_RCK, HIGH);
	delayMicroseconds(10);
	digitalWrite(SR_RCK, LOW);
	delayMicroseconds(10);
}

void setup(void) {
	int i = 0;

	initOutputs();

	initBitlash(115200);
	DMXSerial2.init(&rdmInit, processCommand, DmxModePin, HIGH, LOW);

	for (i = 0; i < sizeof(inputPins); i++) {
		pinMode(pgm_read_byte(&inputPins[i]), INPUT);

		debouncers[i].setPin(pgm_read_byte(&inputPins[i]));
		debouncers[i].setInterval(10);
	}

	addBitlashFunction("setoutputstate", (bitlash_function) bl_setOutputState);
	addBitlashFunction("sos", (bitlash_function) bl_setOutputState);

	addBitlashFunction("getdevicename", (bitlash_function) bl_getDeviceName);
	addBitlashFunction("setdevicename", (bitlash_function) bl_setDeviceName);
	addBitlashFunction("getdmxstartaddress",
			(bitlash_function) bl_getDMXStartAddress);
	addBitlashFunction("getrdmuid", (bitlash_function) bl_getRDMUID);
	addBitlashFunction("setoutputname", (bitlash_function) bl_setOutputName);
	addBitlashFunction("getoutputname", (bitlash_function) bl_getOutputName);
	addBitlashFunction("getoutputstate", (bitlash_function) bl_getOutputState);
	addBitlashFunction("setinputname", (bitlash_function) bl_setInputName);
	addBitlashFunction("getinputname", (bitlash_function) bl_getInputName);
	addBitlashFunction("setinputmode", (bitlash_function) bl_setInputMode);
	addBitlashFunction("getinputmode", (bitlash_function) bl_getInputMode);
	addBitlashFunction("listoutputs", (bitlash_function) bl_listOutputs);
	addBitlashFunction("listinputs", (bitlash_function) bl_listInputs);

	addBitlashFunction("toggleoutputstate",
			(bitlash_function) bl_toggleOutputState);
	addBitlashFunction("tos", (bitlash_function) bl_toggleOutputState);

	addBitlashFunction("anyoutputon", (bitlash_function) bl_anyOutputOn);
	addBitlashFunction("aoo", (bitlash_function) bl_anyOutputOn);

	Wire.begin();
	delay(10);
	/*  byte b = i2c_eeprom_read_byte(0x68, 0);
	 
	 Serial1.println(b);
	 
	 i2c_eeprom_write_byte(0x68, 0, 127);*/

	byte error, address;
	int nDevices;
	Serial1.println("Scanning...");

	nDevices = 0;
	for (address = 1; address < 127; address++) {
		// The i2c_scanner uses the return value of
		// the Write.endTransmisstion to see if
		// a device did acknowledge to the address.
		Wire.beginTransmission(address);
		error = Wire.endTransmission();

		if (error == 0) {
			Serial1.print("I2C device found at address 0x");
			if (address < 16)
				Serial1.print("0");
			Serial1.print(address, HEX);
			Serial1.println("  !");

			nDevices++;
		} else if (error == 4) {
			Serial1.print("Unknow error at address 0x");
			if (address < 16)
				Serial1.print("0");
			Serial1.println(address, HEX);
		}
	}
	if (nDevices == 0)
		Serial1.println("No I2C devices found\n");
	else
		Serial1.println("done\n");
}

void loop(void) {
	uint16_t i;
	bool outputMode;
	bool removeBlock = true;

	runBitlash();
	DMXSerial2.tick();

	looper++;

	// Only run every 255 ticks, this isn't critical
	if (looper == 255) {
		for (i = DMXSerial2.getStartAddress();
				i < DMXSerial2.getStartAddress() + NUM_OUTPUTS; i++) {
			if (DMXSerial2.read(i) > 127) {
				outputMode = true;
			} else {
				outputMode = false;
			}

			if (outputMode != getOutputState(i)) {
				removeBlock = false;
			}

		}

		if (removeBlock == true) {
			dmxInputBlocked = false;
		}

		if (!dmxInputBlocked) {
			for (i = DMXSerial2.getStartAddress();
					i < DMXSerial2.getStartAddress() + NUM_OUTPUTS; i++) {
				// Check if the received DMX data 
				if (DMXSerial2.read(i) > 127) {
					if (getOutputState(i) == 0) {
						setOutputState(i, 1);
					}
				} else {
					if (getOutputState(i) == 1) {
						setOutputState(i, 0);
					}
				}
			}
		}

		looper = 0;
	}
}

boolean processCommand(struct RDMDATA *rdm, uint16_t *nackReason) {
	byte CmdClass = rdm->CmdClass;     // command class
	uint16_t Parameter = rdm->Parameter;    // parameter ID
	int i,j;
	boolean handled = false;

// This is a sample of how to return some device specific data
	if (CmdClass == E120_GET_COMMAND) {
		switch (Parameter) {
		case SWAPINT(E120_DEFAULT_SLOT_VALUE):
			if (rdm->DataLength > 0) {
				*nackReason = E120_NR_FORMAT_ERROR;
			} else if (rdm->SubDev != 0) {
				*nackReason = E120_NR_SUB_DEVICE_OUT_OF_RANGE;
			} else {
				rdm->DataLength = 3 * NUM_OUTPUTS;
				for (i = 0; i < NUM_OUTPUTS; i++) {
					rdm->Data[i * 3] = 0;
					rdm->Data[(i * 3) + 1] = i;

					if (getOutputState(i + 1) == 1) {
						rdm->Data[(i * 3) + 2] = 255;
					} else {
						rdm->Data[(i * 3) + 2] = 0;
					}

				}
				handled = true;
			}

			break;

		case SWAPINT(E120_SLOT_DESCRIPTION):
			// Return the requested slot
			if (rdm->DataLength != 2) {
				*nackReason = E120_NR_FORMAT_ERROR;
				handled = false;
			} else if (rdm->SubDev != 0) {
				*nackReason = E120_NR_SUB_DEVICE_OUT_OF_RANGE;
			} else {
				i = READINT(rdm->Data);

				if (i < NUM_OUTPUTS) {
					// Copy the slot description
					// @todo stub

					WRITEINT(rdm->Data, i);
					for (j=0;j<MAX_LABEL_LENGTH;j++) {
						rdm->Data[j+2] = i2c_eeprom_read_byte(I2C_EEPROM_ADDRESS, EEPROM_OUTPUT_NAME_OFFSET + ((i-1)*MAX_LABEL_LENGTH)+j);
						Serial1.println(rdm->Data[j+2]);
						if (rdm->Data[j+2] == '\0') {
							break;
						}
					}
					rdm->DataLength = j+2;
					Serial1.println(j+2);
					handled = true;
				} else {
					*nackReason = E120_NR_FORMAT_ERROR;
					handled = false;
				}
			}

			break;

		case SWAPINT(E120_SLOT_INFO):
			if (rdm->DataLength > 0) {
				*nackReason = E120_NR_FORMAT_ERROR;
			} else if (rdm->SubDev != 0) {
				*nackReason = E120_NR_SUB_DEVICE_OUT_OF_RANGE;
			} else {
				rdm->DataLength = 5 * NUM_OUTPUTS;
				for (i = 0; i < NUM_OUTPUTS; i++) {
					WRITEINT(rdm->Data + (i * 5), i);
					rdm->Data[(i * 5) + 2] = 0;
					WRITEINT(rdm->Data + (i * 5) + 3, 0xFFFF); // SD_UNDEFINED

				}
				handled = true;
			}

			break;
		}
	}

	return handled;
} // processCommand

