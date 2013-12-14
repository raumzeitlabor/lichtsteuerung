#include <avr/pgmspace.h>

#define SETOUTPUTSTATE 1

char buffer[120];

const char usage_setOutputState[] PROGMEM = "Usage: setOutputState(output,state)\n\routput: Output Number (1-32)\n\rstate: State (0 = off, 1 = on)\n\r";
const char usage_setDeviceName[] PROGMEM = "Usage: setDeviceName(deviceName)\n\rdeviceName: ASCII String, 32 chars max\n\r";
const char usage_setOutputName[] PROGMEM = "Usage: setOutputName(port, portName)\n\rport: Output Number (1-32)\n\rportName: ASCII String, 32 chars max\n\r";
const char usage_getOutputName[] PROGMEM = "Usage: getOutputName(port)\n\rport: Output Number (1-32)\n\r";
const char usage_getOutputState[] PROGMEM = "Usage: getOutputState(port)\n\rport: Output Number (1-32)\n\r";
const char usage_toggleOutputState[] PROGMEM = "Usage: toggleOutputState(port)\n\rport: Output Number (1-32)\n\r";
const char usage_setInputName[] PROGMEM = "Usage: setInputName(port, portName)\n\rport: Input Number (1-16)\n\rportName: ASCII String, 32 chars max\n\r";
const char usage_getInputName[] PROGMEM = "Usage: getInputName(port)\n\rport: Input Number (1-16)\n\r";

PGM_P const usage[] PROGMEM = {
	usage_setOutputState,
	usage_setDeviceName,
	usage_setOutputName,
	usage_getOutputName,
	usage_getOutputState,
	usage_toggleOutputState,
	usage_setInputName,
	usage_getInputName
};

const char messages_setDeviceName[] PROGMEM = "New device name: ";
const char messages_invalidOutputPortNumber[] PROGMEM = "Invalid Output Port Number. Valid values are: 1-32.";
const char messages_invalidInputPortNumber[] PROGMEM = "Invalid Input Port Number. Valid values are: 1-16.";

PGM_P const messages[] PROGMEM = {
	messages_setDeviceName,
	messages_invalidOutputPortNumber,
	messages_invalidInputPortNumber
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
	if (i < 1 || i > 32) {
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
	if (i < 1 || i > 32) {
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
	if (i < 1 || i > 32) {
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
	if (i < 1 || i > 32) {
		// Output out of range error message
		strcpy_P(buffer, (PGM_P) pgm_read_word(&(messages[1])));
		Serial1.print(buffer);
		return 0;
	}
	
	Serial1.println("NOT IMPLEMENTED YET");
	// Copy the name to the device label and save the EEPROM.
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
	if (i < 1 || i > 32) {
		// Output out of range error message
		strcpy_P(buffer, (PGM_P) pgm_read_word(&(messages[1])));
		Serial1.print(buffer);
		return 0;
	}
	
	Serial1.println("NOT IMPLEMENTED YET");
	// Copy the name to the device label and save the EEPROM.
	//strncpy(DMXSerial2.deviceLabel, (char *) getarg(1), 32);
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
	if (i < 1 || i > 16) {
		// Output out of range error message
		strcpy_P(buffer, (PGM_P) pgm_read_word(&(messages[1])));
		Serial1.print(buffer);
		return 0;
	}
	
	Serial1.println("NOT IMPLEMENTED YET");
	// Copy the name to the device label and save the EEPROM.
	//strncpy(DMXSerial2.deviceLabel, (char *) getarg(1), 32);
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
	if (i < 1 || i > 16) {
		// Output out of range error message
		strcpy_P(buffer, (PGM_P) pgm_read_word(&(messages[2])));
		Serial1.print(buffer);
		return 0;
	}
	
	Serial1.println("NOT IMPLEMENTED YET");
	// Copy the name to the device label and save the EEPROM.
	//strncpy(DMXSerial2.deviceLabel, (char *) getarg(1), 32);
}

// @todo: setInputMode, getInputMode, listOutputs, listInputs, anyOutputOn, onInput(N)