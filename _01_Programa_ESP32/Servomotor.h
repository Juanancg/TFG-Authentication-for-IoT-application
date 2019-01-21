#include "Final_de_carrera.h"
#include <ESP32Servo.h>

class Servomotor: public Sensor{

		
	public:
		Final_de_carrera sensor_apertura;
		Final_de_carrera sensor_cierre;
		Servo myservo;
				
		void set_pin(int x){
			pin = x;
			sensor_apertura.set_pin(5);
			sensor_cierre.set_pin(4);
		}

		void setup(){
  			myservo.setPeriodHertz(50);// Standard 50hz servo
  			myservo.attach(pin, 500, 2500);
		}

		/*void write(int value_to_move){
			value = value_to_move;
			myservo.write(value);
			//ledcWrite(0, value);
			Serial.println("He movido el servo");
		}*/
		
		int get_value(){
		
			return(value);
		}
		
		bool open(){
			Serial.println("Abriendo tapa...");
			int valor_fca = sensor_apertura.get_value();
			if(valor_fca == 0){
				value = 100; 
				/*for (int i = value; i > 40; i -= 1){
					if(sensor_apertura.get_value() == 1){
						Serial.println("Sensor apertura activado");
						break;
					}
					Serial.println(i);
					myservo.write(i);
					delay(100);
				}
				}*/
				while(valor_fca==0 || value == 40){

					valor_fca = sensor_apertura.get_value();
					Serial.print("LimitSwitch: ");
					Serial.println(valor_fca);
					Serial.println("----------------");
					Serial.print("Value: ");
					Serial.println(value);
					myservo.write(value);

					delay(100);  
					value -= 1;
				}	

			
			return true;
			}
		}
			
		bool close(){
			
			// int valor_fcc = sensor_cierre.get_value();
			// while(valor_fcc==0){
				// valor_fcc = sensor_cierre.get_value();
				
					// myservo.write(value);              
					// delay(30);  
					// value += 1;
			// }	
			
			
			for (int i=0; i <= 170; i += 1) { 
        		Serial.println(i);
				myservo.write(i);              
				delay(30);                       
			}
			return true;
		}
};
