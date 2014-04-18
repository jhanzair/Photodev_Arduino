/*
Photodev firmware, v0.7
Joao Sousa, 2012-2013
*/

#include <Wire.h>
#include "OneWire.h"

#define UPDATE_PERIOD 500;  //number of milliseconds between updates

byte tempProbePin = 8;
OneWire ds(tempProbePin);
float temp1;
float temp2;
long time = 0;
byte tempData[4];
volatile unsigned long footNextClick;
volatile byte footButton = 0;

void setup() {
  Serial.begin(9600);
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(1, footButtonClick, FALLING);  //Pin 2
}

void loop() {
  if (millis() > time) {
    Serial.write('t');
    Serial.write(tempData[0]);
    Serial.write(tempData[1]);
    Serial.write(tempData[2]);
    Serial.write(tempData[3]);
    Serial.write('f');
    Serial.write(footButton);
    Serial.write('e');
    
    footButton = 0;
    time = millis() + UPDATE_PERIOD;
    updateTempProbe();
  }
}

//Read the two DS18B20 temperature sensors
//This code is based on the OneWire example in the Arduino website
void updateTempProbe() {
  byte data[12];
  byte addr[8];

  //Read first Sensor
  ds.search(addr);
  
  ds.reset();
  ds.select(addr);
  ds.write(0x44,1);
  
  byte present = ds.reset();
  ds.select(addr);
  ds.write(0xBE);
  
  for (int i = 0; i < 9; i++) {
  data[i] = ds.read();
  }
  
  byte MSB = data[1];
  byte LSB = data[0];
  
  tempData[0] = LSB;
  tempData[1] = MSB;
  
  float tempRead = ((MSB << 8) | LSB);
  temp1 = tempRead / 16;
  
  
  //Read second Sensor
  ds.search(addr);
  
  ds.reset();
  ds.select(addr);
  ds.write(0x44,1);
  
  present = ds.reset();
  ds.select(addr);
  ds.write(0xBE);
  
  for (int i = 0; i < 9; i++) {
  data[i] = ds.read();
  }
  
  MSB = data[1];
  LSB = data[0];
  
  tempData[2] = LSB;
  tempData[3] = MSB;
  
  tempRead = ((MSB << 8) | LSB);
  temp2 = tempRead / 16;
  
  ds.reset_search();
}

//Button click interrupt function
void footButtonClick() {
  //Debounce the button, by setting a time frame (500 ms) where input is ignored
  if (millis() > footNextClick) {
    footNextClick = millis() + 500;
    footButton = 1;
  }
}
