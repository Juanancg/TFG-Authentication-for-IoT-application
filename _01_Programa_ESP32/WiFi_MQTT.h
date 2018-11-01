#include <WiFi.h>
#include "HmacSha256.h"
#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient client;		
char mensaje_inicial[100];

int flag_msg_recibido = 0;



void init(const char* ssidd, const char* pswd, const char* mqtt_Server ,const int mqtt_Port ,const char* mqtt_Usr ,const char* mqtt_Pswd ){
	
	client.setClient(espClient);
	
	WiFi.begin(ssidd, pswd);

	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.println("Connecting to WiFi..");
	}

	Serial.println("Connected to the WiFi network");

	client.setServer(mqtt_Server, mqtt_Port);
	

	while (!client.connected()) {

		Serial.println("Connecting to MQTT...");

		if (client.connect("ESP32Client", mqtt_Usr, mqtt_Pswd )) {

			Serial.println("connected"); 
			//client.publish("esp/test1", "ESP32 conectado !"); 
		} 
		else { 

			Serial.print("failed with state ");
			Serial.print(client.state());
			delay(2000);
		}
	}
	client.subscribe("esp/order");
}

void callback(char* topic, byte* payload, unsigned int length) { 
   
      //auth=0;
      for (int i = 0; i < length; i++) {    
        mensaje_inicial[i]=payload[i];
        //Serial.print((char)payload[i]);
      }
      
      //client.publish("esp/test1", "Mensaje recibido!");
      flag_msg_recibido=1;

}
		

		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
