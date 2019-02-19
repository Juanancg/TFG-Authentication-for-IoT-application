#ifndef COORDINATOR_H
#define COORDINATOR_H

#include <ArduinoJson.h>
#include "Fotodiodo.h"
#include "HmacSha256.h"
#include "MPU_6050.h"
#include "Servomotor.h"
#include "WiFi_MQTT.h"

#include <string>

#define PHOTODIODE_MAX_VALUE 1000
#define X_AXIS_MAX_VALUE -10
#define SECRET_KEY "secretKey"

class Coordinator {

	private:
		
		char JSONmessageBuffer[250];
	    /* WiFi */
	    const char* ssid = "**************";
	    const char* password =  "**********";
	    /* MQTT */
	    const char* mqttServer = "************";
	    const int   mqttPort = *********;
	    const char* mqttUser = "************";
	    const char* mqttPassword = "************";
    	char *key;

		// Variables para los pines del MPU6050
		int sdaPin = 26;
		int sclPin = 25;
		
		/* System Components */
		HmacSha256 crypto;
		Fotodiodo fotodiodo;
		Servomotor servo; 
		MPU_6050 mpu_sensor;
		WiFi_MQTT client;

	public:


    	/*********************************************FUNCTION******************************************//**
    	*	\brief Function that initializes the coordinator class and objects 
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
			key = SECRET_KEY; 
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
    	

       	/*********************************************FUNCTION******************************************//**
    	*	\brief Function that checks if the incoming message has a valid timestamp
    	*	\param Message without digital signature
    	*	\return true if is valid, false if not
    	***************************************************************************************************/ 
		bool bIsOnTime(char* message){
			int secondsMsg = client.get_time_in_seconds();
			String strmsg(message);
			char buffer[100];
			int len = strmsg.length();

			if(len<99){
				strmsg.toCharArray(buffer, strmsg.length()+1);
				len = len - 8; // To get the position of the time
				int seconds = ((int(buffer[len])-48)*10 + (int(buffer[len+1])-48))*3600 +((int(buffer[len+3])-48)*10 + (int(buffer[len+4])-48))*60+((int(buffer[len+6])-48)*10 + (int(buffer[len+7])-48));
				if((secondsMsg - seconds)<60){
					return true;	
				} else{
					return false;
				}

			} else{
				return false;
			}

		}


       	/*********************************************FUNCTION******************************************//**
    	*	\brief Function that waits for an incoming message and coordinates its reception and response
    	***************************************************************************************************/ 
		void waitingMessage(){
			client.MQTTClient.loop();
			// When it receives a message 
			int apertura;
			int valor = 100;
			if(client.flag_msg_recibido){
				client.flag_msg_recibido = 0;
				if (crypto.bCheckAuth(key,client.mensaje_inicial)){
					if (bIsOnTime(crypto.strGetMessageFromRaw(client.mensaje_inicial))){

						switch(iMessageType(crypto.strGetOnlyOrder(client.mensaje_inicial))){
							// PING CASE
							case 1:
							    sendPingMessage();
							    break;

							// GET STATUS CASE
						  	case 2:
							    sendStatusMessage();
							    break;

					    	// OPEN CASE
						    case 3:
						    	if(bCheckStatus(true)){			    	
							    	if (servo.open()){
					    				sendOpennedMessage();
					    			} else{

					    			}
				    			} else{
			    					sendInvalidConditionsMessage();
				    			}
				    			break;

			    			// CLOSE CASE
					    	case 4:
						    	if(bCheckStatus(false)){			    	
							    	if (servo.close()){
					    				sendClosedMessage();
					    			} else{

					    			}
				    			} else{
			    					sendInvalidConditionsMessage();
				    			}
				    			break;

						  	default:
							    sendUnkownMessage();
							    break;
						}

					} else{
						Serial.println("Invalid Timestamp");
					}

				} else{
					Serial.println("El mensaje no es autentico");
				}
			}

		}


       	/*********************************************FUNCTION******************************************//**
    	*	\brief Function that check all values after moving the motor
    	*	\return true if is valid, false if not
    	***************************************************************************************************/
		bool bCheckStatus(bool openOrder){
			// Checkea los final de carrera
			Serial.println(servo.sensor_cierre.get_value());
			if(servo.sensor_apertura.get_value() != openOrder || servo.sensor_cierre.get_value() == openOrder){

				if(openOrder){
					// Si la orden es abrir, check a los sensores
					if (fotodiodo.get_value() > PHOTODIODE_MAX_VALUE){
						float *yawPitchRoll = mpu_sensor.getAngles();
						if(yawPitchRoll[2] < X_AXIS_MAX_VALUE){
							Serial.println(yawPitchRoll[2]);
							return true;

						} else{
							return false;
						}

					} else{
						return false;
					}

				} else {
					// Si la orden es cerrar, da igual el valor del resto de valores
					return true;
				}
			} else{
				return false;
			}
		}


       	/*********************************************FUNCTION******************************************//**
    	*	\brief Function that identifies the typo of message that is introduced as argument
    	*	\param Message without digital signature and timestamp
    	*	\return true if is valid, false if not
    	***************************************************************************************************/ 
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

		void sendInvalidConditionsMessage(){
			String timemsg = client.get_time();
			String messageUnknown = "INVALIDCONDITIONS";
			messageUnknown = messageUnknown + timemsg;
			String hmacAndMsg (crypto.strComputeHMAC(key, messageUnknown));
			hmacAndMsg = hmacAndMsg + messageUnknown;
			client.MQTTClient.publish("esp/responses",hmacAndMsg.c_str());
			Serial.println("Mensaje de INVALIDCONDITIONS enviado");
		}

};


#endif
