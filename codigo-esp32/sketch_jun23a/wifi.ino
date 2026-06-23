#include <WiFi.h>
#include "config.h"

const char* ssid = "SSID_WIFI";
const char* password = "PASS_WIFI";


#define VALUE 5

void setup_wifi(){
  Serial.print("Configurando WIFI");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  int intentos = 0;
  while(WiFi.status() != WL_CONNECTED && intentos < 30){
    delay(500);
    Serial.print(".");
    intentos++;
  }

  if(WiFi.status() == WL_CONNECTED) {
    Serial.println("SE PUDO CONECTAR AL WIFI :)");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("ERROR AL CONECTAR CON WIFI");
    delay(5000);
    ESP.restart();
  }
}