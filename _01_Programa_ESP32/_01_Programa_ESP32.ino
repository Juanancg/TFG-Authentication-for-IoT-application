#include "Coordinator.h"


/* VARIABLES */
Coordinator coordinator;


void dmpDataReady() {
    mpuInterrupt = true;
}

void setup() {

  Serial.begin(115200);
  coordinator.init();

  pinMode(32, INPUT_PULLUP);
  attachInterrupt(32, dmpDataReady, RISING);
}

void loop() {

	coordinator.waitingMessage();
	coordinator.mpu_sensor.mpuloop();

  

}
