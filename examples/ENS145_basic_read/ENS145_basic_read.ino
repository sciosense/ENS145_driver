#include <Wire.h>
#include <ScioSense_ENS145.h>

ScioSense_ENS145 ens145;

/************************/
/*    TEST VARIABLES    */
uint16_t  voltageHP1, voltageHP3;
uint16_t  dacValue;
bool      rampUP = true;
/************************/


void setup() {
  Serial.begin(9600);
  ens145.begin(true);       // debug mode enabled

  

}

void loop() {

  for(voltageHP1=0;voltageHP1<=3000;voltageHP1 +=100) {
    dacValue = ens145.mVoltsToDACvalue(voltageHP1);
    ens145.setDACvalue(ENS145_HP1,dacValue,false);
    delay(100);
  }
  
}
