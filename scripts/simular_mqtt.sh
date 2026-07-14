#!/bin/bash
# Simula las publicaciones MQTT que hace el ESP32 (ver codigo-esp32/sketch_jun23a/mqtt.ino)
# Uso: ./simular_mqtt.sh [status|ambiente|deep_sleep] [temperatura|valor_deep_sleep] [humedad]
# Broker host configurable via variable de entorno: MQTT_HOST=192.168.0.10 ./simular_mqtt.sh ...

set -euo pipefail

ACCION="${1:-ambiente}"
HOST="${MQTT_HOST:-localhost}"
PORT=1883
DISPOSITIVO="Carlitos"

case "$ACCION" in
  status)
    mosquitto_pub -h "$HOST" -p "$PORT" \
      -t "sensores/status" \
      -m "{\"estado\":\"online\",\"dispositivo\":\"${DISPOSITIVO}\"}" \
      -r
    echo "Publicado status online en ${HOST}"
    ;;
  ambiente)
    TEMPERATURA="${2:-22.5}"
    HUMEDAD="${3:-58.3}"
    TS=$(date +%s)
    mosquitto_pub -h "$HOST" -p "$PORT" \
      -t "sensores/ambiente" \
      -m "{\"temperatura\":${TEMPERATURA},\"humedad\":${HUMEDAD},\"unidad_temp\":\"°C\",\"unidad_hum\":\"%\",\"timestamp\":${TS},\"dispositivo\":\"${DISPOSITIVO}\"}"
    echo "Publicado ambiente en ${HOST} (temperatura=${TEMPERATURA}, humedad=${HUMEDAD}, timestamp=${TS})"
    ;;
  deep_sleep)
    VALOR="${2:-5}"
    mosquitto_pub -h "$HOST" -p "$PORT" \
      -t "sensores/configuracion/deep_sleep" \
      -m "{\"valor\":${VALOR}}"
    echo "Publicado deep_sleep=${VALOR} en ${HOST}"
    ;;
  *)
    echo "Uso: $0 [status|ambiente|deep_sleep] [temperatura|valor_deep_sleep] [humedad]"
    echo "Broker host configurable via MQTT_HOST (default: localhost)"
    exit 1
    ;;
esac
