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

bool outputs[NUM_OUTPUTS];
bool dmxInputBlocked = false;

uint8_t looper = 0;

struct RDMINIT rdmInit = {
  "raumzeitlabor.de",
  "Lichtsteuerung",
  32, // footprint
  3, (uint16_t[]){SWAPINT(E120_DEVICE_HOURS), SWAPINT(E120_LAMP_HOURS),SWAPINT(E120_DEFAULT_SLOT_VALUE)}
};

PROGMEM const prog_uchar inputPins[] = {
	0,
	1,
	2,
	3,
	4,
	5,
	6,
	7,
	16,
	17,
	18,
	19,
	20,
	21,
	22,
	23
};

Bounce debouncers[16];

/**
 * Initializes the output array and sets up the pins required for the shift register.
 */
void initOutputs () {
    pinMode(SR_OUTPUT, OUTPUT);
    pinMode(SR_SCK, OUTPUT);
    pinMode(SR_RCK, OUTPUT);
    pinMode(SR_OE, OUTPUT);
    
    digitalWrite(SR_OE, LOW);
    for (int i=0;i<NUM_OUTPUTS;i++) {
       outputs[i] = LOW; 
    }
    
    sendOutputs();
}

/**
 * Transmits all output states to the output drivers
 */
void sendOutputs () {
   for (int i=NUM_OUTPUTS;i>-1;i--) {
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

void setup (void) {
  int i=0;
  
  initOutputs();
  
  initBitlash(115200);
  DMXSerial2.init(&rdmInit, processCommand, DmxModePin, HIGH, LOW);

  for (i=0;i<sizeof(inputPins);i++) {
	  pinMode(pgm_read_byte(&inputPins[i]), INPUT);
	  
	  debouncers[i].setPin(pgm_read_byte(&inputPins[i]));
	  debouncers[i].setInterval(10);
  }

  addBitlashFunction("setoutputstate", (bitlash_function) bl_setOutputState);
  addBitlashFunction("sos", (bitlash_function) bl_setOutputState);
  
  addBitlashFunction("getdevicename", (bitlash_function) bl_getDeviceName);
  addBitlashFunction("setdevicename", (bitlash_function) bl_setDeviceName);
  addBitlashFunction("getdmxstartaddress", (bitlash_function) bl_getDMXStartAddress);
  addBitlashFunction("getrdmuid", (bitlash_function) bl_getRDMUID);
  addBitlashFunction("setoutputname", (bitlash_function) bl_setOutputName);
  addBitlashFunction("getoutputname", (bitlash_function) bl_getOutputName);
  addBitlashFunction("getoutputstate", (bitlash_function) bl_getOutputState);
  addBitlashFunction("setinputname", (bitlash_function) bl_setInputName);
  addBitlashFunction("getinputname", (bitlash_function) bl_getInputName);
  addBitlashFunction("setinputmode", (bitlash_function) bl_setInputMode);
  addBitlashFunction("getinputmode", (bitlash_function) bl_getInputMode);
  addBitlashFunction("listoutputs", (bitlash_function) bl_listOutputs);
  
  
  addBitlashFunction("toggleoutputstate", (bitlash_function) bl_toggleOutputState);
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
    for(address = 1; address < 127; address++ ) 
    {
      // The i2c_scanner uses the return value of
      // the Write.endTransmisstion to see if
      // a device did acknowledge to the address.
      Wire.beginTransmission(address);
      error = Wire.endTransmission();

      if (error == 0)
      {
        Serial1.print("I2C device found at address 0x");
        if (address<16) 
          Serial1.print("0");
        Serial1.print(address,HEX);
        Serial1.println("  !");

        nDevices++;
      }
      else if (error==4) 
      {
        Serial1.print("Unknow error at address 0x");
        if (address<16) 
          Serial1.print("0");
        Serial1.println(address,HEX);
      } 
    }
    if (nDevices == 0)
      Serial1.println("No I2C devices found\n");
    else
      Serial1.println("done\n");

}

void loop (void) {
	uint16_t i;
	bool outputMode;
	bool removeBlock = true;
	
	runBitlash();
	DMXSerial2.tick();
	
	looper++;
	
	// Only run every 255 ticks, this isn't critical
	if (looper == 255) {
		for (i=DMXSerial2.getStartAddress();i<DMXSerial2.getStartAddress()+32;i++) {
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
			for (i=DMXSerial2.getStartAddress();i<DMXSerial2.getStartAddress()+32;i++) {
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

boolean processCommand(struct RDMDATA *rdm)
{
  byte CmdClass       = rdm->CmdClass;     // command class
  uint16_t Parameter  = rdm->Parameter;    // parameter ID
  int i;
  boolean handled = false;

// This is a sample of how to return some device specific data
  if ((CmdClass == E120_GET_COMMAND) && (Parameter == SWAPINT(E120_DEVICE_HOURS))) { // 0x0400
    rdm->DataLength = 4;
    rdm->Data[0] = 0;
    rdm->Data[1] = 0;
    rdm->Data[2] = 2;
    rdm->Data[3] = 0;
    handled = true;

  } else if ((CmdClass == E120_GET_COMMAND) && (Parameter == SWAPINT(E120_LAMP_HOURS))) { // 0x0401
    rdm->DataLength = 4;
    rdm->Data[0] = 0;
    rdm->Data[1] = 0;
    rdm->Data[2] = 0;
    rdm->Data[3] = 1;
    handled = true;
  } else if ((CmdClass == E120_GET_COMMAND) && (Parameter == SWAPINT(E120_DEFAULT_SLOT_VALUE))) {
	  rdm->DataLength = 3 * NUM_OUTPUTS;
	  for (i=0;i<32;i++) {
		  rdm->Data[i*3] = 0;
		  rdm->Data[(i*3)+1] = i;

                  if (getOutputState(i+1) == 1) {
                        rdm->Data[(i*3)+2] = 255;
                  } else {
                        rdm->Data[(i*3)+2] = 0;
                  }

	  }
	  
	  handled = true;
  } else if ((CmdClass == E120_GET_COMMAND) && (Parameter == SWAPINT(E120_SLOT_DESCRIPTION))) {
          // Return the requested slot
          i = READINT(rdm->Data);
          rdm->DataLength = 32;

          // Copy the slot description
          // @todo stub
          WRITEINT(rdm->Data, i);
          memcpy(rdm->Data+2, "DummySlot", 10);
	  handled = true;
  }

  return(handled);
} // processCommand

