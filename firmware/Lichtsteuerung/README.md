# RaumZeitLabor Lichtsteuerung Firmware

This is the firmware for the RaumZeitLabor Lichtsteuerung.

## Build Prerequisites

* Arduino 1.5 IDE
* DMXSerial2 from https://github.com/raumzeitlabor/DMXSerial2
* bitlash from https://github.com/raumzeitlabor/bitlash
* The Arduino Board definition from https://github.com/raumzeitlabor/lichtsteuerung-arduino

## Quick Install

### Building with arduino-mk

```bash
apt-get arduino-mk
# in root of repo
git clone https://github.com/raumzeitlabor/bitlash.git
# check the vars in Makefile
```
## Flashing

A small makefile is provided which just calls the arduino IDE and uploads the code.

```bash
make upload
```

## Problems

* avrdude 6.0.1 doesn't work due to a bug; the result will be a verification error. Use avrdude 5.1 or something like that.
