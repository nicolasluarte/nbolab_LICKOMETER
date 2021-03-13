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
