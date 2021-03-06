# RaumZeitLabor Lichtsteuerung Firmware

This is the firmware for the RaumZeitLabor Lichtsteuerung.

## Build Prerequisites

* Arduino 1.5 IDE
* DMXSerial2 from https://github.com/raumzeitlabor/DMXSerial2
* bitlash from https://github.com/raumzeitlabor/bitlash
* The Arduino Board definition from https://github.com/raumzeitlabor/lichtsteuerung-arduino

## Quick Install

### Installing Arduino 1.5 on Debian

```bash
echo "deb http://ftp.de.debian.org/debian/ experimental main non-free contrib" >> /etc/apt/sources.list
apt-get update
apt-get -t experimental arduino
```

### Installing required libraries

```bash
cd ~/Arduino/libraries
git clone https://github.com/raumzeitlabor/DMXSerial2.git
git clone https://github.com/raumzeitlabor/bitlash.git
mkdir ~/Arduino/hardware && cd ~/Arduino/hardware
git clone https://github.com/raumzeitlabor/lichtsteuerung-arduino.git
```

## Flashing

A small makefile is provided which just calls the arduino IDE and uploads the code.

```bash
make verfiy
```

or

```bash
make flash
```
## Problems

* avrdude 6.0.1 doesn't work due to a bug; the result will be a verification error. Use avrdude 5.1 or something like that.
