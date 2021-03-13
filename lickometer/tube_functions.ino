void testTubeStatus() {
if (Serial.available() > 0) { // If data is available to read,
    char val = Serial.read(); // read it and store it in val
    if(val >= '0' && val <= '9') { // is this an ascii digit between 0 and 9?
      tubeIndex = (val - '0');
      flagActive[0] = tubeIndexArray[tubeIndex][0];
      flagActive[1] = tubeIndexArray[tubeIndex][1];
      flagActive[2] = tubeIndexArray[tubeIndex][2];
  }
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
