/**
 * Sets the output state for a specific port
 * 
 * @param port int Port (1-32)
 * @param state bool false:off, true:on
 */
boolean setOutputState (uint8_t port, boolean state) {
	if (port < 1 || port > 32) {
		return false;
	}
	outputs[port-1] = state;
	sendOutputs();
	return true;
}