#include "Sensor.h"

class Fotodiodo: public Sensor{

	public:
		Fotodiodo(int pin1){
			pin=pin1;
		}
		int get_value(){
			value=analogRead(pin);
			return (value);
		}
};