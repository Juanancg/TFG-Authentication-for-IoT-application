#ifndef SENSOR_H
#define SENSOR_H


class Sensor{

	protected:
	
		int value;
		int pin;
	
	public: 

    Sensor(){
      
    }
    
		void set_pin(int x){
			pin = x;
		}

		int get_value(){
			return value;
		}
		
		int get_pin(){
			
			return pin;
		}
};
#endif
