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
int motorPorts[] = {1, 2};

// sensor constructors
sensors plate(sensorPin[0], sensorLed[0]);
sensors spout0(sensorPin[1], sensorLed[1], motorPorts[0]);
sensors spout1(sensorPin[2], sensorLed[2], motorPorts[1]);

// sensor flags constructors
flags plateFlag(false, false, 5, 1);
flags spout0Flag(false, false, 5, 1);
flags spout1Flag(false, false, 5, 1);

// for serial coms
int experiment;
int fr;
int spout_0;
int spout_1;
int plate_0;
int scheduleSpout_0;
int scheduleSpout_1;
int timePlate_0;
int timeOut;
int newSetup;
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
//  spout1.begin();
  plateFlag.setHeldValid(false); // at the start the held time should not be valid

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
  // <1511111111>


  // read experimental setup
  recvWithStartEndMarkers(); // read serial port store into setupAr ||||ray
  experiment = receivedChars[0] - '0';
  fr = receivedChars[1] - '0';
  spout_0 = receivedChars[2] - '0';
  spout_1 = receivedChars[3] - '0';
  plate_0 = receivedChars[4] - '0';
  scheduleSpout_0 = receivedChars[5] - '0';
  scheduleSpout_1 = receivedChars[6] - '0';
  timePlate_0 = ((int) receivedChars[7] - '0') * 1000; // time specifies number of seconds
  timeOut = ((int) receivedChars[8] - '0') * 1000;
  newSetup = receivedChars[9] - '0';

  // set sensor on or off PROBABLY NOT USEFUL
  if (spout_0 == 1) spout0.sensorOn(); else spout0.sensorOff();
  if (spout_1 == 1) spout1.sensorOn(); else spout1.sensorOff();
  if (plate_0 == 1) plate.sensorOn(); else plate.sensorOff();
  // set LEDs setup, turn off is deactivated
  if (!spout0.active()) spout0.trialLed.off();
  if (!spout1.active()) spout0.trialLed.off();
  if (!plate.active()) plate.trialLed.off();

  // create ratios, TODO: PR
  if (newSetup == 1) {
    // spout 0
    spout0Flag.setRatio(fr);
    spout0Flag.setParadigm(scheduleSpout_0);
    spout0Flag.createRatio();

    // spout 1
    spout1Flag.setRatio(fr);
    spout1Flag.setParadigm(scheduleSpout_1);
    spout1Flag.createRatio();

    newSetup = 0;
    receivedChars[9] = 0;
  }

  // read sensors
  if (spout_0 == 1) spout0.sense(), spout0Flag.lickometerOn(); else spout0Flag.lickometerOff();
  if (spout_1 == 1) spout1.sense(), spout1Flag.lickometerOn(); else spout0Flag.lickometerOff();
  if (plate_0 == 1) plate.sense(), plateFlag.plateOn(); else plateFlag.plateOff();
  // read if animal has spent enough time on top of the plate, set this time to 0
  // if plate is not going to be used
  if (plate.held() > timePlate_0) plateFlag.setHeldValid(true); else plateFlag.setHeldValid(false);

  // get sensor reads
  if (spout0Flag.lickometerActive()) outReads[0] = spout0.status(); else outReads[0] = 3; // 3 means not a valid lecture because its off
  if (spout1Flag.lickometerActive()) outReads[1] = spout1.status(); else outReads[1] = 3;
  if (plateFlag.lickometerActive()) outReads[2] = plate.status(); else outReads[2] = 3;

  // spout 0 control
  if (spout0Flag.timeOut(timeOut)) {
    spout0.trialLed.on();
    if (spout0Flag.paradigm() == 0) { // 0 = fixed ratio
      spout0Flag.isEvent(spout0.validStatusSum());
    }
    else if (spout0Flag.paradigm() == 1) { // 1 = random ratio
      if (spout0Flag.isEvent(spout0.validStatusSumReset()) == 1) {
        spout0.resetSum();
        spout0Flag.createRatio();
      }
    }
  }
  else {
    spout0.trialLed.off();
  }

//  if (spout0Flag.timeOut(timeOut) && spout0Flag.eventChanged()){
//    spout0.deliverLiquid();
//    spout0Flag.setEventChanged();
//  }

  // spout 1 control
  if (spout1Flag.timeOut(timeOut)) {
    spout1.trialLed.on();
    if (spout1Flag.paradigm() == 0) { // 0 = fixed ratio
      spout0Flag.isEvent(spout1.validStatusSum());
    }
    else if (spout1Flag.paradigm() == 1) { // 1 = random ratio
      if (spout0Flag.isEvent(spout1.validStatusSumReset()) == 1) {
        spout1.resetSum();
        spout1Flag.createRatio();
      }
    }
  }
  else {
    spout1.trialLed.off();
  }

  // check for valid trials
  Serial.println((String) "ratio: " + spout0Flag.ratio() + "Events: " + spout0Flag.totalEvents() + "time out is: " + timeOut);

  // time out

  // write data


}
