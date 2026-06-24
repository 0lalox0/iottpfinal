#include "config.h"
#include <PubSubClient.h>

extern PubSubClient mqttClient;

void setup() {
  Serial.begin(115200);
  setup_wifi();
  mqtt_init();
}

void loop() {
  // put your main code here, to run repeatedly:
  if(!mqttClient.connected()){
    connectMQTT();
  }
  mqttClient.loop();

  publicar_ambiente();
  delay(10000);
}
          