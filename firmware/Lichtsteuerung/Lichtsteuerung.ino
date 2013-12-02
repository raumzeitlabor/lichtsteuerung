#include <EEPROM.h>
#include <DMXSerial2.h>

#include <bitlash.h>

numvar setOutput(void);
numvar clearOutput(void);
void initOutputs ();
void sendOutputs ();
void setup (void);
void loop (void);
boolean processCommand(struct RDMDATA *rdm);

#define DmxModePin 36
#define DmxModeOut LOW
#define DmxModeIn HIGH

#define SR_OUTPUT 39
#define SR_SCK 38
#define SR_RCK 37
#define SR_OE  13

#define NUM_OUTPUTS 32

bool outputs[NUM_OUTPUTS];

struct RDMINIT rdmInit = {
  "mathertel.de",
  "Arduino RDM Device",
  3, // footprint
  2, (uint16_t[]){SWAPINT(E120_DEVICE_HOURS), SWAPINT(E120_LAMP_HOURS)}
};


/**
 * bitlash function to set an output.
 *
 * Usage: setOutput [output]
 *
 * [output]: 1-32
 */
numvar setOutput(void) {
    if (getarg(0) != 1) {
       Serial1.println("Usage: setOutput [output]");
       return 0;
    }
    
    int i = getarg(1);
    if (i<1 || i>32) {
       Serial1.print("Port ");
       Serial1.print(i);
       Serial1.println(" out of range. Valid values are 1-32");
       return 0;
    }
    
    outputs[i-1] = HIGH;
    sendOutputs();
}

/**
 * bitlash function to clear an output.
 *
 * Usage: clearOutput [output]
 *
 * [output]: 1-32
 */
numvar clearOutput(void) {
    if (getarg(0) != 1) {
       Serial1.println("Usage: clearOutput [output]");
       return 0;
    }
    
    int i = getarg(1);
    if (i<1 || i>32) {
       Serial1.print("Port ");
       Serial1.print(i);
       Serial1.println(" out of range. Valid values are 1-32");
       return 0;
    }
    
    outputs[i-1] = LOW;
    sendOutputs();
}

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
  
  Serial1.begin(115200);
  Serial1.println("FOOBAR");

  DMXSerial2.init(&rdmInit, processCommand, 36, HIGH, LOW);
  
  pinMode(0, INPUT);
  pinMode(1, INPUT);

//  uint16_t start = DMXSerial2.getStartAddress();
  
//  Serial1.print("Listening on DMX address #"); Serial1.println(start);
  
  outputs[1] = HIGH;
  sendOutputs();
    
  addBitlashFunction("setoutput", (bitlash_function) setOutput);
  addBitlashFunction("clearoutput", (bitlash_function) clearOutput);
}

void loop (void) {
  //runBitlash();
  DMXSerial2.tick();
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

