// Class to provide comunication between sensors and experiment logic
class flags {
  public:
    flags(bool plateActive, bool lickometerActive, int ratio, String paradigm);
    bool plateActive(); // signals if the plate sensor should be active or not
    bool lickometerActive(); // same but for the lickometers
    void plateOn(); // turn on or off the plate
    void plateOff();
    void lickometerOn(); // same but for the lickometer
    void lickometerOff();
    int isEvent(int validLickSum);
    void event();
    int ratio();
    String paradigm();
    int totalEvents();
    int lastEvent();
    void validLicks();
    void createRatio(); // this functions create the ratio for the trial, given the paradigm string

  private:
    bool _plateActive;
    bool _lickometerActive;
    int _event;
    int _ratio;
    int _totalEvents;
    int _lastTotalEvents;
    int _lastEvent;
    int _validLicks;
    int _min;
    int _max;
    String _paradigm;
};

flags::flags(bool plateActive, bool lickometerActive, int ratio, String paradigm) {
  // constructor this allows for status specification at the start of the experiment
  _plateActive = plateActive;
  _lickometerActive = lickometerActive;
  _ratio = ratio;
  _totalEvents = 0;
  _paradigm = paradigm;
  randomSeed(analogRead(0));
  _min = (int) _ratio - (_ratio / 2);
  _max = (int) _ratio + (_ratio / 2);
}

bool flags::plateActive() {
  return _plateActive;
}

bool flags::lickometerActive() {
  return _lickometerActive;
}

void flags::plateOn() {
  _plateActive = true;
}

void flags::plateOff() {
  _plateActive = false;
}

void flags::lickometerOff() {
  _lickometerActive = false;
}

void flags::lickometerOn() {
  _lickometerActive = true;
}

void flags::event() {
  return _event;
}

void flags::validLicks() {
  return _validLicks;
}

int flags::ratio() {
  return _ratio;
}

String flags::paradigm() {
  return _paradigm;
}

int flags::totalEvents() {
  return _totalEvents;
}

void flags::createRatio() {
  if (_paradigm == "FR") {
    _ratio == _ratio;
  }
  else if (_paradigm == "RR") {
    _ratio = random(_min, _max); // random uniform, same mean as fixed ratio
  }
}

int flags::isEvent(int validLickSum) {
  if (validLickSum % _ratio == 0 && validLickSum != 0) {
    _event = 1;
    if (_lastEvent != _event) {
      _totalEvents++;
    }
  }
  else {
    _event = 0;
    _lastTotalEvents = _totalEvents;
  }
  _lastEvent = _event;
  return _event;
}
