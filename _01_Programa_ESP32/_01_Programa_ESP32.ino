#include "Coordinator.h"

/* VARIABLES */
Coordinator coordinator;

void setup() {
  Serial.begin(115200);
  coordinator.init();
  //coordinator.servo.write(10);
  // 

  // sda, scl
  // sensor.mpu_calibrate();
  // Serial.println(coordinador.check_auth("HolaMensaje"));
  //Serial.println(coordinator.get_values_to_json());
  
}

void loop() {

 coordinator.waitingMessage();

   
  delay(500);
  

}
