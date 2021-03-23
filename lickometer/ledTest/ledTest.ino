#include "leds.h"
#include "sensorPlate.h"
#include <Wire.h>
#include "Adafruit_MPR121.h" //  MPR121 breakout board library
#include <Adafruit_MotorShield.h> // Library for Adafruit MotorShield v2
//
//digitalPin LED1(6);
sensorPlate S1(0, 6);


void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  //  LED1.begin();
  S1.begin();
  S1.trialLed.on();

}

void loop() {
  // put your main code here, to run repeatedly:
  S1.sense();
  if (S1.held() == 1000) {
    Serial.println("held for 1 sec");
    S1.trialLed.off();
  }
}
