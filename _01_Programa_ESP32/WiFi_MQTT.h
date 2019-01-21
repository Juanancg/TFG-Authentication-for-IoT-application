#include <NTPClient.h>
#include <WiFi.h>
#include "HmacSha256.h"
#include <PubSubClient.h>
#include <WiFiUdp.h>
#define MQTT_MAX_PACKET_SIZE 1024



class WiFi_MQTT {
	
  public:
	WiFiClient wifiClient;
	PubSubClient MQTTClient;  

	char mensaje_inicial[100];
	int flag_msg_recibido = 0;
	
	WiFiUDP ntpUDP;
	NTPClient timeClient;

    void init(const char* ssidd, const char* pswd, const char* mqtt_Server ,const int mqtt_Port ,const char* mqtt_Usr ,const char* mqtt_Pswd ){
		timeClient.setUDPClient(ntpUDP);
		MQTTClient.setClient(wifiClient);

		WiFi.begin(ssidd, pswd);

		while (WiFi.status() != WL_CONNECTED) {
			delay(500);
			Serial.println("Connecting to WiFi..");
		}

		Serial.println("Connected to the WiFi network");

		MQTTClient.setServer(mqtt_Server, mqtt_Port);


		while (!MQTTClient.connected()) {

			Serial.println("Connecting to MQTT...");

			if (MQTTClient.connect("ESP32Client", mqtt_Usr, mqtt_Pswd,"esp/LastWill",0,1,"0")) {

				Serial.println("connected"); 
				//client.publish("esp/test1", "ESP32 conectado !"); 
			} 
			else { 

				Serial.print("failed with state ");
				Serial.print(MQTTClient.state());
				delay(2000);
			}
		}
		if(MQTTClient.subscribe("esp/order")){
			MQTTClient.publish("esp/LastWill",  "1", true );	
			
		} else{
			MQTTClient.publish("esp/LastWill",  "0", true );	
		}
		MQTTClient.setCallback([this] (char* topic, byte* payload, unsigned int length) { this->callback(topic, payload, length); });
		timeClient.begin();
    }


    void callback(char* topic, byte* payload, unsigned int length) { 
    	// Clear mensaje_incial
		memset(mensaje_inicial, 0, strlen(mensaje_inicial));
		for (int i = 0; i < length; i++) {    
			mensaje_inicial[i]=payload[i];
		}
		Serial.print("Mensaje Recibido: ");
		Serial.println(mensaje_inicial);
		//client.publish("esp/test1", "Mensaje recibido!");
		flag_msg_recibido=1;
    }


    String get_time(){
    	timeClient.update();

    	char msgToSend[10];
		sprintf(msgToSend, "%02d:%02d:%02d", timeClient.getHours()+1, timeClient.getMinutes(), timeClient.getSeconds());
		delay(100);
		String str1(msgToSend);
		return /*static_cast<char*>(*/str1/*)*/;

    }
};
    
