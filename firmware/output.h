#include "lc_ports.h"

#ifndef OUTPUT_H
#define	OUTPUT_H

extern struct lc_output lc_outputs[NUM_OUTPUTS];

void send_to_output(void);
void initOutputShiftRegister(void);


#endif	/* OUTPUT_H */

