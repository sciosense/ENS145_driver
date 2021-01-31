/*
  ScioSense_ENS145.h - Library for the ENS145 Development Shield

  2021 Jan 24   v0.3  Christoph Friese
  2021 Jan 21   v0.2  Patryk Hallek
  2020 Dec 08   v0.1  Patryk Hallek   Created

*/

#include "ScioSense_ENS145.h"
#include "math.h"

ScioSense_ENS145::ScioSense_ENS145() {
	
}

// Initializes ENS145, set pin suitable modes, start I2C communciation with DACs
bool ScioSense_ENS145::begin(uint16_t adc, bool debug) {
	this->_adcRes = adc; 
	this->_debugENS145 = debug;

	//Set pins 
	if (this->_debugENS145) Serial.println("ENS145debug: set pin modes");
	
	
	pinMode(ENS145_ENABLE_PIN_RS1, OUTPUT);
	digitalWrite(ENS145_ENABLE_PIN_RS1, HIGH);			//Set to high to deactivate channel

	pinMode(ENS145_ENABLE_PIN_RS3, OUTPUT);
	digitalWrite(ENS145_ENABLE_PIN_RS3, HIGH);			//Set to high to deactivate channel

	//Init I2C for both DACs
	Wire.begin();
	if (this->_debugENS145) Serial.println("ENS145debug: I2C init done");

	//Read reference voltage 
	analogReference(ENS145_AREF);
	
	//Set heater to 0 for initialisation
	this->_available = this->setHeaterVoltage(ENS145_HP1, 0);
	if (this->_available) {
		this->_available = this->setHeaterVoltage(ENS145_HP3, 0);
	}
	if (this->_debugENS145) Serial.println(this->_available ? "ENS145debug: Heater initially set to 0" : "ENSdebug: Setup failed");

	return this->_available;
}

//-----------------------------------------------------------------------------------

// Convert DAC set point in [mV] to a suitable DAC value
uint16_t ScioSense_ENS145::mVoltsToDACvalue(uint16_t mVolts) {
	return (uint16_t)((uint32_t)mVolts * (uint32_t)ENS145_DAC_RESOLUTION / (uint32_t)ENS145_DAC_REF_VOLT);
}

// Write a suitable value to DAC.
bool ScioSense_ENS145::setDACvalue(uint8_t hotplate, uint16_t dacValue) {
	uint8_t result;

	Wire.beginTransmission(this->_addressDAC[hotplate]);
	Wire.write(64);
	Wire.write(dacValue >> 4);
	Wire.write((dacValue & 15) << 4);
	result = Wire.endTransmission();
	return result == 0;
}

// Convert mVolt input to HEX and write to DAC for specific hotplate.
bool ScioSense_ENS145::setHeaterVoltage(uint8_t hotplate, uint16_t targetVolts) {
	bool result;
	
	if (targetVolts > 1200) return false;											// Skip process if voltage would exceed maximum range of DAC
	
	this->_voltHP[hotplate] = targetVolts; 
	

	if (this->_voltHP[hotplate] > 0) {
		// Ramp-up output DAC until measured heater voltage corresponds to target voltage
		this->_voltDAC[hotplate] = 300; 											// define initial (non-critical) voltage, expected heater voltage 100mV

		for (uint8_t adjustTrials = 0; adjustTrials<9; adjustTrials++) {
			uint16_t dacValue = this->mVoltsToDACvalue(this->_voltDAC[hotplate]);  	// Convert to DAC value
			result = this->setDACvalue(hotplate, dacValue);							// Set DAC value of output channel 		
			this->measureHeater(hotplate);											// Measure heater voltage through analog pin

			if (this->_debugENS145) {
				Serial.print("Volt DAC: ");
				Serial.print(this->_voltDAC[hotplate]);
				Serial.print("\tDAC value: ");
				Serial.print(dacValue);
				Serial.print("\tResult: ");
				Serial.print(result);
				Serial.print("\tVolt ADC: ");
				Serial.print(this->_voltIn[0][hotplate]);
			}
		
			//Adjust DAC output voltage according to previous steps to meet target heater voltage
			this->_voltDAC[hotplate] = (uint16_t)(((uint32_t)targetVolts * (uint32_t)this->_voltDAC[hotplate]) / (uint32_t)this->_voltIn[0][hotplate]);

			if (this->_debugENS145) {
				Serial.print("\tnew Volt DAC: ");
				Serial.println(this->_voltDAC[hotplate]);
			}
				
			if (this->_voltDAC[hotplate] > 3300) return false;						// Skip process if voltage would exceed maximum range of DAC
		}
	} else {
		result = this->setDACvalue(hotplate, 0);									// Switch off DAC
		if (this->_debugENS145) {
			Serial.print("Switch off DAC: ");
			Serial.println(result);
		}
	}		
	
	return result;
}

//-----------------------------------------------------------------------------------

bool ScioSense_ENS145::measure() {
	bool result = this->measureHeater(ENS145_HP1);
	result = this->measureSensor(ENS145_HP1);
	result = this->measureHeater(ENS145_HP3);
	result = this->measureSensor(ENS145_HP3);
	
	return result;
}

// Measure heater parameters
bool ScioSense_ENS145::measureHeater(uint8_t hotplate) {

	for (int i=0; i<10; i++) {
		delay(1);
		this->_adcIn[0][hotplate] = (uint16_t)((float)analogRead(this->_adcChannel[0][hotplate]) * this->_adcCorrection);
	}

	this->_voltIn[0][hotplate] = (uint16_t)(((uint32_t)this->_adcIn[0][hotplate] * (uint32_t)this->_voltRef) / (uint32_t)this->_adcRes);

	this->_resIn[0][hotplate]  = (uint32_t)(((uint32_t)this->_voltIn[0][hotplate] * (uint32_t)this->_shuntRH[hotplate]) / ((uint32_t)this->_voltDAC[hotplate] - (uint32_t)this->_voltIn[0][hotplate]));

	if (this->_debugENS145) {
		Serial.print("ADC in: ");
		Serial.print(this->_adcIn[0][hotplate]);
		Serial.print("\tVolt in: ");
		Serial.print(this->_voltIn[0][hotplate]);
		Serial.print("\tR heater: ");
		Serial.println(this->_resIn[0][hotplate]);
	}
	
	return true;
}

//-----------------------------------------------------------------------------------

// Measure sensor parameters
bool ScioSense_ENS145::measureSensor(uint8_t hotplate) {

	digitalWrite(this->_enableRS[hotplate], LOW);
	for (int i=0; i<10; i++) {
		delay(1);
		this->_adcIn[1][hotplate] = (uint16_t)((float)analogRead(this->_adcChannel[1][hotplate]) * this->_adcCorrection);
	}
	digitalWrite(this->_enableRS[hotplate], HIGH);

	this->_voltIn[1][hotplate] = (uint16_t)(((uint32_t)this->_adcIn[1][hotplate] * (uint32_t)this->_voltRef) / (uint32_t)this->_adcRes);

	this->_resIn[1][hotplate]  = (uint32_t)(((uint32_t)this->_voltIn[1][hotplate] * (uint32_t)this->_shuntRS[hotplate]) / ((uint32_t)this->_voltRef - (uint32_t)this->_voltIn[1][hotplate]) );
	
	if (this->_debugENS145) {
		Serial.print("Analog read sensor: ");
		Serial.print(this->_adcIn[1][hotplate]);
		Serial.print("\t");
		Serial.print(this->_voltIn[1][hotplate]);
		Serial.print("\t");
		Serial.println(this->_resIn[1][hotplate]);
	}
	
	return true;
}



