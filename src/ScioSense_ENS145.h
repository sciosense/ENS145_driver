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

#include <Wire.h>

// Chip constants
#define ENS145_HP1							0
#define ENS145_HP3							1
#define ENS145_SHUNT_RH1					100			// shunt resistor in Ohms for heater current measurements  
#define ENS145_SHUNT_RH3					100			// shunt resistor in Ohms for heater current measurements  
#define ENS145_SHUNT_RS1					1000000		// shunt resistor in Ohms for sensing layer current measurement
#define ENS145_SHUNT_RS3					100000		// shunt resistor in Ohms for sensing layer current measurement
#define ENS145_DAC_RESOLUTION 				4095		// 12bit DAC resolution of MCP4725 DACs
#define ENS145_DAC_REF_VOLT					3300
#define ENS145_ADC_RESOLUTION 				1024		// typical 10bit ADC resolution of Arduino board

#define ENS145_ENABLE_PIN_RS1				5
#define ENS145_ENABLE_PIN_RS3				12
#if (defined(ADAFRUIT_FEATHER_M0))
	#define ENS145_ADC_CHANNEL_RH1			A5
	#define ENS145_ADC_CHANNEL_RS1			A4
	#define ENS145_AREF						AR_EXTERNAL
	#define ENS145_REFERENCE_VOLTAGE		3300
#else 
	#define ENS145_ADC_CHANNEL_RH1			A1
	#define ENS145_ADC_CHANNEL_RS1			A0
	#define ENS145_AREF						EXTERNAL
	#define ENS145_REFERENCE_VOLTAGE		3300
#endif
	
#define ENS145_ADC_CHANNEL_RH3				A3
#define ENS145_ADC_CHANNEL_RS3				A2
#define NEO_PIXEL_DATA_INPUT_PIN			9

// 7-bit I2C slave address of the MCP4725 DACs
#define ENS145_I2CADDR_HP1_DAC        		0x60	// A0 low
#define ENS145_I2CADDR_HP3_DAC        	 	0x61	// A0 high

class ScioSense_ENS145 {
		
	public:
	    ScioSense_ENS145();   			// Constructor 
		
		bool 				begin(uint16_t adc, bool debug);
		bool 				available() { return this->_available; }
		
		void				setADCcorrection(float factor) { this->_adcCorrection = factor; }
		
		bool				setHeaterVoltage(uint8_t hotplate, uint16_t mVolts);
			
		bool				measure();

		uint16_t			getADCvalue(uint8_t element, uint8_t hotplate) { return this->_adcIn[element][hotplate]; }
		
		uint16_t			getHeaterVoltage(uint8_t hotplate) { return this->_voltIn[0][hotplate]; }
		uint16_t			getSensorVoltage(uint8_t hotplate) { return this->_voltIn[1][hotplate]; }

		uint32_t			getHeaterResistance(uint8_t hotplate) { return this->_resIn[0][hotplate]; }
		uint32_t			getSensorResistance(uint8_t hotplate) { return this->_resIn[1][hotplate]; }
		
	private:
		uint16_t			mVoltsToDACvalue(uint16_t voltage);
		bool				setDACvalue(uint8_t i2cAddr, uint16_t dacValue);
		
		bool				measureHeater(uint8_t hotplate);

		bool				measureSensor(uint8_t hotplate);
		
		uint8_t			    _addressDAC[2] 		= {ENS145_I2CADDR_HP1_DAC, ENS145_I2CADDR_HP3_DAC};
		uint16_t			_voltRef 			= ENS145_REFERENCE_VOLTAGE;
		uint16_t			_voltHP[2] 			= {0,0};
		uint16_t			_voltDAC[2] 		= {0,0};
		uint16_t			_adcOut[2]			= {0,0};

		uint16_t			_adcChannel[2][2] 	= {{ENS145_ADC_CHANNEL_RH1, ENS145_ADC_CHANNEL_RH3} , {ENS145_ADC_CHANNEL_RS1, ENS145_ADC_CHANNEL_RS3}};
		uint16_t			_adcRes 			= ENS145_ADC_RESOLUTION;
		float				_adcCorrection 		= 1.1;
		uint16_t			_adcIn[2][2] 		= {{0,0},{0,0}};
		uint16_t			_voltIn[2][2] 		= {{0,0},{0,0}};
		uint32_t			_resIn[2][2] 		= {{0,0},{0,0}};
		
		uint16_t			_enableRS[2] 		= {ENS145_ENABLE_PIN_RS1, ENS145_ENABLE_PIN_RS3};
		uint32_t			_shuntRS[2] 		= {ENS145_SHUNT_RS1, ENS145_SHUNT_RS3};
		uint32_t			_shuntRH[2] 		= {ENS145_SHUNT_RH1, ENS145_SHUNT_RH3};
		
		bool				_debugENS145 = false;
		bool				_available = false;
};


#endif
