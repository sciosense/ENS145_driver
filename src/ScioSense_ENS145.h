/*
  ScioSense_ENS145.h - Library for the ENS145 Development Shield

  2021 Jan 24   v0.3  Christoph Friese
  2021 Jan 21   v0.2  Patryk Hallek
  2020 Dec 08   v0.1  Patryk Hallek   Created

*/

#ifndef __SCIOSENSE_ENS145_H_
#define __SCIOSENSE_ENS145_H_

#if (ARDUINO >= 100)
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include <Adafruit_I2CDevice.h>
#include <Wire.h>

// Chip constants
#define ENS145_HP1							1
#define ENS145_HP3							3
#define ENS145_SHUNT_RH1					100			// shunt resistor in Ohms for heater current measurements  
#define ENS145_SHUNT_RH3					100			// shunt resistor in Ohms for heater current measurements  
#define ENS145_SHUNT_RS1					1000000		// shunt resistor in Ohms for sensing layer current measurement
#define ENS145_SHUNT_RS3					100000		// shunt resistor in Ohms for sensing layer current measurement
#define ENS145_REFERENCE_VOLTAGE			3300
#define ENS145_DAC_RESOLUTION 				4095
#define ENS145_ADC_RESOLUTION 				1024

#define ENS145_ENABLE_PIN_RS1				5
#define ENS145_ENABLE_PIN_RS3				12
#define ENS145_ADC_CHANNEL_RH1				A5
#define ENS145_ADC_CHANNEL_RS1				A4
#define ENS145_ADC_CHANNEL_RH3				A3
#define ENS145_ADC_CHANNEL_RS3				A2
#define NEO_PIXEL_DATA_INPUT_PIN			9

// 7-bit I2C slave address of the MCP4725 DACs
#define ENS145_I2CADDR_HP1_DAC        		0x60	// A0 low
#define ENS145_I2CADDR_HP3_DAC        	 	0x61	// A0 high

class ScioSense_ENS145 {
		
	public:
	    ScioSense_ENS145();   			// Constructor 
		
		bool 				begin(bool debug);								// init i2c interface, get partID und uID. Returns false on I2C problems or wrong PART_ID.

		bool				setHeaterVoltage(uint8_t hotplate, uint16_t mVolts);

		uint16_t			measureHeaterVoltage(uint8_t hotplate);
		uint16_t			measureHeaterResistance(uint8_t hotplate);

		uint16_t			measureSensorVoltage(uint8_t hotplate);
		uint16_t			measureSensorResistance(uint8_t hotplate);

	private:
		uint16_t			mVoltsToDACvalue(uint16_t voltage);
		bool				setDACvalue(uint8_t i2cAddr, uint16_t dacValue);
		
		uint16_t			_voltHP[2];
		
		bool 				debugENS145 = false;
};


#endif
