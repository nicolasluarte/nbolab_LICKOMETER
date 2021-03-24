#include "leds.h"
#include "sensorPlate.h"
#include "flagPass.h"
#include <Wire.h>
#include "Adafruit_MPR121.h" //  MPR121 breakout board library
#include <Adafruit_MotorShield.h> // Library for Adafruit MotorShield v2
//

sensorPlate S1(0, 6);
sensorPlate L1(1, 5);
flagPass flag(true, true);


void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  S1.begin();
  L1.begin();
}

void loop() {

    S1.sense();
    L1.sense();
    Serial.write(L1.status());
    Serial.write(S1.status());



}
