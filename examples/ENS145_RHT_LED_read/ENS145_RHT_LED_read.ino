/*
  Example code for the ENS145 Development Shield

  This example will read the ENS145 as well as the ENS210 and it will drive the RGBW LED
  
  The RGBW LED will be used to indicate the startup procedure
    red           start ENS145
    red blink     error ENS145
    green         start ENS145 successful
    purple        start ENS210
    purple blink  error ENS210
    blue          start ENS210 successful
  
  Afterwards, the color of the RGBW LED will be adjusted to the humidity level
    red           80% relative humidity
    blue          20% relative humidity
  
  2021 Jan 29   v0.1  Christoph Friese
*/

// Library to drive the ENS145 sensor
#include <ScioSense_ENS145.h>
ScioSense_ENS145 ens145;

// Library to drive the EN210 temperture & humidity sensor
#include <ScioSense_ENS210.h>
ScioSense_ENS210 ens210;

// Library to drive the onoard RGBW LED
#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel pixels(1, NEO_PIXEL_DATA_INPUT_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
 
  Serial.begin(9600);
  
  while(!Serial) { delay(100); };
  
  Serial.println("ENS145 Development Shield demo");
 
  pixels.begin(); // INITIALIZE NeoPixel strip object 
  pixels.clear(); // Set pixel colors to 'off'

  Serial.print("ENS145...");
  ens145.begin(ENS145_ADC_RESOLUTION, false);       // debug mode enabled

  //Set LED to red before start of sensor initialization 
  pixels.setPixelColor(0, pixels.Color(150, 0, 0));
  pixels.show();   
  delay(1000);
    
  // If initialization is not successful, then blink LED red else put it to orange
  if (!ens145.available()) {
    Serial.println("failed!");
    while (1) {
      pixels.clear();
      pixels.show();
      delay(500);   
      pixels.setPixelColor(0, pixels.Color(150, 0, 0));
      pixels.show();   
    }
  } 
  Serial.println("done!");
  pixels.setPixelColor(0, pixels.Color(250, 150, 50));
  pixels.show();   
  delay(1000);
  
  // Set voltage of heaters for operation 
  uint16_t voltageP1 = 800;
  if (!ens145.setHeaterVoltage(ENS145_HP1, voltageP1)) {
    Serial.println("Issue setting voltage for HP1");
  }
  uint16_t voltageP3 = 1100;
  if (!ens145.setHeaterVoltage(ENS145_HP3, voltageP3)) {
    Serial.println("Issue setting voltage for HP3");
  }

  //Set pixel to green after successful setup of sensor
  pixels.setPixelColor(0, pixels.Color(0, 250, 0));
  pixels.show();   
  delay(1000);

  Serial.print("ENS210...");
  //Set LED to purple before start of ENS210 initialization 
  pixels.setPixelColor(0, pixels.Color(200, 50, 250));
  pixels.show();   
  delay(1000);
   
  ens210.begin();
  // If initialization is not successful, then blink LED purple else put it to blue
  if (!ens210.available()) {
    Serial.println("failed!");
    while (1) {
      pixels.clear();
      pixels.show();
      delay(500);   
      pixels.setPixelColor(0, pixels.Color(200, 50, 250));
      pixels.show();   
    }
  } 
  ens210.setSingleMode(false);
  Serial.println("done!");
  pixels.setPixelColor(0, pixels.Color(0, 0, 250));
  pixels.show();
  delay(1000);
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
    Serial.print(" Ohm\t");
  }
  
  if (ens210.available()) {
    ens210.measure();
    Serial.print("ENS210:");
    Serial.print("\t");
    Serial.print(ens210.getTempCelsius());
    Serial.print("°C\t");
    float rh = ens210.getHumidityPercent();
    Serial.print(rh);
    Serial.print("%\t");

    //Pixel color will change from blue (0% relative humidity) to red (100% relative humidity)
    pixels.setPixelColor(0, pixels.Color(map(rh, 20, 80, 0, 255), 0, 255 - map(rh, 20, 80, 0, 255)));
    pixels.show();
  }
  
  Serial.println();
   
  delay(1000); 
}