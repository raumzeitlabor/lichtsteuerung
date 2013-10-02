#include <bitlash.h>

#define SR_OUTPUT 39
#define SR_SCK 38
#define SR_RCK 37
#define SR_OE  13

#define NUM_OUTPUTS 32

bool outputs[NUM_OUTPUTS];

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
  initOutputs();
  initBitlash(57600);

  addBitlashFunction("setoutput", (bitlash_function) setOutput);
  addBitlashFunction("clearoutput", (bitlash_function) clearOutput);
}

void loop (void) {
  runBitlash(); 
}
