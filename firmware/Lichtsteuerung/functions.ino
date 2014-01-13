/**
 * Sets the output state for a specific port
 * 
 * @param port int Port (1-32)
 * @param state bool false:off, true:on
 */
boolean setOutputState (uint8_t port, boolean state) {
	// Block the DMX inputs as soon as a state change on the port occurs;
	// even if the state change is triggered by a DMX update.
	dmxInputBlocked = true;
	
	if (port < 1 || port > 32) {
		return false;
	}
	outputs[port-1] = state;
	sendOutputs();
	return true;
}

boolean getOutputState (uint8_t port) {
	if (port < 1 || port > 32) {
		return false;
	}
	
	return outputs[port-1];
}

void PrintHex8(uint8_t *data, uint8_t length) // prints 8-bit data in hex
{
	 char tmp[length*2+1];
	  byte first ;
	  int j=0;
	  for (uint8_t i=0; i<length; i++) 
	  {
	    first = (data[i] >> 4) | 48;
	    if (first > 57) tmp[j] = first + (byte)39;
	    else tmp[j] = first ;
	    j++;

	    first = (data[i] & 0x0F) | 48;
	    if (first > 57) tmp[j] = first + (byte)39; 
	    else tmp[j] = first;
	    j++;
	  }
	  tmp[length*2] = 0;

      Serial1.print(tmp);
}