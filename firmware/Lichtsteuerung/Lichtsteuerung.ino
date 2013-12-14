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

struct RDMINIT rdmInit = {
  "raumzeitlabor.de",
  "Lichtsteuerung",
  32, // footprint
  2, (uint16_t[]){SWAPINT(E120_DEVICE_HOURS), SWAPINT(E120_LAMP_HOURS)}
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
  addBitlashFunction("getdevicename", (bitlash_function) bl_getDeviceName);
  addBitlashFunction("setdevicename", (bitlash_function) bl_setDeviceName);
  addBitlashFunction("getdmxstartaddress", (bitlash_function) bl_getDMXStartAddress);
  addBitlashFunction("getrdmuid", (bitlash_function) bl_getRDMUID);
  addBitlashFunction("setoutputname", (bitlash_function) bl_setOutputName);
  addBitlashFunction("getoutputname", (bitlash_function) bl_getOutputName);
  addBitlashFunction("getoutputstate", (bitlash_function) bl_getOutputState);
  addBitlashFunction("toggleoutputstate", (bitlash_function) bl_toggleOutputState);
  addBitlashFunction("setinputname", (bitlash_function) bl_setInputName);
  addBitlashFunction("getinputname", (bitlash_function) bl_getInputName);
  
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
	int i;
	runBitlash();
	DMXSerial2.tick();
	return;
	for (i=0;i<sizeof(inputPins);i++) {
		debouncers[i].update();
		
		if (debouncers[i].fallingEdge()) {
			
			if (debouncers[i].previousDuration() > 500) {
				Serial1.println("LONG");
				
			}
			Serial1.print("INPUT ");
			Serial1.print(i);
			Serial1.print(" (");
			Serial1.print(pgm_read_byte(&inputPins[i]));
			Serial1.print(") ");
			Serial1.println("HIGH");
		}
	}
	
	//delay(100);
}

boolean processCommand(struct RDMDATA *rdm)
{
  byte CmdClass       = rdm->CmdClass;     // command class
  uint16_t Parameter  = rdm->Parameter;    // parameter ID
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
  } // if

  return(handled);
} // processCommand

