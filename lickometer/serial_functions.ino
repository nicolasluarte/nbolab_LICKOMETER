// This function waits until it hears from Serial about starting the collection protocol
void TestConfigContact() {
  if (Serial.available()) { // If data is available to read, (gets the number of bytes available for reading)
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
    // this is to end the test, and turn the lights off
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
