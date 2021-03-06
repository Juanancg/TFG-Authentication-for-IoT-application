#include "Coordinator.h"

/* VARIABLES */
Coordinator coordinator;


void dmpDataReady() {
    mpuInterrupt = true;
}
int num = 0;
void setup() {

  Serial.begin(115200);
  coordinator.init();

  pinMode(32, INPUT_PULLUP);
  attachInterrupt(32, dmpDataReady, RISING);
}

void loop() {
	// Bucle que espera a un mensaje y va actualizando el valor del MPU6050
	coordinator.waitingMessage();
	coordinator.mpu_sensor.mpuloop();
}
