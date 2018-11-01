#ifndef FINAL_DE_CARRERA_H
#define FINAL_DE_CARRERA_H
#include "Sensor.h"

class Final_de_carrera: public Sensor{

	public:
		Final_de_carrera(int pin1){
			pin=pin1;
			pinMode(pin,INPUT_PULLUP);
		}
		int get_value(){
			value=digitalRead(pin);
			return (value);
		}
};
#endif
