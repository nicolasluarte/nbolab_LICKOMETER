// Class to provide comunication between sensors and experiment logic
class flags{
  public:
    flags(bool plateActive, bool lickometerActive, int ratio);
    bool plateActive(); // signals if the plate sensor should be active or not
    bool lickometerActive(); // same but for the lickometers
    void plateOn(); // turn on or off the plate
    void plateOff();
    void lickometerOn(); // same but for the lickometer
    void lickometerOff();
    int isEvent(int statusSum);
    void event();
    void ratio();
    int totalEvents();
    int lastEvent();
    
  private:
    bool _plateActive;
    bool _lickometerActive;
    int _event;
    int _ratio;
    int _totalEvents;
    int _lastEvent;
};

flags::flags(bool plateActive, bool lickometerActive, int ratio){
  // constructor this allows for status specification at the start of the experiment
    _plateActive = plateActive;
    _lickometerActive = lickometerActive;
    _ratio = ratio;
    _totalEvents = 0;
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

void flags::event(){
    return _event;
}

void flags::ratio(){
    return _ratio;
}

int flags::totalEvents(){
    return _totalEvents;
}

int flags::isEvent(int eventSum){
    if (eventSum % _ratio == 0 && eventSum != 0){
      _event = 1;
      if (_lastEvent != _event){
        _totalEvents++;
      }
    }
    else{
      _event = 0;
    }
    _lastEvent = _event;
    return _event;
}
