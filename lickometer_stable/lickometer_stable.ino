#include <Wire.h>
#include "Adafruit_MPR121.h"

#ifndef _BV
#define _BV(bit) (1 << (bit))
#endif

bool sensorStateChange = false;
bool trialFlag = false;
bool lastSensorState = false;
bool currentSensorState = false;
bool validSensorTouch = false;
int startSensorTouch = 0;
int endSensorTouch = 0;
int heldSensorTouch = 0;
bool frCurrentTouch = false;
bool frLastTouch = false;
int fr = 5;


//void toggleTrialFlag() {
//  if (trialFlag == false) {
//    trialFlag = true;
//  }
//  else {
//    trialFlag = false;
//  }
//}

void sensorTimer() {
  if (currentSensorState == 1) {
    startSensorTouch = millis();
  }
  else {
    endSensorTouch = millis();
    heldSensorTouch = endSensorTouch - startSensorTouch;
  }
}

void checkSensorChange(){
  if (currentSensorState != lastSensorState){
    sensorStateChange = true;
  }
  else{
    sensorStateChange = false;
  }
}

// You can have up to 4 on one i2c bus but one is enough for testing!
Adafruit_MPR121 cap = Adafruit_MPR121();

void setup() {
  Serial.begin(9600);

  while (!Serial) { // needed to keep leonardo/micro from starting too fast!
    delay(10);
  }

  Serial.println("Adafruit MPR121 Capacitive Touch sensor test");

  // Default address is 0x5A, if tied to 3.3V its 0x5B
  // If tied to SDA its 0x5C and if SCL then 0x5D
  if (!cap.begin(0x5A)) {
    Serial.println("MPR121 not found, check wiring?");
    while (1);
  }
  Serial.println("MPR121 found!");
}

void loop() {
  // gets pin 1
  currentSensorState = cap.touched() & (1 << 1);
  frCurrentTouch = cap.touched() & (1 << 0);
  checkSensorChange();
  if (sensorStateChange == true){
    sensorTimer();
  }
  if (heldSensorTouch >= 1000){
    validSensorTouch = true;
    trialFlag = true;
    heldSensorTouch = 0;
  }
  else{
    validSensorTouch = false;
    heldSensorTouch = 0;
  }

  if (cap.touched() & (1 << 0)){
    if (frCurrentTouch != frLastTouch){
      fr++;
    }
    if (fr > 5){
      trialFlag = false;
      fr = 0;
    }
  }
  Serial.print("Should lickometer be active?: ");
  Serial.println(trialFlag);
  lastSensorState = currentSensorState;
  frLastTouch = frCurrentTouch;


}
