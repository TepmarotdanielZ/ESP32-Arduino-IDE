
#include <Wire.h>
#include "RTClib.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

/* WIFI CREDENTIALS */ 
const char* ssid = "Sonin KH";
const char* password = "77771112";

/* TELEGRAM BOT CREDENTIALS */ 
#define BOT_TOKEN "8084553369:AAHgqoTlB1nURTs69WxGDE2KLjEXDW4soAw"
#define CHAT_ID   "843845173"

/* I2C PINS FOR ESP32-S3 (RTC) */ 
#define SDA_PIN 8
#define SCL_PIN 9

RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {
  "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
};

WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

unsigned long previousMillis = 0;
const unsigned long interval = 3000; // 3 SECONDS

void setup() {
  Serial.begin(115200);

  // Initialize I2C
  Wire.begin(SDA_PIN, SCL_PIN);

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1) delay(10);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, setting time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n📶 WiFi connected");
  Serial.println("📩 Sending message to Telegram...");

  client.setInsecure(); // Trust all certificates
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    DateTime now = rtc.now();

    String yearStr = String(now.year());
    String monthStr = (now.month() < 10 ? "0" : "") + String(now.month());
    String dayStr = (now.day() < 10 ? "0" : "") + String(now.day());
    String hourStr = (now.hour() < 10 ? "0" : "") + String(now.hour());
    String minuteStr = (now.minute() < 10 ? "0" : "") + String(now.minute());
    String secondStr = (now.second() < 10 ? "0" : "") + String(now.second());
    String dayOfWeek = daysOfTheWeek[now.dayOfTheWeek()];

    float rtcTemp = rtc.getTemperature();

    // Determine temperature condition
    String tempCondition;
    if (rtcTemp >= 31 && rtcTemp <= 33) {
      tempCondition = " (COLD)";
    } else if (rtcTemp >= 33 && rtcTemp <= 34) {
      tempCondition = " (NORMAL)";
    } else if (rtcTemp >= 34 && rtcTemp <= 35) {
      tempCondition = " (HOT)";
    } else {
      tempCondition = "";  // for temps between 33.26 and 33.99
    }

    // Format message
    String message = "🗓️ " + dayOfWeek + " | " + dayStr + "/" + monthStr + "/" + yearStr + "\n" +
                     "⏰ " + hourStr + ":" + minuteStr + ":" + secondStr + "\n" +
                     "🌡️ Temp: " + String(rtcTemp, 2) + "ºC" + tempCondition;

    // Print to Serial
    Serial.println(message);
    Serial.println();

    // Send to Telegram
    bot.sendMessage(CHAT_ID, message, "Markdown");
  }
}
