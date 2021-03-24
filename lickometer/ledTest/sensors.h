#include "Adafruit_MPR121.h" //  MPR121 breakout board library
#include <Adafruit_MotorShield.h> // Library for Adafruit MotorShield v2
#include "leds.h"
// This class picks any MPR121 sensor and provides basic functionality
// 1. Detect if being touched or not
// 2. Count how many time has been touched
// 3. For how long it has been touched
class sensors: public Adafruit_MPR121{
	public:
		sensors(int pin, int ledPin);
    digitalPin trialLed; // every sensor has 1 assigned LED, could be used as cue or something else
		void begin(); // function to start the led and the sensors
		void sense(); // sense if being touched
		bool status(); // to get the current 'touched' status, unlike sense() this only read a variable
		int held(); // for how long it has been touched
    int statusSum(); // cumulative sum of 'touched'
    int lastStatus(); // to know the previous 'touched' status
	private:
		int _pin; // the pin where the sensor is connected
    int _ledPin; // same but for the LED
    int _ledStatus; // 0 = off; 1 = on
		int _status; // either on or off
		int _startMs; // in loop timestamp for touch detected
		int _endMs; // same but for touch ended or no touch
    int _heldMs; // variable for storing how long sensor has been touched
    int _statusSum; // variable for storing the touch cumulative sum
    int _lastStatus; // variable for storing the previous 'touched' status
		Adafruit_MPR121 cap; // not sure if this should be here, to initialize the sensor
};

sensors::sensors(int pin, int ledPin)
  : trialLed(ledPin)
  // constructor are variables are detailed above
{
		_pin = pin;
    _ledPin = ledPin;
    _statusSum = 0;
		Adafruit_MPR121 cap;
    digitalPin trialLead(_ledPin);
}

void sensors::sense()
	// change the status when the animal steps on
{
		if (cap.touched() & (1 << _pin)){
			_status = 1;
			_startMs = millis();
      // only add to the cumulative summ
      // if sensor has changed status, that is +1 per touch
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

int sensors::held()
	// change the status when the animal touches
{
    if (_status == 1){
      _heldMs = _startMs - _endMs;
    }
    else{
      _heldMs = 0;
    }
    return _heldMs;
}

void sensors::begin()
// starts the sensor and the LED
{
    cap.begin(0x5A);
    trialLed.begin();
}

bool sensors::status()
// retrieves the status
{
		return _status;
}

int sensors::statusSum()
// retrieves the cumulative sum
{
    return _statusSum;
}

int sensors::lastStatus()
// retrieves the last status
{
    return _lastStatus;
}
