#include <avr/pgmspace.h>

#define MODE_TOGGLE 1
#define MODE_MOMENTARY 2
#define MODE_INVALID 99

char buffer[120];

const char usage_setOutputState[] PROGMEM = "Usage: setOutputState(output,state)\n\routput: Output Number (1-32)\n\rstate: State (0 = off, 1 = on)\n\r";
const char usage_setDeviceName[] PROGMEM = "Usage: setDeviceName(deviceName)\n\rdeviceName: ASCII String, 32 chars max\n\r";
const char usage_setOutputName[] PROGMEM = "Usage: setOutputName(port, portName)\n\rport: Output Number (1-32)\n\rportName: ASCII String, 32 chars max\n\r";
const char usage_getOutputName[] PROGMEM = "Usage: getOutputName(port)\n\rport: Output Number (1-32)\n\r";
const char usage_getOutputState[] PROGMEM = "Usage: getOutputState(port)\n\rport: Output Number (1-32)\n\r";
const char usage_toggleOutputState[] PROGMEM = "Usage: toggleOutputState(port)\n\rport: Output Number (1-32)\n\r";
const char usage_setInputName[] PROGMEM = "Usage: setInputName(port, portName)\n\rport: Input Number (1-16)\n\rportName: ASCII String, 32 chars max\n\r";
const char usage_getInputName[] PROGMEM = "Usage: getInputName(port)\n\rport: Input Number (1-16)\n\r";
const char usage_setInputMode[] PROGMEM = "Usage: setInputMode(port,mode)\n\rport: Input Number (1-16)\n\rmode: MODE_TOGGLE or MODE_MOMENTARY\n\rExample: setInputMode(1, \"MODE_MOMENTARY\");\n\r";
const char usage_getInputMode[] PROGMEM = "Usage: getInputMode(port)\n\rport: Input Number (1-16)\n\Prints either MODE_TOGGLE or MODE_MOMENTARY\n\r";


uint8_t inputModes[NUM_INPUTS];

                   
PGM_P const usage[] PROGMEM = {
	usage_setOutputState,
	usage_setDeviceName,
	usage_setOutputName,
	usage_getOutputName,
	usage_getOutputState,
	usage_toggleOutputState,
	usage_setInputName,
	usage_getInputName,
	usage_setInputMode,
	usage_getInputMode
};

const char messages_setDeviceName[] PROGMEM = "New device name: ";
const char messages_invalidOutputPortNumber[] PROGMEM = "Invalid Output Port Number. Valid values are: 1-32.";
const char messages_invalidInputPortNumber[] PROGMEM = "Invalid Input Port Number. Valid values are: 1-16.";
const char messages_invalidInputMode[] PROGMEM = "Invalid port mode. Valid values are: MODE_TOGGLE or MODE_MOMENTARY.";

PGM_P const messages[] PROGMEM = {
	messages_setDeviceName,
	messages_invalidOutputPortNumber,
	messages_invalidInputPortNumber,
	messages_invalidInputMode
};

/**
 * bitlash function to set or clear an output.
 *
 * Usage: setOutputState [output] [state]
 *
 * [output]: 1-32
 * [state]: 0 for off, 1 for on
 */
numvar bl_setOutputState(void) {
	if (getarg(0) != 2) {
		strcpy_P(buffer, (PGM_P) pgm_read_word(&(usage[0])));
		Serial1.println(buffer);
		return 0;
	}

	int i = getarg(1);
	if (i < 1 || i > NUM_OUTPUTS) {
		// Output out of range error message
		strcpy_P(buffer, (PGM_P) pgm_read_word(&(messages[1])));
		Serial1.print(buffer);
		return 0;
	}

	setOutputState(i, getarg(2));
}

/**
 * bitlash function to return (not print) the output state.
 *
 * Usage: getOutputState [output]
 *
 * [output]: 1-32
 */
numvar bl_getOutputState(void) {
	if (getarg(0) != 1) {
		strcpy_P(buffer, (PGM_P) pgm_read_word(&(usage[4])));
		Serial1.println(buffer);
		return 0;
	}

	int i = getarg(1);
	if (i < 1 || i > NUM_OUTPUTS) {
		// Output out of range error message
		strcpy_P(buffer, (PGM_P) pgm_read_word(&(messages[1])));
		Serial1.print(buffer);
		return 0;
	}

	return getOutputState(i);
}

/**
 * bitlash function to toggle the output state.
 *
 * Usage: toggleOutputState [output]
 *
 * [output]: 1-32
 */
numvar bl_toggleOutputState(void) {
	if (getarg(0) != 1) {
		strcpy_P(buffer, (PGM_P) pgm_read_word(&(usage[5])));
		Serial1.println(buffer);
		return 0;
	}

	int i = getarg(1);
	if (i < 1 || i > NUM_OUTPUTS) {
		// Output out of range error message
		strcpy_P(buffer, (PGM_P) pgm_read_word(&(messages[1])));
		Serial1.print(buffer);
		return 0;
	}

	setOutputState(i, !getOutputState(i));
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
 * Usage: setDeviceName [name]
 */
numvar bl_setDeviceName(void) {
	if (getarg(0) != 1) {
		strcpy_P(buffer, (PGM_P) pgm_read_word(&(usage[1])));
		Serial1.println((char *) buffer);
		return 0;
	}

	// Copy the name to the device label and save the EEPROM.
	strncpy(DMXSerial2.deviceLabel, (char *) getarg(1), 32);
	DMXSerial2._saveEEPRom();

	// Output confirmation message
	strcpy_P(buffer, (PGM_P) pgm_read_word(&(messages[0])));
	Serial1.print(buffer);
	Serial1.println(DMXSerial2.deviceLabel);
}

/**
 * Prints the DMX start address.
 *
 * Usage: getDMXStartAddress
 */
numvar bl_getDMXStartAddress(void) {
	Serial1.println(DMXSerial2.getStartAddress());
}

/**
 * Prints the RDM UID on the serial output
 *
 * Usage: getRDMUID
 */
numvar bl_getRDMUID(void) {
	byte uid[6];
	DMXSerial2.getDeviceUID(uid);
	PrintHex8(uid, 2);
	Serial1.print(":");
	PrintHex8(uid + 2, 4);
	Serial1.println();
}

/**
 * Sets the output port name. ASCII only, 32 bytes max. Additional length will be cropped.
 *
 * Usage: setOutputName [port] [name]
 */
numvar bl_setOutputName(void) {
	if (getarg(0) != 2) {
		strcpy_P(buffer, (PGM_P) pgm_read_word(&(usage[2])));
		Serial1.println((char *) buffer);
		return 0;
	}

	int i = getarg(1);
	if (i < 1 || i > NUM_OUTPUTS) {
		// Output out of range error message
		strcpy_P(buffer, (PGM_P) pgm_read_word(&(messages[1])));
		Serial1.print(buffer);
		return 0;
	}
	
	Serial1.println("NOT IMPLEMENTED YET");
	// @todo Copy the name to the device label and save the EEPROM.
	//strncpy(DMXSerial2.deviceLabel, (char *) getarg(1), 32);
}

/**
 * Gets the output port name.
 *
 * Usage: getOutputName [port]
 */
numvar bl_getOutputName(void) {
	if (getarg(0) != 1) {
		strcpy_P(buffer, (PGM_P) pgm_read_word(&(usage[3])));
		Serial1.println((char *) buffer);
		return 0;
	}

	int i = getarg(1);
	if (i < 1 || i > NUM_OUTPUTS) {
		// Output out of range error message
		strcpy_P(buffer, (PGM_P) pgm_read_word(&(messages[1])));
		Serial1.print(buffer);
		return 0;
	}
	
	Serial1.println("NOT IMPLEMENTED YET");
	// Copy the name to the device label and save the EEPROM.
	// @todo strncpy(DMXSerial2.deviceLabel, (char *) getarg(1), 32);
}

/**
 * Sets the input port name. ASCII only, 32 bytes max. Additional length will be cropped.
 *
 * Usage: setInputName [port] [name]
 */
numvar bl_setInputName(void) {
	if (getarg(0) != 2) {
		strcpy_P(buffer, (PGM_P) pgm_read_word(&(usage[6])));
		Serial1.println((char *) buffer);
		return 0;
	}

	int i = getarg(1);
	if (i < 1 || i > NUM_INPUTS) {
		// Output out of range error message
		strcpy_P(buffer, (PGM_P) pgm_read_word(&(messages[1])));
		Serial1.print(buffer);
		return 0;
	}
	
	Serial1.println("NOT IMPLEMENTED YET");
	// Copy the name to the device label and save the EEPROM.
	// @todo strncpy(DMXSerial2.deviceLabel, (char *) getarg(1), 32);
}

/**
 * Gets the input port name.
 *
 * Usage: getInputName [port]
 */
numvar bl_getInputName(void) {
	if (getarg(0) != 1) {
		strcpy_P(buffer, (PGM_P) pgm_read_word(&(usage[7])));
		Serial1.println((char *) buffer);
		return 0;
	}

	int i = getarg(1);
	if (i < 1 || i > NUM_INPUTS) {
		// Output out of range error message
		strcpy_P(buffer, (PGM_P) pgm_read_word(&(messages[2])));
		Serial1.print(buffer);
		return 0;
	}
	
	Serial1.println("NOT IMPLEMENTED YET");
	//@todo Copy the name to the device label and save the EEPROM.
	//strncpy(DMXSerial2.deviceLabel, (char *) getarg(1), 32);
}

numvar bl_setInputMode(void) {
	uint8_t mode = MODE_INVALID;
	
	if (getarg(0) != 2) {
		strcpy_P(buffer, (PGM_P) pgm_read_word(&(usage[8])));
		Serial1.println((char *) buffer);
		return 0;
	}
	
	int i = getarg(1);
	if (i < 1 || i > NUM_INPUTS) {
		// Output out of range error message
		strcpy_P(buffer, (PGM_P) pgm_read_word(&(messages[2])));
		Serial1.print(buffer);
		return 0;
	}
	
	if (strcmp((char*)getarg(2), "MODE_TOGGLE") == 0) {
		mode = MODE_TOGGLE;
		// Mode will be toggle
	}
	
	if (strcmp((char*)getarg(2), "MODE_MOMENTARY") == 0) {
		mode = MODE_MOMENTARY;
	}
	
	if (mode == MODE_INVALID) {
		strcpy_P(buffer, (PGM_P) pgm_read_word(&(messages[3])));
		Serial1.print(buffer);
		return 0;
	}
	
	inputModes[i] = mode;
	//@todo save to flash
}

numvar bl_getInputMode(void) {
	if (getarg(0) != 1) {
		strcpy_P(buffer, (PGM_P) pgm_read_word(&(usage[9])));
		Serial1.println((char *) buffer);
		return 0;
	}
	
	int i = getarg(1);
	if (i < 1 || i > NUM_INPUTS) {
		// Output out of range error message
		strcpy_P(buffer, (PGM_P) pgm_read_word(&(messages[2])));
		Serial1.print(buffer);
		return 0;
	}
	
	if (inputModes[i] == MODE_MOMENTARY) {
		Serial1.println("MODE_MOMENTARY");
	} else if (inputModes[i] == MODE_TOGGLE) {
		Serial1.println("MODE_TOGGLE");
	} else {
		Serial1.println("Unknown mode");
	}
}


numvar bl_anyOutputOn (void) {
	uint8_t i;
	
	for (i=0; i<getarg(0);i++) {
		if (getarg(i+1) < 1 || getarg(i+1) > NUM_INPUTS) {
			// Output out of range error message
			strcpy_P(buffer, (PGM_P) pgm_read_word(&(messages[2])));
			Serial1.print(buffer);
			return 0;
		}
	}

	for (i=0; i<getarg(0);i++) {
		if (getOutputState(getarg(i+1)) == 1) {
			return true;
		}
	}
	
	return false;
}

numvar bl_listOutputs (void) {
	uint8_t i;
	
	for (i=1;i<NUM_OUTPUTS+1;i++) {
		Serial1.print("#");
		Serial1.print(i);
		Serial1.print(": ");
		if (getOutputState(i) == 1) {
			Serial1.print("ON");
		} else {
			Serial1.print("OFF");
		}
		
		Serial1.println("  UNKNOWN NAME"); // @todo retrieve the name
	}
}
// @todo: listOutputs, listInputs, onInput(N)