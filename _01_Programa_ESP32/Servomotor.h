#include "Final_de_carrera.h"
#include <Servo.h>

class Servomotor: public Sensor{

		
	public:
		Final_de_carrera sensor_apertura;
		Final_de_carrera sensor_cierre;
		Servo myservo;
		
		Servomotor(int servo_pin):sensor_apertura(15),sensor_cierre(12){
		
			pin = servo_pin;			
		}
		
		void setup(){
		
			myservo.attach(pin); 
		}
		
		void write(int value_to_move){
			
			value = value_to_move;
			myservo.write(value);
		}
		
		int get_value(){
		
			return(value);
		}
		
		bool open(){
			
			int valor_fca = sensor_apertura.get_value();
			while(valor_fca==0){
				valor_fca = sensor_apertura.get_value();
				
					myservo.write(value);              
					delay(30);  
					value -= 1;
			}	
			
			return true;
		}
		
		bool close(){
			
			// int valor_fcc = sensor_cierre.get_value();
			// while(valor_fcc==0){
				// valor_fcc = sensor_cierre.get_value();
				
					// myservo.write(value);              
					// delay(30);  
					// value += 1;
			// }	
			
			
			for (value; value <= 90; value += 1) { 

				myservo.write(value);              
				delay(30);                       
			}
			return true;
		}
};