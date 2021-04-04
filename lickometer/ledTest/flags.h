// Class to provide comunication between sensors and experiment logic
class flags {
  public:
    flags(bool plateActive, bool lickometerActive, int ratio, int paradigm);
    bool plateActive(); // signals if the plate sensor should be active or not
    bool lickometerActive(); // same but for the lickometers
    void plateOn(); // turn on or off the plate
    void plateOff();
    void lickometerOn(); // same but for the lickometer
    void lickometerOff();
    int isEvent(int validLickSum);
    void event();
    int ratio();
    void setRatio(int r);
    int paradigm();
    void setParadigm(int p);
    int totalEvents();
    int lastEvent();
    void createRatio(); // this functions create the ratio for the trial, given the paradigm string
    void setHeldValid(bool v);
    bool heldValid();
    bool eventChanged();
    void setEventChanged();
    int eventMs();
    bool timeOut(int timeOutMs);


  private:
    bool _plateActive;
    bool _lickometerActive;
    int _event;
    int _ratio;
    int _RATIO;
    int _totalEvents;
    int _lastTotalEvents;
    int _lastEvent;
    bool _eventChanged;
    bool _heldValid;
    int _validLicks;
    int _min;
    int _max;
    int _paradigm;
    int _eventMs;
};

flags::flags(bool plateActive, bool lickometerActive, int ratio, int paradigm) {
  // constructor this allows for status specification at the start of the experiment
  _plateActive = plateActive;
  _lickometerActive = lickometerActive;
  _RATIO = ratio;
  _totalEvents = 0;
  _paradigm = paradigm;
  randomSeed(analogRead(0));
  _min = (int) _ratio - (_ratio / 2);
  _max = (int) _ratio + (_ratio / 2);
}

bool flags::plateActive() {
  return _plateActive;
}

bool flags::timeOut(int timeOutMs) {
  if (_totalEvents != 0) {
    if ((millis() - _eventMs) > timeOutMs){
      return true;
    }
    else{
      return false;
    }
  }
  else if (_totalEvents= 0){
    return true;
  }
}

int flags::eventMs() {
  return _eventMs;
}

bool flags::eventChanged() {
  return _eventChanged;
}

void flags::setEventChanged() {
  _eventChanged = false;
}

void flags::setHeldValid(bool v) {
  _heldValid = v;
}

bool flags::heldValid() {
  return _heldValid;
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

int flags::ratio() {
  return _ratio;
}

void flags::setRatio(int r) {
  _ratio = r;
}

int flags::paradigm() {
  return _paradigm;
}

void flags::setParadigm(int p) {
  _paradigm = p;
}

int flags::totalEvents() {
  return _totalEvents;
}

void flags::createRatio() {
  if (_paradigm == 0) {
    _ratio == _RATIO;
  }
  if (_paradigm == 1) {
    _min = (int) _RATIO - (_RATIO / 2);
    _max = (int) _RATIO + (_RATIO / 2);
    _ratio = random(_min, _max); // random uniform, same mean as fixed ratio
  }
}

int flags::isEvent(int validLickSum) {
  if (validLickSum % _ratio == 0 && validLickSum != 0) {
    _event = 1;
    if (_lastEvent != _event) {
      _totalEvents++;
      _eventChanged = true;
      _eventMs = millis();
    }
  }
  else {
    _event = 0;
    _lastTotalEvents = _totalEvents;
  }
  _lastEvent = _event;
  return _event;
}
