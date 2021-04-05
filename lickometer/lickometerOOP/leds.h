#ifndef DIGITALPIN_H
#define DIGITALPIN_H
// Class to provide basic functionality to LEDS
// 1. Turn led on
// 2. Turn led off
// 3. Blink led
// 4. Know the status of the led (on or off)
class digitalPin{
	public:
		digitalPin(int pin);
		void begin();
		void on();
		void off();
		void blink();
    int ledPower();
		bool status();
	private:
    int _ledPower;
		int _pin;
		int _status;
};

digitalPin::digitalPin(int pin)
{
		   _pin = pin;
      _ledPower = 10;
}

void digitalPin::begin()
{
	pinMode(_pin, OUTPUT);
}

void digitalPin::on()
{
	analogWrite(_pin, _ledPower);
	_status = 1;
}

int digitalPin::ledPower(){
  return _ledPower;
}

void digitalPin::off()
{
	digitalWrite(_pin, LOW);
	_status = 1;
}

void digitalPin::blink()
{
	for (int i = 0; i < 3; i++){
		this->digitalPin::on();
		delay(250);
		this->digitalPin::off();
		delay(250);
	}
}

bool digitalPin::status()
{
	return _status;
}
#endif 
