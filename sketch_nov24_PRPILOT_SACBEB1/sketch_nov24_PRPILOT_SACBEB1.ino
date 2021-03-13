/*********************************************************
PROGRAM: PR (PROGRESSIVE RATIO PILOT) for one pump and inactive pumps
Date: November 24, 2020

DESCRIPTION: This program sets an progressive ratio on a linear schedule for pilot test in november 2020

# VARIABLES
# Time out for drinking tube one is 20 seconds.

Written by Claudio Perez-Leighton.  
**********************************************************/
// Libraries codes
#include <Wire.h>
#include "Adafruit_MPR121.h" //  MPR121 breakout board library
#include <Adafruit_MotorShield.h> // Library for Adafruit MotorShield v2

/**********************************************************
OPERATIVE VARIABLES FOR LICKOMETER
***********************************************************/
// You can have up to 4 on one i2c bus but one is enough for testing!
Adafruit_MPR121 cap = Adafruit_MPR121();

// I do not know what this is
#ifndef _BV
#define _BV(bit) (1 << (bit))
#endif

// Keeps track of the last pins touched so we know when buttons are 'released'
uint16_t lasttouched = 0; // uint16_t are integers of up to 16 bits.
uint16_t currtouched = 0;

/**********************************************************
VARIABLES FOR LEDS
***********************************************************/
// AlL LEDs are connected to PWM digital outpus to start at half 
int ledMotor[] = {3, 5, 6}; // the pin the LED is connected to

/**********************************************************
OPERATIVE VARIABLES FOR PUMPS
***********************************************************/
Adafruit_MotorShield AFMS = Adafruit_MotorShield();

// Connect a stepper motor with 200 steps per revolution (1.8 degree)
// to motor port #2 (M3 and M4)
Adafruit_StepperMotor*  MotorArray[3]; // Declare array
Adafruit_StepperMotor *Motor1 = AFMS.getStepper(200, 1); // M1 and M2
Adafruit_StepperMotor *Motor2 = AFMS.getStepper(200, 2); // M3 and M3

/**********************************************************
CONTROL STRUCTURES AND VARIABLES
***********************************************************/
// Variables for serial communication
int tubeIndexArray[8][3]  = {  {0,0,0},
                                {1,0,0},
                                {0,1,0},
                                {0,0,1},
                                {1,1,0},
                                {1,0,1},
                                {0,1,1},
                                {1,1,1}
                                };
int tubeIndex;

// lick trackers
uint8_t licksRT[3] = {0,0,0}; // array that contains the current state of the sensor, 0 is not touched and 1 is touched
uint8_t licksCM[3] = {0,0,0}; // Cumulative licks per animal to store until trigger

// times for operation
unsigned long timeOut[3] = {20000,20000,20000}; // delay for a new set of licks to activate each pump - miliseconds.
unsigned long timeLED_OFF[3] = {20000,20000,20000}; // This works for each LED and it will be inactive after the delivery of water
uint16_t motorSteps[3] = {12,12,12}; // Number of steps - this is calibrated for a 20 mL syringue
unsigned long timeStart[3]; // This works for both the LED and the Motor
unsigned long timeEnd;

// Modes of operation
byte licksActive[3] = {5,3,5}; // numero de licks para activar cada bomba en FR. En el caso del PR es el numero inicial de PR
uint8_t estacionPR = 0; // Este define el puerto que esta en PR, debe ser 0 o 2.
uint8_t prEventos = 0; // lleva la cantidad de eventos de PR ocurridos para poder actualizar la secuencia de PR
// Este array contiene la secuencia de PR segun formula 5 + cumsum(seq(0,500, by = 2))
uint8_t prSeq[180] = {  5,7,11,17,25,35,47,61,
                            77,95,115,137,161,187,215,245,
                            277,311,347,385,425,467,511,557,
                            605,655,707,761,817,875,935,997,
                            1061,1127,1195,1265,1337,1411,1487,1565,
                            1645,1727,1811,1897,1985,2075,2167,2261,
                            2357,2455,2555,2657,2761,2867,2975,3085,
                            3197,3311,3427,3545,3665,3787,3911,4037,
                            4165,4295,4427,4561,4697,4835,4975,5117,
                            5261,5407,5555,5705,5857,6011,6167,6325,
                            6485,6647,6811,6977,7145,7315,7487,7661,
                            7837,8015,8195,8377,8561,8747,8935,9125,
                            9317,9511,9707,9905,10105,10307,10511,10717,
                            10925,11135,11347,11561,11777,11995,12215,12437,
                            12661,12887,13115,13345,13577,13811,14047,14285,
                            16517,16775,17035,17297,17561,17827,18095,18365,
                            18637,18911,19187,19465,19745,20027,20311,20597,
                            20885,21175,21467,21761,22057,22355,22655,22957,
                            23261,23567,23875,24185,24497,24811,25127,25445,
                            25765,26087,26411,26737,27065,27395,27727,28061,
                            28397,28735,29075,29417,29761,30107,30455,30805,
                            31157,31511,31867,32225
                          };

// Light and motor power
int powerLED = 10; // This goes from zero to 255
double powerMotor = 0.6; // This is in percent. Less than 60% the motors don't really run

// flags
int flag[3] = {0,0,0}; // This will indicate whether a liquid delivery event has been triggered and touches should not count for triggering liquid delivery
int flagLED[3] = {0,0,0}; // This will indicate whether LED is active - 0 is not active and 1 is active
int flagMotor[3] = {0,0,0}; // This will indicate whether motor is active
int flagStart = 0; // This indicates whether data requesting start of the experiment has been sent by processing and received over serial
int flagActive[3] = {0,0,0}; // This indicates which drinking tube is active - 0 is not active, 1 is active
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

  // This is legacy, but it could work
  Serial.println("Adafruit MPR121 Capacitive Touch sensor test");  
  // Default address is 0x5A, if tied to 3.3V its 0x5B
  // If tied to SDA its 0x5C and if SCL then 0x5D
  if (!cap.begin(0x5A)) {
    Serial.println("MPR121 not found, check wiring?");
    while (1);
  }
  Serial.println("MPR121 found!");

  // Adjust threshold for MPR121
  cap.writeRegister(MPR121_ECR, 0x00);
  cap.setThresholds(0x14, 0x07); // This is the threshold that was found not to cause any false positives
  cap.writeRegister(MPR121_ECR, 0x8F);

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
  }

// VOID LOOP
void loop() {  
  // Condition to test for connection from processing
  if (flagStart == 0) {
      TestConfigContact();  // Read Serial port to trigger test of lights      
    }
     
  if (flagStart == 1) {
    testTubeStatus(); // Update tube status
    mpr121_sensor(); // Update Sensors    

      // Write serial data
     // Send information to processing
    Serial.print('H'); // unique header to identify start of message
    Serial.print(",");
    for (uint8_t i=0; i<3; i++) {
      Serial.print(licksRT[i]); Serial.print(",");  
      Serial.print(flag[i]); Serial.print(",");  
      }  
    Serial.println();
   
    for (uint8_t i=0; i<3; i++) {
      // CASE 1: If tube is not active stop light
      if (flagActive[i] == 0) {
            analogWrite(ledMotor[i],0); // turn cue-delivery light OFF
            flagLED[i] = 0;
            } else {
            if (flag[i] == 0  && flagActive[i] == 1) {
                analogWrite(ledMotor[i],powerLED); // turn cue-delivery light OFF
                flagLED[i] = 1;          
                }  
              // CASE 3: Check if trigger has been delivered
              if (licksCM[i] % licksActive[i] == 0 && licksCM[i] != 0 && flag[i] == 0 && flagActive[i] == 1) {  // This is to prevent bugs to trigger pause while already on a pause
                timeStart[i] = millis();
               
                // Update flags
                flag[i] = 1; // Indicates that a liquid delivery event has been triggered and this stops counting licks to the next delivery
                flagMotor[i] = 1; // Flag to indicate that pump is ON
                flagLED[i] = 0; // Flag to indicate that LED is OFF
         
                // Deliver liquid through motor
                MotorArray[i]->step(motorSteps[i], FORWARD, MICROSTEP); // motor ON
                MotorArray[i]->release();
                analogWrite(ledMotor[i],0); // cue-delivery light OFF    
         
                // reset counter of cumulative lick counter
                licksCM[i] = 0;

                // Actualizar el numero de licks en bomba PR
                if (i == estacionPR) {
                  prEventos = prEventos + 1;                      
                  licksActive[i] = prSeq[prEventos]                                
;                  }
                } // close if

            // CASE 4: Determine if motor needs to be turned off and LED ON
            if (flag[i] == 1  && flagActive[i] == 1) {
                timeEnd = millis();
                // Check if LED has to be turned ON
                if(timeEnd - timeStart[i] >= timeLED_OFF[i] && flagLED[i] == 0) {
                  analogWrite(ledMotor[i],powerLED); // turn cue-delivery light ON
                  flagLED[i] = 1;
                  }
               
                // Check if time out is over
                if(timeEnd - timeStart[i] >= timeOut[i]) { flag[i] = 0; }
              } // close if on case 3
            } // close if on case 1
        } // close loop      
    } // close if on flagStart
} // close void_loop

// This function waits until it hears from Serial about starting the collection protocol
void TestConfigContact() {
  if (Serial.available()) { // If data is available to read,
    char val = Serial.read(); // read it and store it in val
    // This test connection from Processing by flicking all three LEDS
    if (val == 'T') {
       blinkTubeLights(1000, true);
      }  

   // This updates the status of the LEDS  
   if(val >= '0' && val <= '9') { // is this an ascii digit between 0 and 9?
      tubeIndex = (val - '0');
      flagActive[0] = tubeIndexArray[tubeIndex][0];
      flagActive[1] = tubeIndexArray[tubeIndex][1];
      flagActive[2] = tubeIndexArray[tubeIndex][2];
      blinkTubeLights(500, false);        
      }    

   // Activates all pumps and lights for 5 seconds
   if (val == 'B') {
   
    // Iterate over pumps and lights
    for (uint8_t i=0; i<3; i++) {
        if (flagActive[i] == 1) {
          MotorArray[i]->setSpeed(200); // RPM
          MotorArray[i]->step(400, FORWARD, SINGLE); // motor ON for three revolutions
          analogWrite(ledMotor[i],25); // cue-delivery light ON    
          }
        }
    delay(500); // I do not care about blocking code because the program should not be doing anything else right now, so blocking the code is fine
    for (uint8_t i=0; i<3; i++) {
      if (flagActive[i] == 1) {      
        MotorArray[i]->release();
        MotorArray[i]->setSpeed(50); // 50 revolutions per second
        analogWrite(ledMotor[i],0); // cue-delivery light OFF    
        }
    }
   }

   // Starts test  
   if (val == 'S') { // valid header
        flagStart = 1; // Start Test  
      }    
  }
}

void testTubeStatus() {
if (Serial.available() > 0) { // If data is available to read,
    char val = Serial.read(); // read it and store it in val
    if(val >= '0' && val <= '9') { // is this an ascii digit between 0 and 9?
      tubeIndex = (val - '0');
      flagActive[0] = tubeIndexArray[tubeIndex][0];
      flagActive[1] = tubeIndexArray[tubeIndex][1];
      flagActive[2] = tubeIndexArray[tubeIndex][2];
  }
} }

void blinkTubeLights(int Delay, boolean flag) {
      // Iterate over each drinking tube and blink each light three times
      for (int i=0; i<3; i++) {
        if (flag) {
          for (int q = 0; q < 3; q++) {          
            analogWrite(ledMotor[q],25);
            }
          } else {
            for (int q = 0; q < 3; q++) {
              if (flagActive[q] == 1) { analogWrite(ledMotor[q],25); }
              }              
          }
        delay(Delay); // I do not care about blocking code because the program should not be doing anything else right now.
        for (int q = 0; q < 3; q++) { analogWrite(ledMotor[q],0); }
        delay(Delay); // I do not care about blocking code because the program should not be doing anything else right now.
        }
}


// This function by default checks the three sensors and updates the real-time value of each sensor
void mpr121_sensor() {
  currtouched = cap.touched(); // Get currently touched pads
  for (uint8_t i=0; i<3; i++) {
    if ((currtouched & _BV(i)) && !(lasttouched & _BV(i)) ) {
      licksRT[i] = 1;  // it if *is* touched and *wasnt* touched before, alert!
      if (flag[i] == 0) { licksCM[i]++; }  
      }
   
    // Update sensor off
    if (!(currtouched & _BV(i)) && (lasttouched & _BV(i)) ) {
      licksRT[i] = 0; // if it *was* touched and now *isnt*, alert!      
      }
    }
  // reset our state
  lasttouched = currtouched;
  }
