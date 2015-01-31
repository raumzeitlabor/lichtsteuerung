// TODO: Backup EEPROM contents to I²C EEPROM
// TODO: Cleanup code
#include <EEPROM.h>
#include <avr/pgmspace.h>
#include <DMXSerial2.h>
//#include <Bounce.h>
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

#define I2C_EEPROM_ADDRESS 0x54

#define EEPROM_OUTPUT_NAME_OFFSET 0x00000000
#define EEPROM_INPUT_NAME_OFFSET  EEPROM_OUTPUT_NAME_OFFSET + (MAX_LABEL_LENGTH * NUM_OUTPUTS)
#define EEPROM_INPUT_MODE_OFFSET  EEPROM_INPUT_NAME_OFFSET + (MAX_LABEL_LENGTH * NUM_INPUTS)
#define EEPROM_OUTPUT_PRIVATE_FLAG_OFFSET EEPROM_INPUT_MODE_OFFSET+NUM_INPUTS
#define ACTLED 51
#define MAX_LABEL_LENGTH 32

const char privateChannelsMessage[] = "Private Channel Map";

#define PID_PRIVATE_CHANNELS 0x8000

volatile bool outputs[NUM_OUTPUTS];
volatile bool inputs[NUM_INPUTS];
volatile bool inputChanged[NUM_INPUTS];
bool dmxInputBlocked = false;

uint16_t looper = 0;
uint16_t actlooper = 0;
bool actledstate = LOW;

const uint16_t my_pids[] = { E120_SLOT_DESCRIPTION, E120_DEFAULT_SLOT_VALUE,
		E120_SLOT_INFO, PID_PRIVATE_CHANNELS };

struct RDMINIT rdmInit = { "raumzeitlabor.de", 1, "Lichtsteuerung",
NUM_OUTPUTS, // footprint
		(sizeof(my_pids) / sizeof(uint16_t)), my_pids, { 0x09, 0x7F, 0x23, 0x42,
				0x00, 0x00 }, E120_PRODUCT_CATEGORY_POWER, // Product category
		0x00000001UL // Software Version

		};

PROGMEM const prog_uchar inputPins[] = { 0, 1, 2, 3, 4, 5, 6, 7, 16, 17, 18, 19,
		20, 21, 22, 23 };

//Bounce debouncers[16];

/**
 * Initializes the output array and sets up the pins required for the shift register.
 */
void initOutputs() {
  pinMode(ACTLED, OUTPUT);
  
	pinMode(SR_OUTPUT, OUTPUT);
	pinMode(SR_SCK, OUTPUT);
	pinMode(SR_RCK, OUTPUT);
	pinMode(SR_OE, OUTPUT);

	digitalWrite(SR_OE, LOW);
	for (int i = 0; i < NUM_OUTPUTS; i++) {
		outputs[i] = HIGH;
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

	for (i = 0; i < sizeof(inputPins); i++) {
		pinMode(pgm_read_byte(&inputPins[i]), INPUT);
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
	addBitlashFunction("setprivateflag", (bitlash_function) bl_setPrivateFlag);

	addBitlashFunction("toggleoutputstate",
			(bitlash_function) bl_toggleOutputState);
	addBitlashFunction("tos", (bitlash_function) bl_toggleOutputState);

	addBitlashFunction("anyoutputon", (bitlash_function) bl_anyOutputOn);
	addBitlashFunction("aoo", (bitlash_function) bl_anyOutputOn);

	Wire.begin();
	delay(10);
	
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

void handleInput () {
    int i;
    bool j;
    
    for (i = 0; i < sizeof(inputPins); i++) {
        j = digitalRead(pgm_read_byte(&inputPins[i]));
        
        if (j != inputs[i] && inputChanged[i] == 0) {
            inputChanged[i] = true;
        }
        
        inputs[i] = j;
    }
}
void loop(void) {
    String functionName = "onInput";
    char lbuf[128];
    int i;
    
    runBitlash();
    handleInput();
    actlooper++;
    
    if (actlooper==1024) {
      actlooper = 0;
      actledstate = !actledstate;
      digitalWrite(ACTLED, actledstate);
      
      for(i=0;i<NUM_INPUTS;i++) {
        if (inputChanged[i]) {
            inputChanged[i] = 0; 
           
            functionName = "oninput";
            functionName.concat(i+1);
            
            if (findscript((char*)functionName.c_str())) {
              doCommand((char*)functionName.c_str());
            }
            
            if (inputs[i]) {
              functionName.concat("on");
            } else {
              functionName.concat("off");
            }
      
            if (findscript((char*)functionName.c_str())) {
              doCommand((char*)functionName.c_str());
            }
            
            
        }
      }

    }

}

