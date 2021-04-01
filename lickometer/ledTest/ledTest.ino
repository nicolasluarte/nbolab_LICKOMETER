#include "leds.h"
#include "sensors.h"
#include "flags.h"
#include <Wire.h>
#include "Adafruit_MPR121.h" //  MPR121 breakout board library
#include <Adafruit_MotorShield.h> // Library for Adafruit MotorShield v2

// variables to control sensor pin and led associated with sensor
int sensorPin[] = {0, 1, 2};
int sensorLed[] = {3, 5, 6};

// sensor constructors
sensors plate(sensorPin[0], sensorLed[0]);
sensors spout0(sensorPin[1], sensorLed[1]);
sensors spout1(sensorPin[2], sensorLed[2]);


// set experiment type
String XR;

// set motors
Adafruit_StepperMotor* MotorArray[3];
double powerMotor = 0.6; // seemingly not used
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_StepperMotor *Motor1 = AFMS.getStepper(200, 1);
Adafruit_StepperMotor *Motor2 = AFMS.getStepper(200, 2);

// set timeout
int timeout;


void setup() {
  // set serial communications
  Serial.begin(9600);

  // needed to keep leonardo/micro from starting too fast!
  while(!Serial){
    delay(10);
  }
  
  // set sensors
  plate.begin();
  spout0.begin();
  spout1.begin();

  // set motors
  AFMS.begin();
  MotorArray[0] = Motor1;
  MotorArray[2] = Motor2;
  MotorArray[0]->setSpeed(50);
  MotorArray[2]->setSpeed(50);
}

void loop() {
  //
//  S1.sense();
//  S1.sensorOn();
//  L1.sense();
//  Serial.print(S1.statusSum());
//  Serial.print(" ");
//  Serial.print(flag.totalEvents());
//  Serial.print(" ");
//  Serial.println(flag.ratio());
//  if (flag.paradigm() == "RR") {
//    if (flag.isEvent(S1.validStatusSumReset()) == 1) {
//      S1.resetSum();
//      flag.createRatio();
//    }
//  }
//  else if (flag.paradigm() == "FR"){
//    flag.isEvent(S1.validStatusSum());
//  }
  spout0.trialLed.blink();
  
  // set to run experiment or testing
  
  // set lick freq
  
  // read sensors
  
  // check for valid trials
  
  // time out

  // write data
  
  
}
