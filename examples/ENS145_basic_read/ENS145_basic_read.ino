/*
  Example code for the ENS145 Development Shield
  
  This example will read only the ENS145
  
  2021 Jan 24   v0.3  Christoph Friese
  2021 Jan 21   v0.2  Patryk Hallek
  2020 Dec 08   v0.1  Patryk Hallek   Created

*/

#include <ScioSense_ENS145.h>
ScioSense_ENS145 ens145;

void setup() {
  Serial.begin(9600);
  
  while(!Serial);
  
  Serial.println("ENS145 Development Shield demo");
  
  Serial.print("ENS145...");
  ens145.begin(ENS145_ADC_RESOLUTION, false);       // debug mode enabled
  // If ENS145 initialization is not successful, stop execution
  if (!ens145.available()) {
    Serial.println("failed!");
    while (1) {
      delay(500);   
    }
  } 
  Serial.println("done!");

  // Set voltage of heaters for operation 
  uint16_t voltageP1 = 800;
  if (!ens145.setHeaterVoltage(ENS145_HP1, voltageP1)) {
    Serial.println("Issue setting voltage for HP1");
  }
  uint16_t voltageP3 = 1100;
  if (!ens145.setHeaterVoltage(ENS145_HP3, voltageP3)) {
    Serial.println("Issue setting voltage for HP3");
  }
  
}

void loop() {

  if (ens145.available()) {
    Serial.print("ENS145 HP1:");
    Serial.print("\t");
    Serial.print(ens145.measureHeaterVoltage(ENS145_HP1));
    Serial.print(" mV\t");
    Serial.print(ens145.measureHeaterResistance(ENS145_HP1));     //expected below 100Ohm
    Serial.print(" Ohm\t");
    Serial.print(ens145.measureSensorVoltage(ENS145_HP1));
    Serial.print(" mV\t");
    Serial.print(ens145.measureSensorResistance(ENS145_HP1));     //expected up to 1MOhm
    Serial.print(" Ohm\t");

    Serial.print("ENS145 HP3:");
    Serial.print("\t");
    Serial.print(ens145.measureHeaterVoltage(ENS145_HP3));
    Serial.print(" mV\t");
    Serial.print(ens145.measureHeaterResistance(ENS145_HP3));     //expected below 100Ohm
    Serial.print(" Ohm\t");
    Serial.print(ens145.measureSensorVoltage(ENS145_HP3));        //expected up to 1MOhm
    Serial.print(" mV\t");
    Serial.print(ens145.measureSensorResistance(ENS145_HP3));
    Serial.println(" Ohm");
  }
  
  delay(1000); 
}