#ifndef DIGITALPIN_H
#define DIGITALPIN_H
class digitalPin{
	public:
		digitalPin(int pin);
		void begin();
		void on();
		void off();
		void blink();
		bool status();
	private:
		int _pin;
		int _status;
};

digitalPin::digitalPin(int pin)
{
		   _pin = pin;
}

void digitalPin::begin()
{
	pinMode(_pin, OUTPUT);
}

void digitalPin::on()
{
	digitalWrite(_pin, HIGH);
	_status = 1;
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