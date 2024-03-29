# ScioSense ENS145 Development Shield

> :warning: This library is deprecated and no longer maintained.

Arduino library for the ENS145 Development Shield (analog MOX gas sensor) from ScioSense
![ENS145 Development Shield](https://github.com/sciosense/ENS145_driver/blob/main/ENS145_Dev_Shield_Full.JPG)

## Introduction
This project is an Arduino *library*. It implements a driver with examples for the ENS145 Development Shield.
The ENS145 is a analog gas sensor.
The driver in this Arduino library is based on the code supplied by *Sciosense*, the manufacturer of the chip.

## Links
The ENS145 is made by [Sciosense](http://www.sciosense.com).
 - The datasheet of the ENS145 is not yet released but can be provided on request

## Prerequisites
It is assumed that
 - The Arduino IDE has been installed.
   If not, refer to "Install the Arduino Desktop IDE" on the
   [Arduino site](https://www.arduino.cc/en/Guide/HomePage).
 - The library directory is at its default location.
   For me, that is `C:\Users\sciosense\Documents\Arduino\libraries`.

## Installation
- Visit the project page for the Arduino ENS145 library.
- Click the green button Clone or download on the right side.
- From the pop-up choose Download ZIP.
- In Arduino IDE, select Sketch > Include Library > Manage Libraries ... and browse to the just downloaded ZIP file.
- When the IDE is ready this README.md should be located at e.g. `C:\Users\sciosense\Documents\Arduino\libraries\ScioSense_ens145\README.md`.

## Build an example
To build an example sketch
 - (Re)start Arduino.
 - Open File > Example > Examples from Custom Libraries > ENS145 > ENS145_basic_read
 - Make sure Tools > Board lists the correct board.
 - Select Sketch > Verify/Compile.

### ScioSense is a Joint Venture of ams AG
