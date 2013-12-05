#include <avr/pgmspace.h>

#define SETOUTPUTSTATE 1

char buffer[120];

const char usage_setOutputState[] PROGMEM = "Usage: setOutputState(output,state)\n\routput: Output Number (1-32)\n\rstate: State (0 = off, 1 = on)\n\r";
const char usage_setDeviceName[] PROGMEM = "Usage: setDeviceName(deviceName)\n\rdeviceName: ASCII String, 32 chars max\n\r";

PGM_P const usage[] PROGMEM = {
	usage_setOutputState,
	usage_setDeviceName
};

const char messages_setDeviceName[] PROGMEM = "New device name: ";

PGM_P const messages[] PROGMEM = {
	messages_setDeviceName
};

/**
 * bitlash function to set or clear an output.
 *
 * Usage: setOutputState [output]
 *
 * [output]: 1-32
 */
numvar bl_setOutputState(void) {
	if (getarg(0) != 2) {
		
		strcpy_P(buffer, (PGM_P)pgm_read_word(&(usage[0])));
		Serial1.println(buffer);
		return 0;
    }
    
    int i = getarg(1);
    if (i<1 || i>32) {
       Serial1.print("Port ");
       Serial1.print(i);
       Serial1.println(" out of range. Valid values are 1-32");
       return 0;
    }
    
    setOutputState(i, getarg(2));
}

/**
 * Prints the device name to the serial output
 *
 * Usage: getDeviceName
 */
numvar bl_getDeviceName(void) {
	Serial1.println(DMXSerial2.deviceLabel);
}

/**
 * Sets the device name. ASCII only, 32 bytes max. Additional length will be cropped.
 *
 * Usage: getDeviceName
 */
numvar bl_setDeviceName(void) {
	if (getarg(0) != 1) {
		strcpy_P(buffer, (PGM_P)pgm_read_word(&(usage[1])));
		Serial1.println((char *)buffer);
		return 0;
	}
	
	// Copy the name to the device label and save the EEPROM.
	strncpy(DMXSerial2.deviceLabel,(char *)getarg(1),32);
	DMXSerial2._saveEEPRom();
	
	// Output confirmation message
	strcpy_P(buffer, (PGM_P)pgm_read_word(&(messages[0])));
	Serial1.print(buffer);
	Serial1.println(DMXSerial2.deviceLabel);
	
}