#include "config.h"
#include <PubSubClient.h>
#include <WiFi.h>
#include <Preferences.h>

Preferences prefs;
extern PubSubClient mqttClient;
extern WiFiClient espclient;


unsigned int deep_sleep_time;

void setup() {
  Serial.begin(115200);
  prefs.begin("nv-data", false);
  deep_sleep_time = prefs.getUInt("deep_sleep_time", DEFAULT_DEEP_SLEEP_TIME);
  setup_wifi();

  mqtt_init();
  connectMQTT();
  publicar_ambiente();
  
 for(int i = 0; i < 10; i++) {
    mqttClient.loop();
    delay(100);
  }
  

  Serial.println("ME DUERMO");
  ESP.deepSleep(deep_sleep_time * 60 * 1000000);
  
}

void loop() {

  // if(!mqttClient.connected()){
    //connectMQTT();
  // }
  // mqttClient.loop();

  // publicar_ambiente();
  // delay(10000);
}
          