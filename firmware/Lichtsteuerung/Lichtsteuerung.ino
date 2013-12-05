#include <EEPROM.h>
#include <avr/pgmspace.h>
#include <DMXSerial2.h>
#include <Bounce.h>
#include <bitlash.h>

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
  
  initBitlash(57600);
  DMXSerial2.init(&rdmInit, processCommand, DmxModePin, HIGH, LOW);

  for (i=0;i<sizeof(inputPins);i++) {
	  pinMode(pgm_read_byte(&inputPins[i]), INPUT);
	  
	  debouncers[i].setPin(pgm_read_byte(&inputPins[i]));
	  debouncers[i].setInterval(10);
  }

  addBitlashFunction("setoutputstate", (bitlash_function) bl_setOutputState);
  addBitlashFunction("getdevicename", (bitlash_function) bl_getDeviceName);
  addBitlashFunction("setdevicename", (bitlash_function) bl_setDeviceName);
  
}

void loop (void) {
	int i;
	runBitlash();
	DMXSerial2.tick();
  
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
	
	delay(100);
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

