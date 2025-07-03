#include "DHT.h"

#define DHTPIN 8        /* PIN DHT11 */
#define DHTTYPE DHT11   /* DHT 11 */

const int uvSensorPin = 3; /* GPIO14 ON ESP32 */

DHT dht(DHTPIN, DHTTYPE);

unsigned long previousMillis = 0;
const long interval = 1000; /* 1 SECOND */

float humidity = NAN;
float temperature = NAN;
int uvRawValue = 0;
float uvVoltage = 0.0;
float uvIntensity = 0.0;

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(uvSensorPin, INPUT);
  analogReadResolution(12);
}

void loop() {
  unsigned long currentMillis = millis();

  /* READ DHT SENSOR MORE FREQUENTLY WITHOUT BLOCKING
    BUT ONLY UPDATE HUMIDITY & TEMPERATURE READINGS EVERY 2 SECONDS
  */
  static unsigned long previousDHTMillis = 0;
  const long dhtInterval = 2000; /* 2 SECONDS DELAY FOR DHT */ 

  if (currentMillis - previousDHTMillis >= dhtInterval) {
    previousDHTMillis = currentMillis;

    humidity = dht.readHumidity();
    temperature = dht.readTemperature();
  }

  /* READ UV SENSOR AND PRINT ALL DATA EVERY 1 SECOND */ 
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
      Serial.println(F(" µW/cm²"));
    }
  }
}