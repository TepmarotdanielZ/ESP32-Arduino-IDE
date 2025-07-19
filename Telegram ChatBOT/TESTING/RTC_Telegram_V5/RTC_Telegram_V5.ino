
// #include <Wire.h>
// #include "RTClib.h"
// #include <WiFi.h>
// #include <WiFiClientSecure.h>
// #include <UniversalTelegramBot.h>

// /* WIFI CREDENTIALS */
// // const char* ssid = "Sonin KH";
// // const char* password = "77771112";


// const char* ssid = "Mengmonysonika🎀";
// const char* password = "nikaloveyou";

// /* TELEGRAM BOT CREDENTIALS */
// #define BOT_TOKEN "8084553369:AAHgqoTlB1nURTs69WxGDE2KLjEXDW4soAw"
// #define CHAT_ID   "843845173"

// /* I2C PINS FOR ESP32-S3 (RTC) */
// #define SDA_PIN 8
// #define SCL_PIN 9

// /* LED PIN */
// #define LED_PIN 19

// RTC_DS3231 rtc;

// char daysOfTheWeek[7][12] = {
//   "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
// };

// WiFiClientSecure client;
// UniversalTelegramBot bot(BOT_TOKEN, client);

// unsigned long previousMillis = 0;
// const unsigned long interval = 30; /* 3 SECONDS */ 

// /* SCHEDULED LED ON/OFF TIME */
// int onHour = -99999, onMinute = -99999;   /* TIME NO LED */
// int offHour = -99999, offMinute = -99999; /* TIME OFF LED */
// bool manualOverride = false;

// void setup() {
//   Serial.begin(115200);

//   Wire.begin(SDA_PIN, SCL_PIN);

//   pinMode(LED_PIN, OUTPUT);
//   digitalWrite(LED_PIN, LOW);

//   if (!rtc.begin()) {
//     Serial.println("Couldn't find RTC");
//     while (1) delay(10);
//   }

//   if (rtc.lostPower()) {
//     Serial.println("RTC lost power, setting time!");
//     rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
//   }

//   WiFi.begin(ssid, password);
//   Serial.print("Connecting to WiFi");
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }
//   Serial.println("\n📶 WiFi connected");

//   client.setInsecure(); /* TRUST ALL CERTIFICATES */ 
// }

// void handleTelegramMessages() {
//   int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

//   while (numNewMessages) {
//     for (int i = 0; i < numNewMessages; i++) {
//       String text = bot.messages[i].text;
//       String chat_id = bot.messages[i].chat_id;

//       if (text.startsWith("/seton ")) {
//         int sep = text.indexOf(":");
//         if (sep > 7) {
//           onHour = text.substring(7, sep).toInt();
//           onMinute = text.substring(sep + 1).toInt();
//           bot.sendMessage(chat_id, "✅ LED ON scheduled at " + String(onHour) + ":" + String(onMinute), "");
//         }
//       } else if (text.startsWith("/setoff ")) {
//         int sep = text.indexOf(":");
//         if (sep > 8) {
//           offHour = text.substring(8, sep).toInt();
//           offMinute = text.substring(sep + 1).toInt();
//           bot.sendMessage(chat_id, "✅ LED OFF scheduled at " + String(offHour) + ":" + String(offMinute), "");
//         }
//       } else if (text == "/status") {
//         String msg = "🔧 LED Schedule:\n";
//         msg += "🟢 ON: " + (onHour >= 0 ? (String(onHour) + ":" + String(onMinute)) : "Not Set") + "\n";
//         msg += "🔴 OFF: " + (offHour >= 0 ? (String(offHour) + ":" + String(offMinute)) : "Not Set");
//         bot.sendMessage(chat_id, msg, "");
//       }
//     }
//     numNewMessages = bot.getUpdates(bot.last_message_received + 1);
//   }
// }

// void loop() {
//   handleTelegramMessages();

//   unsigned long currentMillis = millis();
//   if (currentMillis - previousMillis >= interval) {
//     previousMillis = currentMillis;

//     DateTime now = rtc.now();
//     String yearStr = String(now.year());
//     String monthStr = (now.month() < 10 ? "0" : "") + String(now.month());
//     String dayStr = (now.day() < 10 ? "0" : "") + String(now.day());
//     String hourStr = (now.hour() < 10 ? "0" : "") + String(now.hour());
//     String minuteStr = (now.minute() < 10 ? "0" : "") + String(now.minute());
//     String secondStr = (now.second() < 10 ? "0" : "") + String(now.second());
//     String dayOfWeek = daysOfTheWeek[now.dayOfTheWeek()];
//     float rtcTemp = rtc.getTemperature();

//     bool ledStatus = digitalRead(LED_PIN);  /* ✅ DECLARE ledStatus BEFORE USE */  
//     String tempCondition;

//     if (rtcTemp >= 31 && rtcTemp <= 33) {
//       tempCondition = " | (COLD)";
//       ledStatus = false;
//     } else if (rtcTemp > 33 && rtcTemp < 34) {
//       tempCondition = " | (NORMAL)";
//       ledStatus = false;
//     } else if (rtcTemp >= 34 && rtcTemp <= 35) {
//       tempCondition = " | (HOT)";
//       ledStatus = true;  /* TURN ON LED DUE TO HOT temp */  
//     }

//     /* AUTOMATIC SCHEDULING (/seton): TURN ON LED */ 
//     if (onHour >= 0 && now.hour() == onHour && now.minute() == onMinute) {
//       ledStatus = true;
//     }
//     /* AUTOMATIC SCHEDULING (/setoff): TURN OFF LED */ 
//     if (offHour >= 0 && now.hour() == offHour && now.minute() == offMinute) {
//       ledStatus = false;
//     }

//     digitalWrite(LED_PIN, ledStatus ? HIGH : LOW);  /* APPLY STATUS */ 

//     String ledStr = ledStatus ? "💡 LED : ON" : "💡 LED : OFF";

//     String message = "📅 " + dayOfWeek + " | " + dayStr + "/" + monthStr + "/" + yearStr + "\n" +
//                      "🕒 " + hourStr + ":" + minuteStr + ":" + secondStr + "\n" +
//                      "🌡️ Temp: " + String(rtcTemp, 2) + "ºC" + tempCondition + "\n" +
//                      ledStr;

//     Serial.println(message);
//     Serial.println();

//     bot.sendMessage(CHAT_ID, message, "Markdown");
//   }
// }




///


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

/* LED PIN */
#define LED_PIN 19

RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {
  "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
};

WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

unsigned long previousMillis = 0;
const unsigned long interval = 30; // 3 seconds

/* SCHEDULED LED ON/OFF TIME */
int onHour = -99999, onMinute = -99999;   // Schedule for LED ON
int offHour = -99999, offMinute = -99999; // Schedule for LED OFF

bool manualOverride = false;    // Flag for manual override
bool manualOn = false;          // LED ON state in manual override

void setup() {
  Serial.begin(115200);

  Wire.begin(SDA_PIN, SCL_PIN);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

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

  client.setInsecure(); // Accept all certificates
}

void handleTelegramMessages() {
  int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

  while (numNewMessages) {
    for (int i = 0; i < numNewMessages; i++) {
      String text = bot.messages[i].text;
      String chat_id = bot.messages[i].chat_id;

      if (text.startsWith("/seton ")) {
        int sep = text.indexOf(":");
        if (sep > 7) {
          onHour = text.substring(7, sep).toInt();
          onMinute = text.substring(sep + 1).toInt();
          bot.sendMessage(chat_id, "✅ LED ON scheduled at " + String(onHour) + ":" + String(onMinute), "");
          manualOverride = false;
        }
      } else if (text.startsWith("/setoff ")) {
        int sep = text.indexOf(":");
        if (sep > 8) {
          offHour = text.substring(8, sep).toInt();
          offMinute = text.substring(sep + 1).toInt();
          bot.sendMessage(chat_id, "✅ LED OFF scheduled at " + String(offHour) + ":" + String(offMinute), "");
          manualOverride = false;
        }
      } else if (text == "/status") {
        String msg = "🔧 LED Schedule:\n";
        msg += "🟢 ON: " + (onHour >= 0 ? (String(onHour) + ":" + String(onMinute)) : "Not Set") + "\n";
        msg += "🔴 OFF: " + (offHour >= 0 ? (String(offHour) + ":" + String(offMinute)) : "Not Set") + "\n";
        msg += String("🛑 Manual Override: ") + (manualOn ? "ACTIVE (ON forever)" : "Inactive") + "\n";
        bot.sendMessage(chat_id, msg, "");
      } else if (text == "/seton") {
        manualOn = true;
        manualOverride = true;
        bot.sendMessage(chat_id, "✅ LED turned ON permanently", "");
      } else if (text == "/setoff") {
        manualOn = false;
        manualOverride = false;
        bot.sendMessage(chat_id, "✅ LED turned OFF", "");
      }
    }
    numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  }
}

void loop() {
  handleTelegramMessages();

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

    bool ledStatus = digitalRead(LED_PIN);
    String tempCondition;

    // Optional: Temp display only (not controlling LED)
    if (rtcTemp >= 31 && rtcTemp <= 33) {
      tempCondition = " | (COLD)";

    } else if (rtcTemp > 33 && rtcTemp < 34) {
      tempCondition = " | (NORMAL)";

    } else if (rtcTemp >= 34 && rtcTemp <= 35) {
      tempCondition = " | (HOT)";
      ledStatus = true;
    }

    // Handle scheduled ON/OFF if not in manual override
    if (!manualOverride) {
      if (onHour >= 0 && now.hour() == onHour && now.minute() == onMinute) {
        ledStatus = true;
      }
      if (offHour >= 0 && now.hour() == offHour && now.minute() == offMinute) {
        ledStatus = false;
      }
    }

    // Manual override always wins
    if (manualOverride) {
      ledStatus = manualOn;
    }

    digitalWrite(LED_PIN, ledStatus ? HIGH : LOW);

    String ledStr = ledStatus ? "💡 LED : ON" : "💡 LED : OFF";

    String message = "📅 " + dayOfWeek + " | " + dayStr + "/" + monthStr + "/" + yearStr + "\n" +
                     "🕒 " + hourStr + ":" + minuteStr + ":" + secondStr + "\n" +
                     "🌡️ Temp: " + String(rtcTemp, 2) + "ºC" + tempCondition + "\n" +
                     ledStr;

    Serial.println(message);
    Serial.println();

    bot.sendMessage(CHAT_ID, message, "Markdown");
  }
}
