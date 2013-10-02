This is the firmware, written in arduino.

Requires the arduino corefiles for the atmega64 [1]. Use the board "Arduino-Cerebot II" with the following settings in the boards.txt file:

extras.name=EXTRAS==============================

##############################################################
#	Added for atmega64 (Digilent Cerebot II)
#	by Mark Sproul Sep 2010
arduino_cerebotii.name=Arduino-Cerebot II atemga64

arduino_cerebotii.upload.protocol=usbasp
arduino_cerebotii.upload.maximum_size=57344
arduino_cerebotii.upload.speed=115200

arduino_cerebotii.bootloader.low_fuses=0xEE
arduino_cerebotii.bootloader.high_fuses=0xD9
arduino_cerebotii.bootloader.extended_fuses=0xFF
arduino_cerebotii.bootloader.path=atmega
arduino_cerebotii.bootloader.file=atemga64
arduino_cerebotii.bootloader.unlock_bits=0x3F
arduino_cerebotii.bootloader.lock_bits=0x0F

arduino_cerebotii.build.mcu=atmega64
arduino_cerebotii.build.f_cpu=16000000L
arduino_cerebotii.build.core=arduino




[1] http://www.avr-developers.com/corefiles/
