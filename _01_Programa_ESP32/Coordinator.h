#ifndef COORDINATOR_H
#define COORDINATOR_H

#include <ArduinoJson.h>
#include "Fotodiodo.h"
#include "HmacSha256.h"
#include "MPU_6050.h"
#include "Servomotor.h"
#include "WiFi_MQTT.h"

#include <string>


class Coordinator {

	private:
		
		char JSONmessageBuffer[250];
	    /* WiFi */
	    const char* ssid = "TP-LINK_F3200A";
	    const char* password =  "43491896";
	    /* MQTT */
	    const char* mqttServer = "m20.cloudmqtt.com";
	    const int   mqttPort = 12834;
	    const char* mqttUser = "rmqewpne";
	    const char* mqttPassword = "kFlJMJ_jC5pk";
    	char *key;

		WiFi_MQTT client;
		HmacSha256 crypto;
		
		/* System Components */
		
		Fotodiodo fotodiodo;
		Servomotor servo; 

		int sdaPin = 26;
		int sclPin = 25;

	public:
		MPU_6050 mpu_sensor;
    	/*********************************************FUNCTION******************************************//**
    	*	\brief Function that initializes the coordinator class and objects 
    	*	\return
    	***************************************************************************************************/
	    void init(){
	    	/* Initialites MQTT Client & WiFi */
	      	client.init(ssid, password, mqttServer, mqttPort, mqttUser, mqttPassword);

	      	/* Initialites System components*/
	      	servo.set_pin(18);
	      	servo.setup();
	      	mpu_sensor.mpuSetup();
	      	fotodiodo.set_pin(36);

			/* Initialites HMAC Key */
			key = "secretKey"; 
			Serial.println("Cooridnator initialized!");
	    }
    
    
    	/*********************************************FUNCTION******************************************//**
    	*	\brief Function that compares HMAC received from MQTT with the HMAC that should be
    	*	\return true if is authentic false if not
    	***************************************************************************************************/    
		bool bCheckAuth(char *mensaje_to_check) {
			// If the message lenght is less than 64, the message doesnt have the HMAC
	      	if(strlen(mensaje_to_check) > 64){

				char* strHMACReceived;
				char* strHMACReal;
				/* EXTRAEMOS LA FIRMA DIGITAL */
				strHMACReceived = crypto.get_digital_sig(mensaje_to_check);
				/* GENERAMOS LA FIRMA DIGITAL QUE DEBERÍA SER */
				strHMACReal = crypto.strComputeHMAC(key, crypto.strGetMessageFromRaw(mensaje_to_check));
				/* COMPARAMOS AMBAS FIRMAS */
				return crypto.comparacion(strHMACReceived, strHMACReal);

			} else{
				return false;
			}
		}
		
       	/*********************************************FUNCTION******************************************//**
    	*	\brief Function that get the values of the components of the system and composes a JSON msg
    	*	\return JSON message 
    	***************************************************************************************************/ 
		char * strGetValuesComposeJSON(){
  		    
  		    memset(JSONmessageBuffer, 0, strlen(JSONmessageBuffer));
			StaticJsonBuffer<300> JSONbuffer;
			JsonObject& JSONStatus = JSONbuffer.createObject();
			JsonObject& JSONStatusContent = JSONbuffer.createObject();
			JsonObject& JSONLimitSwitch = JSONbuffer.createObject();
			JsonObject& JSONAngle = JSONbuffer.createObject();
			JsonObject& JSONAngles = JSONbuffer.createObject();

			float *yawPitchRoll = mpu_sensor.getyaw(mpu_sensor.fifoBuffer);

			/*<< Angle Information */
			JSONAngles["X_AXIS"] = yawPitchRoll[2]; // Roll
			JSONAngles["Y_AXIS"] = yawPitchRoll[1]; // Pitch
			JSONAngles["Z_AXIS"] = yawPitchRoll[0]; // Yaw

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
			int apertura;
			int valor = 100;
			if(client.flag_msg_recibido){
				client.flag_msg_recibido = 0;
				if (bCheckAuth(client.mensaje_inicial)){
					switch(iMessageType(crypto.strGetMessageFromRaw(client.mensaje_inicial))){
						case 1:
						// PING CASE
						    sendPingMessage();
						    break;
					  	case 2:
					  	// GET STATUS CASE
						    sendStatusMessage();
						    break;
					    case 3:			    	
					    	if (servo.open()){
			    				sendOpennedMessage();
			    			} else{

			    			}
			    			break;

				    	case 4:
				    		if(servo.close()){
				    			sendClosedMessage();
				    		}
				    		break;
					  	default:
						    sendUnkownMessage();
						    break;
					}

				} else{

					Serial.println("El mensaje no es autentico");
				}
			}

		}

		int iMessageType(char* message){
			if(strcmp(message,"PING")==0){
        		Serial.println("Tipo de mensaje recibido de Ping");
				return 1;

			}else if(strcmp(message,"GETSTATUS")==0){
				Serial.println("Tipo de mensaje recibido de Get Status");
				return 2;

			}else if(strcmp(message,"OPEN")==0){
				Serial.println("Tipo de mensaje recibido de OPEN");
				return 3;
			}else if(strcmp(message,"CLOSE")==0){
				Serial.println("Tipo de mensaje recibido de CLOSE");
				return 4;
			} else{
				return 0;
			}

		}

		void sendPingMessage(){
			client.MQTTClient.publish("esp/responses", strcat(crypto.strComputeHMAC(key,"PING"),"PING"));
			Serial.println("Mensaje de PING enviado");
		}

		void sendStatusMessage(){
			char * messageJSON = strGetValuesComposeJSON();
			client.MQTTClient.publish("esp/responses", strcat(crypto.strComputeHMAC(key, messageJSON), messageJSON));
			Serial.println("Mensaje de STATUS enviado");
		}

		void sendOpennedMessage(){
			client.MQTTClient.publish("esp/responses", strcat(crypto.strComputeHMAC(key, "OPEN"), "OPEN"));
			Serial.println("Mensaje de OPEN enviado");
		}

		void sendClosedMessage(){
			client.MQTTClient.publish("esp/responses", strcat(crypto.strComputeHMAC(key, "CLOSED"), "CLOSED"));
			Serial.println("Mensaje de OPEN enviado");
		}

		void sendUnkownMessage(){
			client.MQTTClient.publish("esp/responses", strcat(crypto.strComputeHMAC(key, "UNKNOWN"), "UNKNOWN"));
			Serial.println("Mensaje de UNKNOWN enviado");
		}

};


#endif
