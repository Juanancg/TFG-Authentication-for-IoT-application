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

		
		HmacSha256 crypto;
		
		/* System Components */
		
		Fotodiodo fotodiodo;
		Servomotor servo; 

		int sdaPin = 26;
		int sclPin = 25;

	public:
		MPU_6050 mpu_sensor;
		WiFi_MQTT client;
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
    	*	\brief Function that get the values of the components of the system and composes a JSON msg
    	*	\return JSON message 
    	***************************************************************************************************/ 
		String strGetValuesComposeJSON(){
  		    
  		    memset(JSONmessageBuffer, 0, strlen(JSONmessageBuffer));
			StaticJsonBuffer<300> JSONbuffer;
			JsonObject& JSONStatus = JSONbuffer.createObject();
			JsonObject& JSONStatusContent = JSONbuffer.createObject();
			JsonObject& JSONLimitSwitch = JSONbuffer.createObject();
			JsonObject& JSONAngle = JSONbuffer.createObject();
			JsonObject& JSONAngles = JSONbuffer.createObject();

			float *yawPitchRoll = mpu_sensor.getAngles();

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
			String strJSON(JSONmessageBuffer);
			return(strJSON);
		}
    	
		void waitingMessage(){
			client.MQTTClient.loop();
			// When it receives a message 
			int apertura;
			int valor = 100;
			if(client.flag_msg_recibido){
				client.flag_msg_recibido = 0;
				if (crypto.bCheckAuth(key,client.mensaje_inicial)){
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
			String timemsg = client.get_time();
			String messagePing = "PING";
			messagePing = messagePing + timemsg;
			String hmacAndMsg (crypto.strComputeHMAC(key, messagePing));
			hmacAndMsg = hmacAndMsg + messagePing;
			client.MQTTClient.publish("esp/responses",hmacAndMsg.c_str());
			Serial.println("Mensaje de PING enviado");
		}

		void sendStatusMessage(){
			String messageJSON = strGetValuesComposeJSON();
			String timemsg = client.get_time();
			messageJSON = messageJSON + timemsg;
			String hmacAndMsg (crypto.strComputeHMAC(key, messageJSON));
			hmacAndMsg = hmacAndMsg + messageJSON;
			client.MQTTClient.publish("esp/responses",hmacAndMsg.c_str());
			Serial.println("Mensaje de STATUS enviado");
		}

		void sendOpennedMessage(){
			String timemsg = client.get_time();
			String messageOpen = "OPEN";
			messageOpen = messageOpen + timemsg;
			String hmacAndMsg (crypto.strComputeHMAC(key, messageOpen));
			hmacAndMsg = hmacAndMsg + messageOpen;
			client.MQTTClient.publish("esp/responses",hmacAndMsg.c_str());
			Serial.println("Mensaje de OPEN enviado");
		}

		void sendClosedMessage(){
			String timemsg = client.get_time();
			String messageClose = "CLOSE";
			messageClose = messageClose + timemsg;
			String hmacAndMsg (crypto.strComputeHMAC(key, messageClose));
			hmacAndMsg = hmacAndMsg + messageClose;
			client.MQTTClient.publish("esp/responses",hmacAndMsg.c_str());
			Serial.println("Mensaje de CLOSE enviado");
		}

		void sendUnkownMessage(){
			String timemsg = client.get_time();
			String messageUnknown = "UNKNOWN";
			messageUnknown = messageUnknown + timemsg;
			String hmacAndMsg (crypto.strComputeHMAC(key, messageUnknown));
			hmacAndMsg = hmacAndMsg + messageUnknown;
			client.MQTTClient.publish("esp/responses",hmacAndMsg.c_str());
			Serial.println("Mensaje de UNKNOWN enviado");
		}

};


#endif
