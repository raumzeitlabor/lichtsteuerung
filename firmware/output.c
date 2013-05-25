#include <util/delay.h>
#include "output.h"
#include "compat.h"
#include <stdbool.h>
#include "avr64_ports.h"
#include "lc_ports.h"

/**
 * Holds the output data.
 * 
 * Requires 32 bytes
 */
struct lc_output lc_outputs[NUM_OUTPUTS];
struct lc_input lc_inputs[NUM_INPUTS];

/**
 * Shifts the contents of the outputs.
 */
void send_to_output() {
    uint8_t i;

    // We need to count backwards, because the order of the 74HC595 shift
    // registers is wrong (25-32 is the first shift register, but the last
    // in the chain).
    
    for (i = NUM_OUTPUTS;i != 0; i--) {
        if (lc_outputs[i].state) {
            sbi(SR_OUTPUT_PORT, SR_OUTPUT_PIN);
        } else {
            cbi(SR_OUTPUT_PORT, SR_OUTPUT_PIN);
        }
        
        // Toggle pin to shift data
        sbi(SR_SCK_PORT, SR_SCK_PIN);
        _delay_us(100);
        cbi(SR_SCK_PORT, SR_SCK_PIN);
        _delay_us(100);
    }
    
    // Toggle pin to 
    _delay_us(100);
    sbi(SR_RCK_PORT, SR_RCK_PIN);
    _delay_us(100);
    cbi(SR_RCK_PORT, SR_RCK_PIN);
}

/*
 * Initializes the output shift registers ports on the µC.
 * 
 */
void initOutputShiftRegister() {
    sbi(SR_OUTPUT_DPORT, SR_OUTPUT_PIN);
    sbi(SR_SCK_DPORT, SR_SCK_PIN);
    sbi(SR_RCK_DPORT, SR_RCK_PIN);
}

/**
 * Activates the given output
 * @param uint8_t Output number to activate
 */
void activateOutput (uint8_t output) {
    
    output--;
    if (output > NUM_OUTPUTS) {
        return;
    }
    lc_outputs[output].state = true;
}

/**
 * Deactivates the given output
 * @param output Output number to deactivate
 */
void deactivateOutput (uint8_t output) {
    output--;
    if (output > NUM_OUTPUTS) {
        return;
    }

    lc_outputs[output].state = false;
}