

#include <Wire.h>
#include <Adafruit_PN532.h>
#include "DHT.h"
#include <WiFi.h>
#include <PubSubClient.h>

/* DHT11 CONFIGURATION */ 
#define DHTPIN 8
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

/* SENSOR PINS */ 
const int uvSensorPin = 3;
const int pirSensorPin = 12;
const int Buzzer = 7;
const int Relay = 37;

/*  PN532 I2C */ 
#define SDA_PIN 36
#define SCL_PIN 35
Adafruit_PN532 nfc(SDA_PIN, SCL_PIN);

/* WIFI AND THINGSBOARD */   
const char* ssid = "Sonin KH";
const char* password = "77771112";
const char* mqtt_server = "demo.thingsboard.io";
const int mqtt_port = 1883;
const char* access_token = "b2nKAl43lxeWbu2OWuD4";
const char* telemetry_topic = "v1/devices/me/telemetry";

WiFiClient espClient;
PubSubClient client(espClient);

/* SENSOR DATA */   
float humidity = NAN;
float temperature = NAN;
int uvRawValue = 0;
float uvVoltage = 0.0;
float uvIntensity = 0.0;
int pirValue = LOW;
String pirStatus = "No Motion";
String nfcMessage = "No Scan Card";

/* NFC FLAG */  
int nfcFlag = 0;

/* TIMING */   
const unsigned long publishInterval = 500;

SemaphoreHandle_t serialMutex;

void setup_wifi() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("\nWiFi connected. IP: " + WiFi.localIP().toString());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client", access_token, NULL)) {
      Serial.println("Connected to ThingsBoard");
    } else {
      Serial.print("Failed, rc=");
      Serial.println(client.state());
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  dht.begin();

  pinMode(uvSensorPin, INPUT);
  analogReadResolution(12);
  pinMode(pirSensorPin, INPUT);
  pinMode(Buzzer, OUTPUT);
  pinMode(Relay, OUTPUT);
  digitalWrite(Buzzer, LOW);
  digitalWrite(Relay, LOW);

  Wire.begin(SDA_PIN, SCL_PIN);
  nfc.begin();
  nfc.SAMConfig();

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  serialMutex = xSemaphoreCreateMutex();

  xTaskCreatePinnedToCore(taskPrintSensors, "PrintSensors", 4096, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(taskReadNFC, "ReadNFC", 4096, NULL, 1, NULL, 1);
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();
}

/* TASK: PRINT SENSORS */ 
void taskPrintSensors(void *pvParameters) {
  (void) pvParameters;
  char payload[300];

  while (true) {
    humidity = dht.readHumidity();
    temperature = dht.readTemperature();
    pirValue = digitalRead(pirSensorPin);

    /* SET BUZZER AND STATUS */ 
    if (pirValue == HIGH) {
      digitalWrite(Buzzer, LOW);
      pirStatus = "No Motion";
    } else {
      digitalWrite(Buzzer, HIGH);
      pirStatus = "Motion Detectedn";
    }

    uvRawValue = analogRead(uvSensorPin);
    uvVoltage = (uvRawValue / 4095.0) * 3.3;
    uvIntensity = (uvVoltage / 2.5) * 1000.0;

    /* NFC STATUS UPDATE */ 
    if (nfcFlag == 1) {
      nfcMessage = "No Scan Card";
    } else if (nfcFlag == 2) {
      nfcMessage = "Scan Card";
      nfcFlag = 3;
    }

    /* JSON PAYLOAD */ 
    snprintf(payload, sizeof(payload),
             "{\"humidity\":%.2f,\"temperature\":%.2f,\"uv_raw\":%d,"
             "\"uv_voltage\":%.2f,\"uv_intensity\":%.1f,"
             "\"pir_status\":\"%s\",\"nfc_status\":\"%s\"}",
             isnan(humidity) ? 0.0 : humidity,
             isnan(temperature) ? 0.0 : temperature,
             uvRawValue, uvVoltage, uvIntensity,
             pirStatus.c_str(), nfcMessage.c_str());

    if (serialMutex) {
      xSemaphoreTake(serialMutex, portMAX_DELAY);
      Serial.printf("Publishing payload: %s\n", payload);
      Serial.printf("Humidity: %.2f%% | Temperature: %.2f°C | Raw: %d | Voltage: %.2f V | UV Intensity: %.1f µW/cm² | %s | %s\n",
                    isnan(humidity) ? 0.0 : humidity,
                    isnan(temperature) ? 0.0 : temperature,
                    uvRawValue, uvVoltage, uvIntensity,
                    pirStatus.c_str(), nfcMessage.c_str());
      xSemaphoreGive(serialMutex);
    }

    client.publish(telemetry_topic, payload);

    if (nfcFlag == 3) nfcFlag = 0;

    vTaskDelay(pdMS_TO_TICKS(publishInterval));
  }
}

/* TASK: READ NFC */   
void taskReadNFC(void *pvParameters) {
  (void) pvParameters;
  uint8_t uid[7];
  uint8_t uidLength;

  while (true) {
    if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength)) {
      nfcFlag++;
      digitalWrite(Relay, (nfcFlag == 1) ? HIGH : LOW);
      if (nfcFlag >= 2) nfcFlag = 2;

      if (serialMutex) {
        xSemaphoreTake(serialMutex, portMAX_DELAY);
        Serial.print("NFC Tag Detected! UID: ");
        for (uint8_t i = 0; i < uidLength; i++) {
          Serial.print(uid[i], HEX);
          if (i < uidLength - 1) Serial.print(":");
        }
        Serial.println();
        xSemaphoreGive(serialMutex);
      }

      vTaskDelay(pdMS_TO_TICKS(1000));
    }

    vTaskDelay(pdMS_TO_TICKS(200));
  }
}
