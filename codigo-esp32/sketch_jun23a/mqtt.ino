#include "config.h"
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <Preferences.h>
#include <DHT.h>



DHT dht(PIN_DHT, DHT_TYPE);

extern unsigned int deep_sleep_time;
extern Preferences prefs;

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
  String mensaje = "";
  for (int i = 0; i < length; i++) {
    mensaje += (char)payload[i];
  }

  if(String(topic) == "sensores/configuracion/deep_sleep"){
    Serial.print("Se recibio el msg che");
    Serial.println();
    Serial.println(mensaje);
    JsonDocument doc;
    deserializeJson(doc, mensaje);
    unsigned int valor = doc["valor"];
    Serial.print("VALOR EN VALOR: ");
    Serial.println(valor);

    
    
    if(valor != deep_sleep_time){
      Serial.println("CAMBIANDO VALORES PARA DEEP_SLEEP");
      prefs.putUInt("deep_sleep_time", valor);
      deep_sleep_time = valor;
    }

  }
}

void publicar_ambiente(){
  dht.begin();
  float newTemp = dht.readTemperature();
  float newHum = dht.readHumidity();

  Serial.println("VALORES");
  Serial.print(newTemp);
  Serial.print(" ");
  Serial.println(newHum);


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



void connectMQTT(){
  while(!mqttClient.connected()){
    Serial.print("Conectando a BROKER MQTT");
    Serial.println(mqtt_server);

    bool connected;
    connected = mqttClient.connect(MQTT_CLIENT_ID, topico_status, 0, true, "offline");
    mqttClient.subscribe("sensores/configuracion/deep_sleep");

    if (connected) {
      StaticJsonDocument<200> doc;
      doc["estado"] = "online";
      doc["dispositivo"] = MQTT_CLIENT_ID;
      char jsonBuffer[256];
      serializeJson(doc, jsonBuffer);
      Serial.println("MQTT CONECTADO");

      mqttClient.publish(topico_status, jsonBuffer, true);
      
      Serial.println("CONFIGURACION MQTT");
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

