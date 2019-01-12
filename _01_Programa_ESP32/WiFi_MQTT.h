#include <WiFi.h>
#include "HmacSha256.h"
#include <PubSubClient.h>



class WiFi_MQTT {

  public:
	WiFiClient wifiClient;
	PubSubClient MQTTClient;  

	char mensaje_inicial[100];
	int flag_msg_recibido = 0;


    void init(const char* ssidd, const char* pswd, const char* mqtt_Server ,const int mqtt_Port ,const char* mqtt_Usr ,const char* mqtt_Pswd ){

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

			if (MQTTClient.connect("ESP32Client", mqtt_Usr, mqtt_Pswd )) {

				Serial.println("connected"); 
				//client.publish("esp/test1", "ESP32 conectado !"); 
			} 
			else { 

				Serial.print("failed with state ");
				Serial.print(MQTTClient.state());
				delay(2000);
			}
		}
		MQTTClient.subscribe("esp/order");
		MQTTClient.setCallback([this] (char* topic, byte* payload, unsigned int length) { this->callback(topic, payload, length); });

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
};
    
