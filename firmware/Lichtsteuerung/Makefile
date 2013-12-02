# Very simple makefile which calls the arduino IDE in order to verify and upload code

CODE=`readlink -f Lichtsteuerung.ino`

verify:
	arduino --board lichtsteuerung-arduino:avr:lichtsteuerung --verify $(CODE)

flash:
	arduino --board lichtsteuerung-arduino:avr:lichtsteuerung --upload $(CODE)