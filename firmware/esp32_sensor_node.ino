/*
  Terraquipu - Nodo IoT de Calibración Edáfica In-Situ
  Hardware: ESP32 DevKit v1 + Sensor Capacitivo de Humedad + Dallas DS18B20
  Sprint de 48 Horas - NASA Space Apps Challenge
*/

#include <WiFi.h>
#include <HTTPClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ArduinoJson.h>

// Configuración de red
const char* ssid = "TU_WIFI_SSID";
const char* password = "TU_WIFI_PASSWORD";
const char* serverUrl = "http://192.168.1.100:8000/api/v1/telemetry";

// Pines de sensores
#define SOIL_MOISTURE_PIN 34  // ADC1_CH6
#define ONE_WIRE_BUS 4        // DS18B20 1-Wire

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature tempSensors(&oneWire);

// Calibración de sensor capacitivo (valores en seco y en agua)
const int DRY_ADC = 3200;
const int WET_ADC = 1450;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("[*] Iniciando nodo IoT Terraquipu...");

  tempSensors.begin();
  analogSetAttenuation(ADC_11db);

  WiFi.begin(ssid, password);
  Serial.print("[*] Conectando a WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n[+] WiFi Conectado. IP: " + WiFi.localIP().toString());
}

void loop() {
  // 1. Lectura de humedad de suelo capacitiva
  int rawAdc = analogRead(SOIL_MOISTURE_PIN);
  float vwc = map(rawAdc, DRY_ADC, WET_ADC, 0, 100);
  vwc = constrain(vwc, 0.0, 100.0);

  // 2. Lectura de temperatura edáfica
  tempSensors.requestTemperatures();
  float soilTemp = tempSensors.getTempCByIndex(0);

  Serial.printf("[SENSOR] Humedad VWC: %.1f%% | Temp Suelo: %.1f C\n", vwc, soilTemp);

  // 3. Enviar telemetría si hay conexión
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");

    StaticJsonDocument<512> doc;
    doc["device_id"] = "terraquipu-node-cusco-01";
    doc["firmware_version"] = "v1.2.0-esp32";
    doc["timestamp"] = "2026-11-10T10:30:00Z";

    JsonObject location = doc.createNestedObject("location");
    location["latitude"] = -13.5321;
    location["longitude"] = -71.9675;
    location["elevation_masl"] = 3410.5;

    JsonObject readings = doc.createNestedObject("readings");
    readings["soil_moisture_vwc_percent"] = vwc;
    readings["soil_temperature_celsius"] = soilTemp;
    readings["air_temperature_celsius"] = 16.5;
    readings["air_humidity_percent"] = 42.0;
    readings["battery_level_percent"] = 90;

    String jsonPayload;
    serializeJson(doc, jsonPayload);

    int httpCode = http.POST(jsonPayload);
    Serial.printf("[HTTP] Respuesta del Backend: %d\n", httpCode);
    http.end();
  }

  // Esperar 15 segundos entre lecturas (en producción usar Deep Sleep)
  delay(15000);
}

