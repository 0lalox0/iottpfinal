#include "config.h"
#include <PubSubClient.h>

extern PubSubClient mqttClient;

void setup() {
  Serial.begin(115200);
  setup_wifi();

  mqtt_init();
  connectMQTT();
  mqttClient.loop();
  publicar_ambiente();
  Serial.println("ME DUERMO");
  ESP.deepSleep(DEFAULT_SLEEP_TIME * 10000);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  // if(!mqttClient.connected()){
    connectMQTT();
  // }
  // mqttClient.loop();

  // publicar_ambiente();
  // delay(10000);
}
          