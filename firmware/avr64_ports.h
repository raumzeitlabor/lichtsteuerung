#include <avr/io.h>

#ifndef PORTS_H
#define	PORTS_H

// Main Shift Register Output
#define SR_OUTPUT_PIN PE7
#define SR_OUTPUT_PORT PORTE
#define SR_OUTPUT_DPORT DDRE

// Shift Register clock
#define SR_SCK_PIN PE6
#define SR_SCK_PORT PORTE
#define SR_SCK_DPORT DDRE

// Shift Register RCK
#define SR_RCK_PIN PE5
#define SR_RCK_PORT PORTE
#define SR_RCK_DPORT DDRE

#endif	/* PORTS_H */

