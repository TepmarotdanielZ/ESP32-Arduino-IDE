#include "DHT.h"

#define DHTPIN 8        /* PIN DHT11 */
#define DHTTYPE DHT11   /* DHT 11 */

const int uvSensorPin = 3;    /* GPIO14 ON ESP32 */
const int pirSensorPin = 12;  /* PIR SENSOR PIN */
const int ledPin = 6;         /* LED INDICATOR PIN */

DHT dht(DHTPIN, DHTTYPE);

/* TIMING VARIABLES */ 
unsigned long previousMillis = 0;

const long interval = 1000;  /* 1 SECOND INTERVAL FOR PRINTING */

float humidity = NAN;
float temperature = NAN;
int uvRawValue = 0;
float uvVoltage = 0.0;
float uvIntensity = 0.0;

int pirValue = LOW;  /* STORE LATEST PIR READING */ 

void setup() {
  Serial.begin(115200);
  dht.begin();

  pinMode(uvSensorPin, INPUT);
  analogReadResolution(12);  /* FOR ESP32 ADC RESOLUTION */ 

  pinMode(pirSensorPin, INPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
}

void loop() {
  unsigned long currentMillis = millis();

  /* READ PIR SENSOR FREQUENTLY (NON-BLOCKING) */ 
  pirValue = digitalRead(pirSensorPin);
  digitalWrite(ledPin, pirValue == HIGH ? HIGH : LOW);

  /* READ DHT11 HUMIDITY AND TEMPERATURE EVERY 2 SECONDS (TO AVOID SENSOR SATURATION) */ 
  static unsigned long previousDHTMillis = 0;
  const long dhtInterval = 2000;  /* 2 SEC */ 
  if (currentMillis - previousDHTMillis >= dhtInterval) {
    previousDHTMillis = currentMillis;
    humidity = dht.readHumidity();
    temperature = dht.readTemperature();
  }

  /* READ AND PRINT ALL DATA EVERY 1 SECOND */ 
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    uvRawValue = analogRead(uvSensorPin);
    uvVoltage = (uvRawValue / 4095.0) * 3.3;
    uvIntensity = (uvVoltage / 2.5) * 1000.0;

    if (isnan(humidity) || isnan(temperature)) {
      Serial.println(F("Failed to read from DHT sensor!"));
    } else {
      Serial.print(F("Humidity: "));
      Serial.print(humidity, 2);
      Serial.print(F("% | Temperature: "));
      Serial.print(temperature, 2);
      Serial.print(F("°C | Raw: "));
      Serial.print(uvRawValue);
      Serial.print(F(" | Voltage: "));
      Serial.print(uvVoltage, 2);
      Serial.print(F(" V | UV Intensity: "));
      Serial.print(uvIntensity, 1);
      Serial.print(F(" µW/cm² | "));

      if (pirValue == HIGH) {
        Serial.println(F("No motion detected"));
      } else {
        Serial.println(F("Motion detected!"));
      }
    }
  }
}