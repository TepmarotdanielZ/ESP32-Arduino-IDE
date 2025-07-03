
#include <Wire.h>
#include <Adafruit_PN532.h>
#include "DHT.h"

/* DHT11 CONFIGURATION */  
#define DHTPIN 8
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

/* SENSOR PINS */
const int uvSensorPin = 3;
const int pirSensorPin = 12;
const int ledPin = 5;
const int LED_PIN = 6; 

/* PN532 I2C */   
#define SDA_PIN 36
#define SCL_PIN 35
Adafruit_PN532 nfc(SDA_PIN, SCL_PIN);

/* GLOBAL SENSOR DATA */   
float humidity = NAN;
float temperature = NAN;
int uvRawValue = 0;
float uvVoltage = 0.0;
float uvIntensity = 0.0;
int pirValue = LOW;

/* NFC FLAG  */  
int nfcFlag = 0; /* 1 = NFC TAG DETECTED, 0 = NO TAG */ 

/* MUTEX FOR SERIAL (OPTIONAL, FOR SAFETY) */  
SemaphoreHandle_t serialMutex;

void setup() {
  Serial.begin(115200);
  dht.begin();

  pinMode(uvSensorPin, INPUT);
  analogReadResolution(12);
  pinMode(pirSensorPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(LED_PIN, OUTPUT);  
  digitalWrite(ledPin, LOW);
  digitalWrite(LED_PIN, LOW);

  Wire.begin(SDA_PIN, SCL_PIN);
  nfc.begin();
  nfc.SAMConfig();

  serialMutex = xSemaphoreCreateMutex();

  /* CREATE TASKS */ 
  xTaskCreatePinnedToCore(taskPrintSensors, "PrintSensors", 4096, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(taskReadNFC, "ReadNFC", 4096, NULL, 1, NULL, 1);
}

void loop() {
  /* FReeRTOS HANDLES TASKS */ 
}

/* TASK: PRINT SENSORS */ 
void taskPrintSensors(void *pvParameters) {
  (void) pvParameters;

  while (true) {
    humidity = dht.readHumidity();
    temperature = dht.readTemperature();

    pirValue = digitalRead(pirSensorPin);
    digitalWrite(ledPin, pirValue);

    uvRawValue = analogRead(uvSensorPin);
    uvVoltage = (uvRawValue / 4095.0) * 3.3;
    uvIntensity = (uvVoltage / 2.5) * 1000.0;

    if (serialMutex) {
      xSemaphoreTake(serialMutex, portMAX_DELAY);
      Serial.print("Humidity: ");
      Serial.print(isnan(humidity) ? 0.00 : humidity, 2);
      Serial.print("% | Temperature: ");
      Serial.print(isnan(temperature) ? 0.00 : temperature, 2);
      Serial.print("°C | Raw: ");
      Serial.print(uvRawValue);
      Serial.print(" | Voltage: ");
      Serial.print(uvVoltage, 2);
      Serial.print(" V | UV Intensity: ");
      Serial.print(uvIntensity, 1);
      Serial.print(" µW/cm² | ");
      Serial.print(pirValue == HIGH ? "No Motion detected!" : "Motion detected");

      if (nfcFlag == 1) {
        Serial.print(" | No Scan Card");
      } else if (nfcFlag == 2) {
        Serial.print(" | Scan Card");
        nfcFlag = 3;
      }
      Serial.println("");
      xSemaphoreGive(serialMutex);
    }
    if (nfcFlag == 3) {
      nfcFlag = 0;
    }
    vTaskDelay(pdMS_TO_TICKS(1000));  /* DELAY 1 SECOND */ 
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
      if (nfcFlag == 1) {
        digitalWrite(LED_PIN, HIGH); 
      } else if (nfcFlag == 2) {
        digitalWrite(LED_PIN, LOW); 
        nfcFlag = 2;
      }
 
      
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

      vTaskDelay(pdMS_TO_TICKS(1000));  /* DEBOUNCE */ 
    } 

    vTaskDelay(pdMS_TO_TICKS(200));     /* SCAN INTERVAL */ 
  }
}
