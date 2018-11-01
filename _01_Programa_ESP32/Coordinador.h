#ifndef COORDINADOR_H
#define COORDINADOR_H

#include "Fotodiodo.h"
#include "HmacSha256.h"
#include "MPU_6050.h"
#include "Servomotor.h"

#include "HmacSha256.h"
#include "WiFi_MQTT.h"
#include <ArduinoJson.h> 

class Coordinador {

	private:
		char *ping_msg = "PING";
    
    char JSONmessageBuffer[150];
    
	public:

		char *mensaje;
    char *mensaje_get;
		bool auth = 0;
		char *mensaje_recibido;
		char *key = "secretKey";
		
		bool check_auth(char *mensaje_to_check) {

		  char *hmac_recibido;
		  char *hmac_generado;
	  
		  /*  EXTRAEMOS EL MENSAJE RECIBIDO */
		  mensaje_recibido = get_msg(mensaje_to_check);

		  /* EXTRAEMOS LA FIRMA DIGITAL */
		  hmac_recibido = get_digital_sig(mensaje_to_check);

		  /* GENERAMOS LA FIRMA DIGITAL QUE DEBERÍA SER */
		  hmac_generado  = compute_HMAC(key, mensaje_recibido);

		  /* COMPARAMOS AMBAS FIRMAS */
		  auth = comparacion(hmac_recibido, hmac_generado);
		  flag_msg_recibido = 0;
		  return auth;

		}
   
    char * get_values_to_json(Servomotor *servo, MPU_6050 *acel_giros, Fotodiodo *fotodiodo){
      
      StaticJsonBuffer<300> JSONbuffer;
      JsonObject& JSONencoder = JSONbuffer.createObject();
      JSONencoder["PHOTODIODE"] = fotodiodo->get_value();
      JSONencoder["FDC_O"] = servo->sensor_apertura.get_value();
      JSONencoder["FDC_C"] = servo->sensor_cierre.get_value();
      JSONencoder["EJE_X"] = acel_giros->get_value('x');
      JSONencoder["EJE_Y"] = acel_giros->get_value('y');
      JSONencoder["EJE_Z"] = acel_giros->get_value('z');
      
      JSONencoder.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
      return(JSONmessageBuffer);
    }
    
		void msg_type(PubSubClient *cliente,Servomotor *servo, MPU_6050 *acel_giros, Fotodiodo *fotodiodo){
			
			if(strcmp(mensaje_recibido,"PING")==0){
				
				mensaje = compute_HMAC(key,ping_msg);
				strcat(mensaje,ping_msg);
				cliente->publish("esp/responses", mensaje);
				Serial.println("El mensaje es PING");
			}
      if(strcmp(mensaje_recibido,"GET")==0){
        mensaje_get = get_values_to_json(servo, sensor, fotodiodo); // ********* TODO ESTO
        mensaje = compute_HMAC(key,mensaje_get);
        strcat(mensaje,mensaje_get);
        cliente->publish("esp/responses", mensaje);
               
		  }
		}

};


#endif
