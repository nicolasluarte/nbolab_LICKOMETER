void plateTimer() {
  if (currentPlateState == 1) {
    startPlateTime = millis();
  }
  else {
    endPlateTime = millis();
    totalPlateTime = endPlateTime - startPlateTime;
  }
}

void checkPlateChange() {
  if (currentPlateState != lastPlateState) {
    plateChange = true;
  }
  else {
    plateChange = false;
  }
}

void checkTrial() {
  // check for plate reset
  if (plateUse == true) {
    if (validTrial == false) {
      analogWrite(ledMotor[1], powerLED); // if trial is inactive plate light is ON
      flagLED[1] = 1;
      blockPlate[1] = 1;
    }
    else {
      blockPlate[1] = 0;
      // if the trial is valid only tube lights should be ON
      analogWrite(ledMotor[1], 0);
      flagLED[1] = 0;
    }
    currentPlateState = cap.touched() & (1 << 1); // get state of pin 1
    checkPlateChange();
    if (plateChange == true && validTrial == false && blockPlate[1] == 1) {
      plateTimer();
    }
    if (totalPlateTime >= timeForReset && validTrial == false && blockPlate[1] == 1) {
      validTrial = true; // trial now valid
      totalPlateTime = 0; // counter is reseted
    }
    else
    {
      totalPlateTime = 0; // counter is reseted, this avoids non-intentional resets
    }
    lastPlateState = currentPlateState;
  }
}
