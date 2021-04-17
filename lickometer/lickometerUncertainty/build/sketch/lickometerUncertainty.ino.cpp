#include <Arduino.h>
#line 1 "/home/nicoluarte/nbolab_LICKOMETER/lickometer/lickometerUncertainty/lickometerUncertainty.ino"
/*********************************************************
  PROGRAMAS: FR (fixed ratio) para bombas
  Fecha: Noviembre 13, 2020

  Notas
  1.- Permite setear FR para hasta 2 bombas manejadas por motores stepper.
  2.- Funciona con el programa de Processing sketch_lickometer_read_stable_v4.pde
  3.- Este programa fue construido sobre una versión que usaba bombas peristálticas controladas por motores 12V DC. Por tanto,
    el código esta escrito para tres bombas aunque la tarjeta Arduino Uno R3 solo puede manejar 2 motores stepper.
  4.- Codigo comentado en MArzo 4, 2021.

  Escrito por Claudio Perez-Leighton.
**********************************************************/
// Libraries codes
#include <Wire.h> // conectividad general
#include "Adafruit_MPR121.h" //  MPR121 breakout board library - capacitance sensor
#include <Adafruit_MotorShield.h> // Library for Adafruit MotorShield v2 - motor control

/**********************************************************
  OPERATIVE VARIABLES FOR LICKOMETER
***********************************************************/
// You can have up to 4 on one i2c bus but one is enough for testing!
Adafruit_MPR121 cap = Adafruit_MPR121();

// I do not know what this is, but it's in all codes associated to MPR121
#ifndef _BV
#define _BV(bit) (1 << (bit))
#endif

// Keeps track of the last pins touched so we know when buttons are 'released'
uint16_t lasttouched = 0; // uint16_t are integers of up to 16 bits.
uint16_t currtouched = 0;

/**********************************************************
  VARIABLES FOR LEDS
***********************************************************/
// LEDs estan conectados a PWM para potencia de 50%
int ledMotor[] = {3, 5, 6}; // pin3 = sensor 0 MPR121, pin5 = sensor 1 MPR121, pin6 = sensor 2 MPR121

/**********************************************************
  OPERATIVE VARIABLES FOR PUMPS
***********************************************************/
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); // define variable that refers to motorshield

// Connect a stepper motor with 200 steps per revolution (1.8 degree)
// to motor port #2 (M3 and M4)
Adafruit_StepperMotor*  MotorArray[3]; // Declare array of motor
Adafruit_StepperMotor *Motor1 = AFMS.getStepper(200, 1); // ports M1 and M2 of motorshield
Adafruit_StepperMotor *Motor2 = AFMS.getStepper(200, 2); // ports M3 and M4 of motorshield

/**********************************************************
  CONTROL STRUCTURES AND VARIABLES
***********************************************************/
// VARIABLES DE CONTROL DE USUARIO
// El array licksActive es donde se define el numero de licks para cada motor
byte licksActive[3] = {5, 5, 5}; // numero de licks para cada bebedero
long PROBABILITY = 0.5; // defines the probability of obtaining a reward
long randomNumber;

// TIEMPOS FUERA. El programa da la opcion de setear tiempos fuera independientes para cada bomba
// y para cada LED. Sin embargo, en operacion todos estos tiempos debieran ser los mismos
unsigned long timeOut[3] = {20000, 20000, 20000}; // delay for a new set of licks to activate each pump - miliseconds.
unsigned long timeLED_OFF[3] = {20000, 20000, 20000}; // This works for each LED and it will be inactive after the delivery of fluid

// NO MODIFICAR ESTAS VARIABLES
// Variables for serial communication
int tubeIndexArray[8][3]  = {  {0, 0, 0},
    {1, 0, 0},
    {0, 1, 0},
    {0, 0, 1},
    {1, 1, 0},
    {1, 0, 1},
    {0, 1, 1},
    {1, 1, 1}
};
int tubeIndex;

// arrays para conteo de licks en tiempo real (RT) y cumulativos (CM)
uint8_t licksRT[3] = {0, 0, 0}; // array that contains the current state of the sensor, 0 is not touched and 1 is touched
uint8_t licksCM[3] = {0, 0, 0}; // Cumulative licks per animal to store until trigger
int liquidDelivered[3] = {0, 0, 0}; // keeps track if liquid was delivered or not
int randomSpout[3] = {1, 0, 1} ; // set the random spout

// Variables de control interno de motores. NO MODIFICAR!!!
uint16_t motorSteps[3] = {12, 12, 12}; // Number of steps - this is calibrated for a 20 mL syringue
unsigned long timeStart[3]; // This works for both the LED and the Motor
unsigned long timeEnd;


// VARIABLES DE OPERACION
int powerLED = 10; // Potencia del LED. Maximo es 255. Esto se ha determinado que funciona de forma apropiada
double powerMotor = 0.6; //En porcentaje. Menos de un 60% los motores stepper no funcionan.

// Flags para control interno. NO MODIFICAR!!!
int flag[3] = {0, 0, 0}; // This will indicate whether a liquid delivery event has been triggered and touches should not count for triggering liquid delivery
int flagLED[3] = {0, 0, 0}; // This will indicate whether LED is active - 0 is not active and 1 is active
int flagMotor[3] = {0, 0, 0}; // This will indicate whether motor is active
int flagStart = 0; // This indicates whether data requesting start of the experiment has been sent by processing and received over serial
int flagActive[3] = {0, 0, 0}; // This indicates which drinking tube is active - 0 is not active, 1 is active
/************************************************************************
*************************************************************************/
// SETUP
#line 103 "/home/nicoluarte/nbolab_LICKOMETER/lickometer/lickometerUncertainty/lickometerUncertainty.ino"
void setup();
#line 144 "/home/nicoluarte/nbolab_LICKOMETER/lickometer/lickometerUncertainty/lickometerUncertainty.ino"
void loop();
#line 2 "/home/nicoluarte/nbolab_LICKOMETER/lickometer/lickometerUncertainty/functions.ino"
void TestConfigContact();
#line 47 "/home/nicoluarte/nbolab_LICKOMETER/lickometer/lickometerUncertainty/functions.ino"
void testTubeStatus();
#line 59 "/home/nicoluarte/nbolab_LICKOMETER/lickometer/lickometerUncertainty/functions.ino"
void blinkTubeLights(int Delay, boolean flag);
#line 83 "/home/nicoluarte/nbolab_LICKOMETER/lickometer/lickometerUncertainty/functions.ino"
void mpr121_sensor();
#line 103 "/home/nicoluarte/nbolab_LICKOMETER/lickometer/lickometerUncertainty/lickometerUncertainty.ino"
void setup() {
    // Initialize serial @ 115200 bits per second
    Serial.begin(115200);

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

    // Agregar items a array de motor.
    // IMPORTANTE: La posición del medio del array esta vacia y por lo tanto no tiene ninguna acción. Debiera modificarse en versiones futuras
    AFMS.begin();  // create with the default frequency 1.6KHz
    MotorArray[0] = Motor1;
    MotorArray[2] = Motor2;
    MotorArray[0]->setSpeed(50); // 50 revolutions per second
    MotorArray[2]->setSpeed(50); // 50 revolutions per second

    // pinMODE for LED
    pinMode(ledMotor[0], OUTPUT);
    pinMode(ledMotor[1], OUTPUT);
    pinMode(ledMotor[2], OUTPUT);

    randomSeed(analogRead(A0)); // read a pin to generate a random seed, pin must not be used
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
        for (uint8_t i = 0; i < 3; i++) {
            Serial.print(licksRT[i]);
            Serial.print(",");
            Serial.print(flag[i]);
            Serial.print(",");
            Serial.print(liquidDelivered[i]);
            Serial.print(",");
            Serial.print(randomSpout[i]);
            Serial.print(",");
        }
        Serial.println();

        for (uint8_t i = 0; i < 3; i++) {
            // CASE 1: If tube is not active stop light
            if (flagActive[i] == 0) {
                analogWrite(ledMotor[i], 0); // turn cue-delivery light OFF
                flagLED[i] = 0;
            } else {
                // CASE 2: If tube is active but I am within a time off.
                if (flag[i] == 0  && flagActive[i] == 1) {
                    analogWrite(ledMotor[i], powerLED); // turn cue-delivery light OFF
                    flagLED[i] = 1;
                }
                // CASE 3: Check if trigger has been delivered
                if (licksCM[i] % licksActive[i] == 0 && licksCM[i] != 0 && flag[i] == 0 && flagActive[i] == 1) {  // This is to prevent bugs to trigger pause while already on a pause
                    timeStart[i] = millis();

                    // Update flags
                    flag[i] = 1; // Indicates that a liquid delivery event has been triggered and this stops counting licks to the next delivery
                    flagMotor[i] = 1; // Flag to indicate that pump is ON
                    flagLED[i] = 0; // Flag to indicate that LED is OFF


                    randomNumber = random(0, 2);
                    // checks if the current spout should be random or not
                    if (randomSpout[i] == 1) {
                        if (randomNumber > PROBABILITY) {
                            // Deliver liquid through motor
                            MotorArray[i]->step(motorSteps[i], FORWARD, MICROSTEP); // motor ON
                            MotorArray[i]->release();
                            analogWrite(ledMotor[i], 0); // cue-delivery light OFF
                            ++liquidDelivered[i];
                        }
                        else {
                            analogWrite(ledMotor[i], 0);
                        }
                    }
                    // the current spout is not random so program runs as usual
                    else {
                        // Deliver liquid through motor
                        MotorArray[i]->step(motorSteps[i], FORWARD, MICROSTEP); // motor ON
                        MotorArray[i]->release();
                        analogWrite(ledMotor[i], 0); // cue-delivery light OFF
                        ++liquidDelivered[i];
                    }

                    // reset counter of cumulative lick counter
                    licksCM[i] = 0;
                } // close if

                // CASE 4: Determine if time out is over and LED needs to be turned ON
                if (flag[i] == 1  && flagActive[i] == 1) {
                    timeEnd = millis();
                    // Check if LED has to be turned ON
                    if (timeEnd - timeStart[i] >= timeLED_OFF[i] && flagLED[i] == 0) {
                        analogWrite(ledMotor[i], powerLED); // turn cue-delivery light ON
                        flagLED[i] = 1;
                    }

                    // Check if time out is over
                    if (timeEnd - timeStart[i] >= timeOut[i]) {
                        flag[i] = 0;
                    }
                } // close if on case 3
            } // close if on case 1
        } // close loop
    } // close if on flagStart
} // close void_loop


#line 1 "/home/nicoluarte/nbolab_LICKOMETER/lickometer/lickometerUncertainty/functions.ino"
// This function waits until it hears from Serial about starting the collection protocol
void TestConfigContact() {
    if (Serial.available()) { // If data is available to read,
        char val = Serial.read(); // read it and store it in val
        // This test connection from Processing by flicking all three LEDS
        if (val == 'T') {
            blinkTubeLights(1000, true);
        }

        // This updates the status of the LEDS
        if (val >= '0' && val <= '9') { // is this an ascii digit between 0 and 9?
            tubeIndex = (val - '0');
            flagActive[0] = tubeIndexArray[tubeIndex][0];
            flagActive[1] = tubeIndexArray[tubeIndex][1];
            flagActive[2] = tubeIndexArray[tubeIndex][2];
            blinkTubeLights(500, false);
        }

        // Activates all pumps and lights for 5 seconds
        if (val == 'B') {

            // Iterate over pumps and lights
            for (uint8_t i = 0; i < 3; i++) {
                if (flagActive[i] == 1) {
                    MotorArray[i]->setSpeed(200); // RPM
                    MotorArray[i]->step(400, FORWARD, SINGLE); // motor ON for three revolutions
                    analogWrite(ledMotor[i], 25); // cue-delivery light ON
                }
            }
            delay(500); // I do not care about blocking code because the program should not be doing anything else right now, so blocking the code is fine
            for (uint8_t i = 0; i < 3; i++) {
                if (flagActive[i] == 1) {
                    MotorArray[i]->release();
                    MotorArray[i]->setSpeed(50); // 50 revolutions per second
                    analogWrite(ledMotor[i], 0); // cue-delivery light OFF
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
        if (val >= '0' && val <= '9') { // is this an ascii digit between 0 and 9?
            tubeIndex = (val - '0');
            flagActive[0] = tubeIndexArray[tubeIndex][0];
            flagActive[1] = tubeIndexArray[tubeIndex][1];
            flagActive[2] = tubeIndexArray[tubeIndex][2];
        }
    }
}

void blinkTubeLights(int Delay, boolean flag) {
    // Iterate over each drinking tube and blink each light three times
    for (int i = 0; i < 3; i++) {
        if (flag) {
            for (int q = 0; q < 3; q++) {
                analogWrite(ledMotor[q], 25);
            }
        } else {
            for (int q = 0; q < 3; q++) {
                if (flagActive[q] == 1) {
                    analogWrite(ledMotor[q], 25);
                }
            }
        }
        delay(Delay); // I do not care about blocking code because the program should not be doing anything else right now.
        for (int q = 0; q < 3; q++) {
            analogWrite(ledMotor[q], 0);
        }
        delay(Delay); // I do not care about blocking code because the program should not be doing anything else right now.
    }
}


// This function by default checks the three sensors and updates the real-time value of each sensor
void mpr121_sensor() {
    currtouched = cap.touched(); // Get currently touched pads
    for (uint8_t i = 0; i < 3; i++) {
        if ((currtouched & _BV(i)) && !(lasttouched & _BV(i)) ) {
            licksRT[i] = 1;  // it if *is* touched and *wasnt* touched before, alert!
            if (flag[i] == 0) {
                licksCM[i]++;
            }
        }

        // Update sensor off
        if (!(currtouched & _BV(i)) && (lasttouched & _BV(i)) ) {
            licksRT[i] = 0; // if it *was* touched and now *isnt*, alert!
        }
    }
    // reset our state
    lasttouched = currtouched;
}

