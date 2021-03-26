#include "leds.h"
#include "sensors.h"
#include "flags.h"
#include <Wire.h>
#include "Adafruit_MPR121.h" //  MPR121 breakout board library
#include <Adafruit_MotorShield.h> // Library for Adafruit MotorShield v2
//

sensors S1(0, 6);
sensors L1(1, 5);
flags flag(true, true, 5, "FR");


void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  S1.begin();
  L1.begin();
  flag.createRatio();
}

void loop() {
  //
  S1.sense();
  S1.sensorOn();
  L1.sense();
  Serial.print(S1.statusSum());
  Serial.print(" ");
  Serial.print(flag.totalEvents());
  Serial.print(" ");
  Serial.println(flag.ratio());
  if (flag.paradigm() == "RR") {
    if (flag.isEvent(S1.validStatusSumReset()) == 1) {
      S1.resetSum();
      flag.createRatio();
    }
  }
  else if (flag.paradigm() == "FR"){
    flag.isEvent(S1.validStatusSum());
  }
}
