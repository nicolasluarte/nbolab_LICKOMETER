#include "leds.h"
#include "sensors.h"
#include "flags.h"
#include "serialFunctions.h"
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

// sensor flags constructors
flags plateFlag(false, false, 5, 1);
flags spout0Flag(false, false, 5, 1);
flags spout1Flag(false, false, 5, 1);


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

// for serial coms
int experiment;
int fr;
int spout_0;
int spout_1;
int plate_0;
int scheduleSpout_0;
int scheduleSpout_1;
int timePlate_0;
int outReads[32];


void setup() {
  // set serial communications
  Serial.begin(9600);

  // needed to keep leonardo/micro from starting too fast!
  while (!Serial) {
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

  // read experimental setup
  recvWithStartEndMarkers(); // read serial port store into setupArray
  experiment = receivedChars[0] - '0';
  fr = receivedChars[1] - '0';
  spout_0 = receivedChars[2] - '0';
  spout_1 = receivedChars[3] - '0';
  plate_0 = receivedChars[4] - '0';
  scheduleSpout_0 = receivedChars[5] - '0';
  scheduleSpout_1 = receivedChars[6] - '0';
  timePlate_0 = receivedChars[7] - '0';

  // set sensor on or off
  if (spout_0 == 1) spout0.sensorOn(); else spout0.sensorOff();
  if (spout_1 == 1) spout1.sensorOn(); else spout1.sensorOff();
  if (plate_0 == 1) plate.sensorOn(); else plate.sensorOff();

  // create ratios, TODO: PR
  // spout 0
  spout0Flag.setRatio(fr);
  spout0Flag.setParadigm(scheduleSpout_0);
  spout0Flag.createRatio();

  // spout 1
  spout1Flag.setRatio(fr);
  spout1Flag.setParadigm(scheduleSpout_1);
  spout1Flag.createRatio();

  // read sensors
  if (spout_0 == 1) spout0.sense(), spout0Flag.lickometerOn(); else spout0Flag.lickometerOff();
  if (spout_1 == 1) spout1.sense(), spout1Flag.lickometerOn(); else spout0Flag.lickometerOff();
  if (plate_0 == 1) plate.sense(), plateFlag.plateOn(); else plateFlag.plateOff();
  // read if animal has spent enough time on top of the plate, set this time to 0
  // if plate is not going to be used
  if (plate.held() >= timePlate_0) plateFlag.setHeldValid(true); else plateFlag.setHeldValid(false);

  // get sensor reads
  if (spout0Flag.lickometerActive()) outReads[0] = spout0.status(); else outReads[0] = 3; // 3 means not a valid lecture because its off
  if (spout1Flag.lickometerActive()) outReads[1] = spout1.status(); else outReads[1] = 3;
  if (plateFlag.lickometerActive()) outReads[2] = plate.status(); else outReads[2] = 3;
  
  // check for valid trials

  // time out

  // write data


}
