/*********************************************************
  PROGRAM: PR (PROGRESSIVE RATIO PILOT) for one pump and inactive pumps
  Date: November 24, 2020

  DESCRIPTION: This program sets an progressive ratio on a linear schedule for pilot test in november 2020

  # VARIABLES
  # Time out for drinking tube one is 20 seconds.

  Written by Claudio Perez-Leighton.
**********************************************************/
#include "a_header.h"

/************************************************************************
*************************************************************************/
// SETUP
void setup() {
  // Initialize serial @ 9600 bits per second
  Serial.begin(9600);

  // needed to keep leonardo/micro from starting too fast!
  while (!Serial) {
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

  // Adjust threshold for MPR121
  cap.writeRegister(MPR121_ECR, 0x00); // puts MPR121 into stop mode, so threshold can be modified
  cap.setThresholds(0x14, 0x07); // This is the threshold that was found not to cause any false positives
  cap.writeRegister(MPR121_ECR, 0x8F); // start MPR121 run mode again

  // Populate motor array
  AFMS.begin();  // create with the default frequency 1.6KHz
  MotorArray[0] = Motor1;
  MotorArray[2] = Motor2;
  MotorArray[0]->setSpeed(50); // 50 revolutions per second
  MotorArray[2]->setSpeed(50); // 50 revolutions per second

  // pinMODE for LED
  pinMode(ledMotor[0], OUTPUT);
  pinMode(ledMotor[1], OUTPUT);
  pinMode(ledMotor[2], OUTPUT);

  // check to use trial reset with plate
  //  if (tubeIndexArray[tubeIndex][1] == 1) {
  //    plateUse = true;
  //  }
  // this should when a certain condition is met
  plateUse = true;
}


// VOID LOOP
void loop() {
  // Condition to test for connection from processing
  if (flagStart == 0) {
    TestConfigContact();  // Read Serial port to trigger test of lights
    // used to disable plate
    for (uint8_t i = 0; i < 3; i++) {
      blockPlate[i] = flagActive[i];
    }
  }

    if (flagStart == 1) {
      testTubeStatus(); // Update tube status
      mpr121_sensor(); // Update Sensors

      // Write serial data
      // Send information to processing
      Serial.print('H'); // unique header to identify start of message
      Serial.print(",");
      for (uint8_t i = 0; i < 3; i++) {
        Serial.print(licksRT[i]); Serial.print(",");
        Serial.print(flag[i]); Serial.print(",");
      }
      Serial.println();

      checkTrial(); // function check to set trial to true or false, also deals with plate timing

      if (validTrial == true) {
        for (uint8_t i = 0; i < 3; i++) {

          // CASE 1: If tube is not active stop light
          // cue-delivery should also turn off if is not a valid trial
          if (flagActive[i] == 0) {
            analogWrite(ledMotor[i], 0); // turn cue-delivery light OFF
            flagLED[i] = 0;
          }
          // if trial is valid, then turn light on
          else if (flag[i] == 0  && flagActive[i] == 1 && validTrial == true) {
            // this is only for tubes
            if (i != 1) {
              analogWrite(ledMotor[i], powerLED); // turn cue-delivery light ON
              flagLED[i] = 1;
            }
          }
          // CASE 3: Check if trigger has been delivered
          if (licksCM[i] % licksActive[i] == 0 && licksCM[i] != 0 && flag[i] == 0 && blockPlate[i] == 1) {  // This is to prevent bugs to trigger pause while already on a pause
            timeStart[i] = millis();

            // Update flags
            flag[i] = 1; // Indicates that a liquid delivery event has been triggered and this stops counting licks to the next delivery
            flagMotor[i] = 1; // Flag to indicate that pump is ON
            flagLED[i] = 0; // Flag to indicate that LED is OFF

            // Deliver liquid through motor
            MotorArray[i]->step(motorSteps[i], FORWARD, MICROSTEP); // motor ON
            MotorArray[i]->release();

            // only tube lights turn OFF
            if (i != 1) {
              analogWrite(ledMotor[i], 0); // cue-delivery light OFF
            }

            // after liquid is delivered the trial ends
            // only when experiment uses plate reset
            if (plateUse == true) {
              validTrial = false;
            }

            // reset counter of cumulative lick counter
            licksCM[i] = 0;

            // Actualizar el numero de licks en bomba PR
            if (i == estacionPR) {
              prEventos = prEventos + 1;
              licksActive[i] = prSeq[prEventos];
            }
          } // close if

          // CASE 4: Determine if motor needs to be turned off and LED ON
          if (flag[i] == 1  && blockPlate[i] == 1) {
            timeEnd = millis();
            // Check if LED has to be turned ON
            if (timeEnd - timeStart[i] >= timeLED_OFF[i] && flagLED[i] == 0) {
              analogWrite(ledMotor[i], powerLED); // turn cue-delivery light ON
              flagLED[i] = 1;
            }
            // Check if time out is over
            if (timeEnd - timeStart[i] >= timeOut[i] || validTrial == true) {
              flag[i] = 0;
            }

          } // close if on case 3
        } // close if on case 1
      } // close loop
    } // close if on flagStart
  } // close void_loop
