void plateTimer() {
  if (currentPlateState == 1) {
    startPlateTime = millis();
  }
  else {
    endPlateTime = millis();
    totalPlateTime = endPlateTime - startPlateTime;
  }
}

void checkPlateChange(){
  if (currentPlateState != lastPlateState){
    plateChange = true;
  }
  else{
    plateChange = false;
  }
}
