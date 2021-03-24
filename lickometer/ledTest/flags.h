// Class to provide comunication between sensors and experiment logic
class flags{
  public:
    flags(bool plateActive, bool lickometerActive);
    bool plateActive(); // signals if the plate sensor should be active or not
    bool lickometerActive(); // same but for the lickometers
    void plateOn(); // turn on or off the plate
    void plateOff();
    void lickometerOn(); // same but for the lickometer
    void lickometerOff();
    
  private:
    bool _plateActive;
    bool _lickometerActive;
};

flags::flags(bool plateActive, bool lickometerActive){
  // constructor this allows for status specification at the start of the experiment
    _plateActive = plateActive;
    _lickometerActive = lickometerActive;
}

bool flags::plateActive(){
    return _plateActive;
}

bool flags::lickometerActive(){
    return _lickometerActive;
}

void flags::plateOn(){
    _plateActive = true;
}

void flags::plateOff(){
    _plateActive = false;
}

void flags::lickometerOff(){
    _lickometerActive = false;
}

void flags::lickometerOn(){
    _lickometerActive = true;
}
