#include "Coordinator.h"

/* VARIABLES */
Coordinator coordinator;

void setup() {
  Serial.begin(115200);

  // client.setCallback(callback);

  coordinator.init();
  //coordinator.servo.write(10);
  // 

  // sda, scl
  // sensor.mpu_calibrate();
  // Serial.println(coordinador.check_auth("HolaMensaje"));
  //Serial.println(coordinator.get_values_to_json());
  
}

void loop() {
  //coordinator.cliente.MQTTClient.loop(); 
  //Serial.println(coordinator.get_values_to_json());
  //Serial.println();
  coordinator.waitingMessage();
  delay(1000);
}
