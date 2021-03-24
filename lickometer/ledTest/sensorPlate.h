#include "Adafruit_MPR121.h" //  MPR121 breakout board library
#include <Adafruit_MotorShield.h> // Library for Adafruit MotorShield v2
#include "leds.h"
// Class for the sensor plate that resets the trials
class sensorPlate: public Adafruit_MPR121{
	public:
		sensorPlate(int pin, int ledPin);
    digitalPin trialLed;
		void begin();
		void sense();
		bool status();
		int held();
    int statusSum();
    int lastStatus();
	private:
		int _pin; // where is connected
    int _ledPin;
    int _ledStatus;
		int _status; // either on or off
		int _startMs;
		int _endMs;
    int _heldMs;
    int _statusSum;
    int _lastStatus;
		Adafruit_MPR121 cap;
};

sensorPlate::sensorPlate(int pin, int ledPin)
  : trialLed(ledPin)
{
		_pin = pin;
    _ledPin = ledPin;
    _statusSum = 0;
		Adafruit_MPR121 cap;
    digitalPin trialLead(_ledPin);
}

void sensorPlate::sense()
	// change the status when the animal steps on
{
		if (cap.touched() & (1 << _pin)){
			_status = 1;
			_startMs = millis();
      if (_lastStatus != _status){
        _statusSum++;
      }
		}
		else{
			_status = 0;
			_endMs = millis();
		}
    _lastStatus = _status;
}

int sensorPlate::held()
	// change the status when the animal steps on
{
    if (_status == 1){
      _heldMs = _startMs - _endMs;
    }
    else{
      _heldMs = 0;
    }
    return _heldMs;
}

void sensorPlate::begin()
{
    cap.begin(0x5A);
    trialLed.begin();
}

bool sensorPlate::status()
{
		return _status;
}

int sensorPlate::statusSum()
{
    return _statusSum;
}

int sensorPlate::lastStatus()
{
    return _lastStatus;
}
