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
bool ScioSense_ENS145::begin(bool debug) {
 
	debugENS145 = debug;
	
	//Set pins 
	if (debugENS145) Serial.println("ENS145debug: set pin modes");
	pinMode(ENS145_ENABLE_PIN_RS1, OUTPUT);
	digitalWrite(ENS145_ENABLE_PIN_RS1, LOW);

	pinMode(ENS145_ENABLE_PIN_RS3, OUTPUT);
	digitalWrite(ENS145_ENABLE_PIN_RS3, LOW);

	//Init I2C for both DACs
	Wire.begin();
	if (debugENS145) Serial.println("ENS145debug: I2C init done");

	//Set heater to 0 for initialiasation
	this->setHeaterVoltage(ENS145_HP1, 0);
	this->setHeaterVoltage(ENS145_HP3, 0);
	if (debugENS145) Serial.println("ENS145debug: Heater set to 0");

	return true;
}

// Convert DAC set point in [mV] to a suitable DAC value
uint16_t ScioSense_ENS145::mVoltsToDACvalue(uint16_t voltage) {
	return (uint16_t)(voltage * ENS145_DAC_RESOLUTION / ENS145_REFERENCE_VOLTAGE);
}

// Write a suitbale value to DAC.
bool ScioSense_ENS145::setDACvalue(uint8_t hotplate, uint16_t dacValue) {
	uint8_t result;

	switch(hotplate){
		case 1:		Wire.beginTransmission(ENS145_I2CADDR_HP1_DAC);
					break;
		case 3:		Wire.beginTransmission(ENS145_I2CADDR_HP3_DAC);
					break;
		default:	return false;
					break;
	}
	Wire.write(64);
	Wire.write(dacValue >> 4);
	Wire.write((dacValue & 15) << 4);
	result = Wire.endTransmission();
	return result == 0;
}

bool ScioSense_ENS145::setHeaterVoltage(uint8_t hotplate, uint16_t mVolts) {
	if (!(mVolts > ENS145_REFERENCE_VOLTAGE)) {
		this->_voltHP[hotplate] = mVolts; 
		uint16_t dacValue = this->mVoltsToDACvalue(mVolts);  			// Convert to DAC value
		return this->setDACvalue(hotplate, dacValue);            // Set DAC value of output channel 
	} else {
		if (debugENS145) {
			Serial.print("Voltage setting high than reference voltage: ");
			Serial.println(mVolts);
		}
		return false;
	}	
}

// Measure sensor voltage in [mV]
uint16_t ScioSense_ENS145::measureHeaterVoltage(uint8_t hotplate){

	uint16_t reading;
	
	switch(hotplate){
		case 1:		reading = analogRead(ENS145_ADC_CHANNEL_RH1);
					break;
		case 3:		reading = analogRead(ENS145_ADC_CHANNEL_RH3);
					break;
		default:	return -1;
					break;
	}
		
	if (debugENS145) {
		Serial.print("Analog read: ");
		Serial.println(reading);
	}
	return (reading * ENS145_REFERENCE_VOLTAGE / ENS145_ADC_RESOLUTION);
}

// Measure sensor resistance in [Ohm]
uint16_t ScioSense_ENS145::measureHeaterResistance(uint8_t hotplate){
	uint16_t shunt;

	uint16_t voltage = this->measureHeaterVoltage(hotplate);

	switch(hotplate){
		case 1:		return (voltage * ENS145_SHUNT_RH1) / (this->_voltHP[hotplate] - voltage);
					break;
		case 3:		return (voltage * ENS145_SHUNT_RH3) / (this->_voltHP[hotplate] - voltage);
					break;
		default:	return -1;
					break;
	}	
}

// Measure sensor voltage in [mV]
uint16_t ScioSense_ENS145::measureSensorVoltage(uint8_t hotplate){
	uint16_t reading;

	switch(hotplate){
		case 1:		digitalWrite(ENS145_ENABLE_PIN_RS1, LOW);
					reading = analogRead(ENS145_ADC_CHANNEL_RS1);
					digitalWrite(ENS145_ENABLE_PIN_RS1, HIGH);
					break;
		case 3:		digitalWrite(ENS145_ENABLE_PIN_RS3, LOW);
					reading = analogRead(ENS145_ADC_CHANNEL_RS3);
					digitalWrite(ENS145_ENABLE_PIN_RS3, HIGH);
					break;
		default:	return -1;
					break;
	}
		
	if (debugENS145) {
		Serial.print("Analog read: ");
		Serial.println(reading);
	}
	return (reading * ENS145_REFERENCE_VOLTAGE / ENS145_ADC_RESOLUTION);
}

// Measure sensor resistance in [Ohm]
uint16_t ScioSense_ENS145::measureSensorResistance(uint8_t hotplate){
	uint16_t shunt;

	uint16_t voltage = this->measureSensorVoltage(hotplate);

	switch(hotplate){
		case 1:		return (voltage * ENS145_SHUNT_RS1) / (ENS145_REFERENCE_VOLTAGE - voltage);
					break;
		case 3:		return (voltage * ENS145_SHUNT_RS3) / (ENS145_REFERENCE_VOLTAGE - voltage);
					break;
		default:	return -1;
					break;
	}	
}



