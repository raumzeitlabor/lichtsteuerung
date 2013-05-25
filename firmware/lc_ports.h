#include <stdlib.h>
#include <stdint.h>

#ifndef LC_PORT_H
#define	LC_PORT_H

struct lc_output {
    uint8_t state:1;    // false/0 = off, true/1 = on
};

struct lc_input {
    uint8_t state:1;    // false/0 = off, true/1 = on
};

#define NUM_OUTPUTS 32
#define NUM_INPUTS  16

#endif	/* PORT_H */

