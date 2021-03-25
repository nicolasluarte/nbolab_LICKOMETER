#include "leds.h"
#include "sensors.h"
#include "flags.h"
#include <Wire.h>
#include "Adafruit_MPR121.h" //  MPR121 breakout board library
#include <Adafruit_MotorShield.h> // Library for Adafruit MotorShield v2
//

sensors S1(0, 6);
sensors L1(1, 5);
flags flag(true, true, 5);


void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  S1.begin();
  L1.begin();
}

void loop() {

    S1.sense();
    L1.sense();
    flag.isEvent(S1.statusSum());
    Serial.println(flag.totalEvents());
}
