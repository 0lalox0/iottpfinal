#include "config.h"
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <WiFi.h>

const char *mqtt_server = MQTT_IP_DEFAULT;
const int mqtt_port = MQTT_PORT_DEFAULT;

const char *topico_configuraciones = "sensores/configuraciones";
const char *topico_ambiente = "sensores/ambiente";
const char *topico_status = "sensores/status";


WiFiClient espclient;
PubSubClient mqttClient(espclient);

void mqttCallBack(char *topic, byte *payload, unsigned int length){
  Serial.print("Mensaje recibido en [");
  Serial.print(topic);
  Serial.print("]: ");

  for(int i = 0; i < length; i++){
    Serial.print((char) payload[i]);
  }
  Serial.println();
}

void publicar_ambiente(){
  float newTemp = 5;
  float newHum = 5;

  if (isnan(newTemp) || isnan(newHum)){
    Serial.println("Error leyendo sensor");
    mqttClient.publish(topico_status, "error_lectura");
    return;
  }

  float temperature = newTemp;
  float humidity = newHum;

  char tempStr[8];
  char humStr[8];
  dtostrf(temperature, 4, 1, tempStr);
  dtostrf(humidity, 4, 1, humStr);

  StaticJsonDocument<200> doc;
  doc["temperatura"] = round(temperature * 10) / 10.0;
  doc["humedad"] = round(humidity * 10) / 10.0;
  doc["unidad_temp"] = "°C";
  doc["unidad_hum"] = "%";
  doc["timestamp"] = millis() / 1000;
  doc["dispositivo"] = MQTT_CLIENT_ID;

  char jsonBuffer[256];
  serializeJson(doc, jsonBuffer);

  bool jsonPublished = mqttClient.publish(topico_ambiente, jsonBuffer);

  if(jsonPublished) {
    Serial.println("Datos publicados correctamente");
  }
  else {
    Serial.println("Error al publicar datos");
  }
}

void publicar_seguimiento(){

}

void connectMQTT(){
  while(!mqttClient.connected()){
    Serial.print("Conectando a BROKER MQTT");
    Serial.println(mqtt_server);

    bool connected;
    connected = mqttClient.connect(MQTT_CLIENT_ID, topico_status, 0, true, "offline");


    if (connected) {
      StaticJsonDocument<200> doc;
      doc["estado"] = "online";
      doc["dispositivo"] = MQTT_CLIENT_ID;
      char jsonBuffer[256];
      serializeJson(doc, jsonBuffer);
      Serial.println("MQTT CONECTADO");

      mqttClient.publish(topico_status, jsonBuffer, true);
      
      Serial.println("CONFIGURAICON MQTT");
    }
  }
}

void mqtt_init(){
  unsigned long lastReading = 0;
  const unsigned long readingInterval = 10000;

  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(mqttCallBack);
  connectMQTT();

  
}

