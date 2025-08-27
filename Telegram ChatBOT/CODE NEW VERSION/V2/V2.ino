

/* CODE WORK V2 */

// #include <WiFi.h>
// #include "time.h"
// #include "esp_sntp.h"
// #include <WiFiClientSecure.h>
// #include <UniversalTelegramBot.h>

// /* WIFI CREDENTIALS */
// const char* ssid = "Sonin KH";
// const char* password = "77771112";

// /* TELEGRAM BOT CREDENTIALS */
// #define BOT_TOKEN "8084553369:AAHgqoTlB1nURTs69WxGDE2KLjEXDW4soAw"
// #define CHAT_ID   "843845173"

// /* LED PIN */
// #define LED_PIN 13

// WiFiClientSecure client;
// UniversalTelegramBot bot(BOT_TOKEN, client);

// /* NTP CONFIG */
// const char *ntpServer1 = "pool.ntp.org";
// const char *ntpServer2 = "time.nist.gov";
// const char *time_zone  = "ICT-7";  // Cambodia UTC+7

// /* TIMER VARIABLES */
// unsigned long previousMillis = 0;
// const unsigned long interval = 200;  // check every 1 second

// /* SCHEDULE VARIABLES */
// int onHour = -1, onMinute = -1;
// int offHour = -1, offMinute = -1;

// /* MANUAL OVERRIDE */
// bool manualOverride = false;
// bool manualOn = false;

// /* LED STATE */
// bool ledState = false;

// /* HELPER: GET LOCAL TIME */
// bool getLocalTimeNow(struct tm *timeinfo) {
//   if (!getLocalTime(timeinfo)) {
//     Serial.println("⚠️ Failed to obtain time");
//     return false;
//   }
//   return true;
// }

// /* HANDLE TELEGRAM COMMANDS */
// void handleTelegramMessages() {
//   int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

//   while (numNewMessages) {
//     for (int i = 0; i < numNewMessages; i++) {
//       String text = bot.messages[i].text;
//       String chat_id = bot.messages[i].chat_id;

//       // Command: set schedule -> /setime HH:MM to HH:MM
//       if (text.startsWith("/setime ")) {
//         int sep1 = text.indexOf(":");
//         int sep2 = text.indexOf("to");

//         if (sep1 > 8 && sep2 > sep1 && text.length() > sep2 + 6) {
//           String onStr = text.substring(8, sep2 - 1);
//           String offStr = text.substring(sep2 + 3);

//           int onH = onStr.substring(0, onStr.indexOf(":")).toInt();
//           int onM = onStr.substring(onStr.indexOf(":") + 1).toInt();

//           int offH = offStr.substring(0, offStr.indexOf(":")).toInt();
//           int offM = offStr.substring(offStr.indexOf(":") + 1).toInt();

//           onHour = onH;
//           onMinute = onM;
//           offHour = offH;
//           offMinute = offM;

//           // Disable manual override when a new schedule is set
//           manualOverride = false;

//           String msg = "✅ Schedule set:\n LED ON at " + String(onHour) + ":" + String(onMinute) +
//                        "\n LED OFF at " + String(offHour) + ":" + String(offMinute);
//           bot.sendMessage(chat_id, msg, "");
//         } else {
//           bot.sendMessage(chat_id, "❌ Invalid format. Use /setime HH:MM to HH:MM", "");
//         }
//       }

//       // Command: /status
//       else if (text == "/status") {
//         String msg = "🔧 LED Schedule:\n";
//         msg += "🟢 LED ON : " + (onHour >= 0 ? (String(onHour) + ":" + String(onMinute)) : "Not Set") + "\n";
//         msg += "🔴 LED OFF: " + (offHour >= 0 ? (String(offHour) + ":" + String(offMinute)) : "Not Set") + "\n";
//         msg += String("🛑 Manual Override: ") + (manualOverride ? (manualOn ? "LED ON" : "LED OFF") : "Inactive") + "\n";
//         msg += String("💡 Current LED: ") + (ledState ? "ON" : "OFF");
//         bot.sendMessage(chat_id, msg, "");
//       }

//       // Command: turn LED on manually
//       else if (text == "/ledon") {
//         manualOverride = true;
//         manualOn = true;
//         ledState = true;
//         digitalWrite(LED_PIN, HIGH);
//         bot.sendMessage(chat_id, "💡 LED turned ON (manual override)", "");
//       }

//       // Command: turn LED off manually
//       else if (text == "/ledoff") {
//         manualOverride = true;
//         manualOn = false;
//         ledState = false;
//         digitalWrite(LED_PIN, LOW);
//         bot.sendMessage(chat_id, "💡 LED turned OFF (manual override)", "");
//       }

//       // Command: disable manual override
//       else if (text == "/resume") {
//         manualOverride = false;
//         bot.sendMessage(chat_id, "✅ Manual override disabled. Schedule active.", "");
//       }

//     }
//     numNewMessages = bot.getUpdates(bot.last_message_received + 1);
//   }
// }

// void setup() {
//   Serial.begin(115200);
//   pinMode(LED_PIN, OUTPUT);
//   digitalWrite(LED_PIN, LOW);

//   // WiFi connect
//   WiFi.begin(ssid, password);
//   Serial.print("Connecting to WiFi");
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }
//   Serial.println("\n📶 WiFi connected");

//   client.setInsecure();  // ignore SSL certificate

//   // Setup NTP
//   sntp_set_time_sync_notification_cb([](struct timeval *tv) {
//     Serial.println("⏳ NTP time synchronized!");
//   });
//   configTzTime(time_zone, ntpServer1, ntpServer2);

//   bot.sendMessage(CHAT_ID, "🤖 ESP32 Ready! Send /setime, /status, /ledon, /ledoff, /resume", "");
// }

// void loop() {
//   handleTelegramMessages();

//   unsigned long currentMillis = millis();
//   if (currentMillis - previousMillis >= interval) {
//     previousMillis = currentMillis;

//     struct tm timeinfo;
//     if (getLocalTimeNow(&timeinfo)) {
//       int hourNow = timeinfo.tm_hour;
//       int minuteNow = timeinfo.tm_min;

//       // Apply LED state
//       if (manualOverride) {
//         digitalWrite(LED_PIN, manualOn ? HIGH : LOW);
//         ledState = manualOn;
//       } else {
//         // Scheduled control
//         if (onHour >= 0 && offHour >= 0) {
//           if (hourNow == onHour && minuteNow == onMinute) ledState = true;
//           else if (hourNow == offHour && minuteNow == offMinute) ledState = false;
//         }
//         digitalWrite(LED_PIN, ledState ? HIGH : LOW);
//       }

//       // Debug print
//       char buffer[64];
//       strftime(buffer, sizeof(buffer), "%A, %d/%m/%Y %H:%M:%S", &timeinfo);
//       Serial.println(String(buffer) + " | LED: " + (ledState ? "ON" : "OFF"));
//     }
//   }
// }

///




#include <WiFi.h>
#include "time.h"
#include "esp_sntp.h"
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

/* WIFI CREDENTIALS */
const char* ssid = "Sonin KH";
const char* password = "77771112";

/* TELEGRAM BOT CREDENTIALS */
#define BOT_TOKEN "8084553369:AAHgqoTlB1nURTs69WxGDE2KLjEXDW4soAw"
#define CHAT_ID   "843845173"

/* LED PIN */
#define LED_PIN 13

WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

/* NTP CONFIG */
const char *ntpServer1 = "pool.ntp.org";
const char *ntpServer2 = "time.nist.gov";
const char *time_zone  = "ICT-7";  // Cambodia UTC+7

/* TIMER VARIABLES */
unsigned long previousMillis = 0;
const unsigned long interval = 200;  // check every 1 second

/* SCHEDULE VARIABLES */
int onHour = -1, onMinute = -1;
int offHour = -1, offMinute = -1;

/* MANUAL OVERRIDE */
bool manualOverride = false;
bool manualOn = false;

/* LED STATE */
bool ledState = false;

/* HELPER: GET LOCAL TIME */
bool getLocalTimeNow(struct tm *timeinfo) {
  if (!getLocalTime(timeinfo)) {
    Serial.println("⚠️ Failed to obtain time");
    return false;
  }
  return true;
}

/* HANDLE TELEGRAM COMMANDS */
void handleTelegramMessages() {
  int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

  while (numNewMessages) {
    for (int i = 0; i < numNewMessages; i++) {
      String text = bot.messages[i].text;
      String chat_id = bot.messages[i].chat_id;

      // Command: set schedule -> /setime HH:MM to HH:MM
      if (text.startsWith("/setime ")) {
        int sep1 = text.indexOf(":");
        int sep2 = text.indexOf("to");

        if (sep1 > 8 && sep2 > sep1 && text.length() > sep2 + 6) {
          String onStr = text.substring(8, sep2 - 1);
          String offStr = text.substring(sep2 + 3);

          int onH = onStr.substring(0, onStr.indexOf(":")).toInt();
          int onM = onStr.substring(onStr.indexOf(":") + 1).toInt();

          int offH = offStr.substring(0, offStr.indexOf(":")).toInt();
          int offM = offStr.substring(offStr.indexOf(":") + 1).toInt();

          onHour = onH;
          onMinute = onM;
          offHour = offH;
          offMinute = offM;

          // Disable manual override when a new schedule is set
          manualOverride = false;

          String msg = "✅ Schedule set:\n LED ON at " + String(onHour) + ":" + String(onMinute) +
                       "\n LED OFF at " + String(offHour) + ":" + String(offMinute);
          bot.sendMessage(chat_id, msg, "");
        } else {
          bot.sendMessage(chat_id, "❌ Invalid format. Use /setime HH:MM to HH:MM", "");
        }
      }

      // Command: /status
      else if (text == "/status") {
        String msg = "🔧 LED Schedule:\n";
        msg += "🟢 LED ON : " + (onHour >= 0 ? (String(onHour) + ":" + String(onMinute)) : "Not Set") + "\n";
        msg += "🔴 LED OFF: " + (offHour >= 0 ? (String(offHour) + ":" + String(offMinute)) : "Not Set") + "\n";
        msg += String("🛑 Manual Override: ") + (manualOverride ? (manualOn ? "LED ON" : "LED OFF") : "Inactive") + "\n";
        msg += String("💡 Current LED: ") + (ledState ? "ON" : "OFF");
        bot.sendMessage(chat_id, msg, "");
      }

      // Command: turn LED on manually
      else if (text == "/ledon") {
        manualOverride = true;
        manualOn = true;
        ledState = true;
        digitalWrite(LED_PIN, HIGH);
        bot.sendMessage(chat_id, "💡 LED turned ON (manual override)", "");
      }

      // Command: turn LED off manually
      else if (text == "/ledoff") {
        manualOverride = true;
        manualOn = false;
        ledState = false;
        digitalWrite(LED_PIN, LOW);
        bot.sendMessage(chat_id, "💡 LED turned OFF (manual override)", "");
      }

      // Command: disable manual override
      else if (text == "/resume") {
        manualOverride = false;
        bot.sendMessage(chat_id, "✅ Manual override disabled. Schedule active.", "");
      }

    }
    numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // WiFi connect
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n📶 WiFi connected");

  client.setInsecure();  // ignore SSL certificate

  // Setup NTP
  sntp_set_time_sync_notification_cb([](struct timeval *tv) {
    Serial.println("⏳ NTP time synchronized!");
  });
  configTzTime(time_zone, ntpServer1, ntpServer2);

  bot.sendMessage(CHAT_ID, "🤖 ESP32 Ready! Send /setime, /status, /ledon, /ledoff", "");
}

void loop() {
  handleTelegramMessages();

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    struct tm timeinfo;
    if (getLocalTimeNow(&timeinfo)) {
      int hourNow = timeinfo.tm_hour;
      int minuteNow = timeinfo.tm_min;

      // Apply LED state
      if (manualOverride) {
        digitalWrite(LED_PIN, manualOn ? HIGH : LOW);
        ledState = manualOn;
      } else {
        // Scheduled control
        if (onHour >= 0 && offHour >= 0) {
          if (hourNow == onHour && minuteNow == onMinute) ledState = true;
          else if (hourNow == offHour && minuteNow == offMinute) ledState = false;
        }
        digitalWrite(LED_PIN, ledState ? HIGH : LOW);
      }

      // Debug print
      char buffer[64];
      strftime(buffer, sizeof(buffer), "%A, %d/%m/%Y %H:%M:%S", &timeinfo);
      Serial.println(String(buffer) + " | LED: " + (ledState ? "ON" : "OFF"));
    }
  }
}

