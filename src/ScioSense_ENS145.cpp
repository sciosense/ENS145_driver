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
	this->setHeaterVoltage(ENS145_HP1, 0);
	this->setHeaterVoltage(ENS145_HP3, 0);
	if (this->_debugENS145) Serial.println("ENS145debug: Heater initially set to 0");

	return true;
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
bool ScioSense_ENS145::setHeaterVoltage(uint8_t hotplate, uint16_t mVolts) {
	if (!(mVolts > 3000)) {
		this->_voltHP[hotplate] = mVolts; 
		uint16_t dacValue = this->mVoltsToDACvalue(this->_voltHP[hotplate]);  			// Convert to DAC value
		return this->setDACvalue(hotplate, dacValue);            // Set DAC value of output channel 
	} else {
		if (this->_debugENS145) {
			Serial.print("Voltage setting higher than DAC saturation: ");
			Serial.println(mVolts);
		}
		return false;
	}	
}

//Defines a certain target resistance for heater and adjust heater voltage accordingly
//bool ScioSense_ENS145::setHeaterResistance(uint8_t hotplate, uint16_t targetRes) {
	
	//Set initial non-harmful voltage, determine resistance, calculate current, derive target voltage
//	if (this->_voltHP[hotplate] == 0) this->_voltHP[hotplate] = 500;
	
	//
//	uint16_t targetVolt = targetRes * (this->_voltHP[hotplate] / this->measureHeaterResistance(hotplate));
	
	//Calculate target voltage
//	Serial.print("New HP voltage ");
	//Serial.println(targetVolt);
	
	//return true;
//}

//-----------------------------------------------------------------------------------

// Measure heater voltage in [mV]
uint16_t ScioSense_ENS145::measureHeaterVoltage(uint8_t hotplate) {
	uint16_t reading;
	
	for (int i=0; i<10; i++) {
		delay(1);
		reading = analogRead(this->_adcChannel[0][hotplate]);
	}

	if (this->_debugENS145) {
		Serial.print("Analog read: ");
		Serial.println(reading);
	}
	
	return (uint16_t)(((uint32_t)reading * (uint32_t)this->_voltRef) / (uint32_t)this->_adcRes);
}

// Measure heater resistance in [Ohm]
float ScioSense_ENS145::measureHeaterResistance(uint8_t hotplate){
	
	uint16_t voltage = this->measureHeaterVoltage(hotplate);
	
	return ((float)voltage / ((float)(this->_voltHP[hotplate] - voltage) / (float)this->_shuntRH[hotplate]));
}

//-----------------------------------------------------------------------------------

// Measure sensor voltage in [mV]
uint16_t ScioSense_ENS145::measureSensorVoltage(uint8_t hotplate){
	uint16_t reading;

	digitalWrite(this->_enableRS[hotplate], LOW);
	for (int i=0; i<10; i++) {
		delay(1);
		reading = analogRead(this->_adcChannel[1][hotplate]);
	}
	digitalWrite(this->_enableRS[hotplate], HIGH);
		
	if (this->_debugENS145) {
		Serial.print("Analog read: ");
		Serial.println(reading);
	}
	
	//return (reading * this->_voltRef / this->_adcRes);
	//return (reading * 3.222); // in millivolts
	return (uint16_t)(((uint32_t)reading * (uint32_t)this->_voltRef) / (uint32_t)this->_adcRes);
}

// Measure sensor resistance in [Ohm]
uint32_t ScioSense_ENS145::measureSensorResistance(uint8_t hotplate){

	uint16_t voltage = this->measureSensorVoltage(hotplate);

	return (uint32_t)((float)voltage / ((float)(this->_voltRef - voltage) / (float)this->_shuntRS[hotplate]));
}



