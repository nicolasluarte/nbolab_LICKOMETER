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
