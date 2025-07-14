#include <Wire.h>
#include "RTClib.h"

RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

/* DEFINE I2C PINS FOR ESP32-S3 */ 
#define SDA_PIN 8
#define SCL_PIN 9

unsigned long previousMillis = 0;
const unsigned long interval = 3000; 

void setup() {
  Serial.begin(115200);

  /* INITIALIZE I2C WITH CUSTOM SDA AND SCL PINS */ 
  Wire.begin(SDA_PIN, SCL_PIN);

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    DateTime now = rtc.now();

    String yearStr = String(now.year(), DEC);
    String monthStr = (now.month() < 10 ? "0" : "") + String(now.month(), DEC);
    String dayStr = (now.day() < 10 ? "0" : "") + String(now.day(), DEC);
    String hourStr = (now.hour() < 10 ? "0" : "") + String(now.hour(), DEC);
    String minuteStr = (now.minute() < 10 ? "0" : "") + String(now.minute(), DEC);
    String secondStr = (now.second() < 10 ? "0" : "") + String(now.second(), DEC);
    String dayOfWeek = daysOfTheWeek[now.dayOfTheWeek()];

    String formattedTime = dayOfWeek + " | " + dayStr + "/" + monthStr + "/" + yearStr + " | " + hourStr + ":" + minuteStr + ":" + secondStr;

    Serial.print(formattedTime);
    Serial.print(" | Temp: ");
    Serial.print(rtc.getTemperature());
    Serial.print("ºC");
    Serial.println();
  }
}