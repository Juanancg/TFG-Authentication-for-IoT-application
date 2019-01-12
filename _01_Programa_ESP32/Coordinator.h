#ifndef COORDINATOR_H
#define COORDINATOR_H

#include <ArduinoJson.h>
#include "Fotodiodo.h"
#include "HmacSha256.h"
#include "MPU_6050.h"
#include "Servomotor.h"
#include "HmacSha256.h"
#include "WiFi_MQTT.h"


class Coordinator {

	private:
		char *ping_msg;
    
	    char JSONmessageBuffer[150];
	    
	    /* WiFi */
	    const char* ssid = "TP-LINK_F3200A";
	    const char* password =  "43491896";
	    /* MQTT - Utilizando CloudMQTT */
	    const char* mqttServer = "m20.cloudmqtt.com";
	    const int   mqttPort = 12834;
	    const char* mqttUser = "rmqewpne";
	    const char* mqttPassword = "kFlJMJ_jC5pk";
    
	public:

		char *mensaje;
		char *mensaje_get;
		char *strRawMessage;
		char *key;

		WiFi_MQTT client;

		/* System Components */
		Servomotor servo; 
		MPU_6050 accelerometer;
		Fotodiodo fotodiodo;

		int sdaPin = 26;
		int sclPin = 25;
		float angulox, anguloy, anguloz;
		int fotodiodo_Value;

    	/*********************************************CONSTRUCTOR***************************************//**
    	*	\brief Constructor of Coordinator Class
    	*	\return
    	***************************************************************************************************/
    	/*Coordinator():servo(13), fotodiodo(36){

    	}*/

    	/*********************************************FUNCTION******************************************//**
    	*	\brief Function that initializes the coordinator class and objects 
    	*	\return
    	***************************************************************************************************/
	    void init(){
	    	/* Initialites MQTT Client & WiFi */
	      	client.init(ssid, password, mqttServer, mqttPort, mqttUser, mqttPassword);

	      	/* Initialites System components*/
	      	servo.set_pin(13);
	      	servo.setup();
	      	fotodiodo.set_pin(36);
	      	sdaPin = 26;
			sclPin = 25;
			accelerometer.mpu_init(sdaPin, sclPin);

			/* Initialites HMAC Key */
			key = "secretKey"; 
			Serial.println("Cooridnator initialized!");
	    }
    
    
    	/*********************************************FUNCTION******************************************//**
    	*	\brief Function that compares HMAC received from MQTT with the HMAC that should be
    	*	\return true if is authentic false if not
    	***************************************************************************************************/    
		bool bCheckAuth(char *mensaje_to_check) {

		    char *strHMACReceived;
		    char *strHMACReal;
			/* EXTRAEMOS LA FIRMA DIGITAL */
			strHMACReceived = get_digital_sig(mensaje_to_check);

			/* GENERAMOS LA FIRMA DIGITAL QUE DEBERÍA SER */
			strHMACReal  = strComputeHMAC(key, strGetMessageFromRaw(mensaje_to_check));

			/* COMPARAMOS AMBAS FIRMAS */
			return comparacion(strHMACReceived, strHMACReal);
		}
		
       	/*********************************************FUNCTION******************************************//**
    	*	\brief Function that gets the message from the HMAC+Message and desactivates the flag of msg 
    	*		   received
    	*	\return message 
    	***************************************************************************************************/ 
		char* strGetMessage(char* strhmacMessage){ // TODO - Posiblemente quitar
			// client.flag_msg_recibido = 0;
			return strGetMessageFromRaw(strhmacMessage);
		}

       	/*********************************************FUNCTION******************************************//**
    	*	\brief Function that get the values of the components of the system and composes a JSON msg
    	*	\return JSON message 
    	***************************************************************************************************/ 
		char * strGetValuesComposeJSON(){
		  
			StaticJsonBuffer<300> JSONbuffer;
			JsonObject& JSONStatus = JSONbuffer.createObject();
			JsonObject& JSONStatusContent = JSONbuffer.createObject();
			JsonObject& JSONLimitSwitch = JSONbuffer.createObject();
			JsonObject& JSONAngle = JSONbuffer.createObject();
			JsonObject& JSONAngles = JSONbuffer.createObject();

			/*<< Angle Information */
			JSONAngles["X_AXIS"] = accelerometer.get_value('x');
			JSONAngles["Y_AXIS"] = accelerometer.get_value('y');
			JSONAngles["Z_AXIS"] = accelerometer.get_value('z');
			/*<< Limit Switch Information */
			JSONLimitSwitch["LimitSwitch_C"] = servo.sensor_cierre.get_value();
			JSONLimitSwitch["LimitSwitch_O"] = servo.sensor_apertura.get_value();
			/*<< Photodiode Information */
			JSONStatusContent["PHOTODIODE"] = fotodiodo.get_value();
			/*<< Join all the information */
			JSONStatusContent["LimitSwitch"] = JSONLimitSwitch;
			JSONStatusContent["ANGLES"] = JSONAngles;
			JSONStatus["STATUS"] = JSONStatusContent;

			JSONStatus.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
			return(JSONmessageBuffer);
		}
    	
		void waitingMessage(){
			client.MQTTClient.loop();
			// When it receives a message 
			if(client.flag_msg_recibido){
				client.flag_msg_recibido = 0;
				if (bCheckAuth(client.mensaje_inicial)){
					switch(iMessageType(strGetMessageFromRaw(client.mensaje_inicial))){
						case 1:
           Serial.println("Switch de Ping");
						    sendPingMessage();
						    break;
					  	case 2:
						    // statements
						    break;
					  	default:
						    // statements
						    break;
					}

				} else{
					Serial.println("El mensaje no es autentico");
				}
			}

		}

		int iMessageType(char* message){
			if(strcmp(message,"PING")==0){
        Serial.println("Tipo de mensaje de Ping");
				return 1;
				/*mensaje = compute_HMAC(key,ping_msg);
				strcat(mensaje,ping_msg);
				client->publish("esp/responses", mensaje);
				Serial.println("El mensaje es PING");*/
			}else if(strcmp(message,"GET")==0){
				/*mensaje_get = get_values_to_json(servo, sensor, fotodiodo); // ********* TODO ESTO
				mensaje = compute_HMAC(key,mensaje_get);
				strcat(mensaje,mensaje_get);
				client->publish("esp/responses", mensaje);*/
				return 1;
			} else{
				return 0;
			}

		}

		void sendPingMessage(){
			client.MQTTClient.publish("esp/responses", strcat(strComputeHMAC(key,"PING"),"PING"));
			Serial.println("El mensaje es PING");
		}

};


#endif
