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
// This is a macro to return the value of a bit
// in this case is returning the value of one of the 12 pins
// for example _BV(1) will return us the value of pin 1
// _BV(1) is a short-hand for (1 << (1))
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

// variables for capacitance sensor MPR121
// Basic logic is that when the animal starts licking one tube the other one should
// be inactivated, and to reset the animal must stand on a palte for a given
// amount of time.
bool plateChange = false; // if plate changed state (touched / not touched)
bool plateUse = false; // experiment with or without reset
bool validTrial = true; // true: tubes set on; false: tubes set off
bool currentPlateState = false; // check if animal is standing on plate
bool lastPlateState = false; // stores whether animal stood on plate
int startPlateTime = 0; // this variables are for determining how much time
int endPlateTime = 0; // the animal should stand on the plate
int totalPlateTime = 0; // stores how much time animal stood on plate
int timeForReset = 1000; // needed time on plate to restart
