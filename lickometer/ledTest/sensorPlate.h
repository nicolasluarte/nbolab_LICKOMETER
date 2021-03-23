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
	private:
		int _pin; // where is connected
    int _ledPin;
    int _ledStatus;
		int _status; // either on or off
    bool _valid;
		int _startMs;
		int _endMs;
    int _heldMs;
		Adafruit_MPR121 cap;
};

sensorPlate::sensorPlate(int pin, int ledPin)
  : trialLed(ledPin)
{
		_pin = pin;
    _ledPin = ledPin;
		Adafruit_MPR121 cap;
    digitalPin trialLead(_ledPin);
}

void sensorPlate::sense()
	// change the status when the animal steps on
{
		if (cap.touched() & (1 << _pin)){
			_status = 1;
			_startMs = millis();
		}
		else{
			_status = 0;
			_endMs = millis();
		}
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
