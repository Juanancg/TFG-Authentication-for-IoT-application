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
				if(value == 0){
					value = 100;
				}
				 

				while(valor_fca==0 ){

					if ( value < 40){
						break;
					}
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
			Serial.println("Cerrando tapa...");
			int valor_fcc = sensor_cierre.get_value();
			if(valor_fcc == 0){
				if(value == 0){
					value = 70;
				}
				 

				while(valor_fcc == 0 ){
					if ( value > 140){
						break;
					}
					valor_fcc = sensor_cierre.get_value();
					Serial.print("LimitSwitch: ");
					Serial.println(valor_fcc);
					Serial.println("----------------");
					Serial.print("Value: ");
					Serial.println(value);
					myservo.write(value);

					delay(100);  
					value += 1;
				}	

			
			return true;
			}
		}
};
