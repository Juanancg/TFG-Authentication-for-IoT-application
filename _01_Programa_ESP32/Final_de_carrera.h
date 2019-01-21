#ifndef FINAL_DE_CARRERA_H
#define FINAL_DE_CARRERA_H
#include "Sensor.h"

class Final_de_carrera: public Sensor{

	public:
		
		/*********************************************FUNCTION******************************************//**
    	*	\brief Function that sets the pin of the Limit Switch and configures the specified pin to 
    	*			behave as an Input pullup
    	*	\return None
    	***************************************************************************************************/ 
		void set_pin(int x){
			pin = x;
			pinMode(pin, INPUT);
		}

		/*********************************************FUNCTION******************************************//**
    	*	\brief Function that reads the value of the Limit Switch and returns it
    	*	\return value
    	***************************************************************************************************/ 
		int get_value(){
			value = digitalRead(pin);
			return (value);
		}
};
#endif
