//#include "Final_de_carrera.h"
//#include "Fotodiodo.h"
//#include "HmacSha256.h"
//#include "MPU_6050.h"
//#include "Servomotor.h"
//#include "WiFI_MQTT.h"
#include "Coordinador.h"

/* VARIABLES */
Coordinador coordinador;

Servomotor servo(13);

int sdaPin = 26;
int sclPin = 25;
float angulox, anguloy, anguloz;
MPU_6050 sensor;

Fotodiodo fotodiodo1(36);
int fotodiodo_Value;

bool authenticacion = 0;
/* WiFi */
const char* ssid = "************";
const char* password =  "***********";

/* MQTT - Utilizando CloudMQTT */
const char* mqttServer = "*******";
const int mqttPort = *******;
const char* mqttUser = "*********";
const char* mqttPassword = "*********";

void setup() {
  Serial.begin(115200);

  init(ssid, password, mqttServer, mqttPort, mqttUser, mqttPassword);
  client.setCallback(callback);

  servo.setup();
  servo.write(10);

  sensor.mpu_init(sdaPin, sclPin);// sda, scl
  sensor.mpu_calibrate();

  coordinador.get_values_to_json(&servo, &sensor, &fotodiodo1);
}

void loop() {
  client.loop();
  
  if (flag_msg_recibido == 1) {
    authenticacion = coordinador.check_auth(mensaje_inicial);
    if (authenticacion == 1) {
      //Serial.println(coordinador.mensaje_recibido);
      coordinador.msg_type(&client,&servo, &sensor, &fotodiodo); //******* ESTO ES NNUEVI

    }
  }
  delay(1000);
}
