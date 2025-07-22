#include <Wire.h>                 /* FOR I2C COMMUNICATION (USED BY RTC) */ 
#include "RTClib.h"               /* ADAFRUIT LIBRARY FOR DS3231 RTC */ 
#include <WiFi.h>                 /* FOR WIFI CONNECTION */ 
#include <WiFiClientSecure.h>     /* FOR HTTPS CONNECTION */ 
#include <UniversalTelegramBot.h> /* TELEGRAM BOT LIBRARY */ 

/* WIFI CREDENTIALS */
const char* ssid = "Sonin KH";      /* WIFI SSID */ 
const char* password = "77771112";  /* WIFI PASSWORD */ 

/* TELEGRAM BOT CREDENTIALS */
#define BOT_TOKEN "8084553369:AAHgqoTlB1nURTs69WxGDE2KLjEXDW4soAw" /* TELEGRAM BOT TOKEN FROM: BotFather */ 
#define CHAT_ID   "843845173" /* CHAT ID TO SEND MESSAGES TO. (Userinfo | Get id | IDBot 👤) */ 

/* I2C PINS FOR ESP32-S3 (RTC) */
#define SDA_PIN 8 /* I2C SDA PIN */ 
#define SCL_PIN 9 /* I2C SCL PIN */ 

/* LED PIN */
#define LED_PIN 19 

RTC_DS3231 rtc; /* RTC OBJECT */ 

/* DAYS OF THE WEEK FOR DISPLAYING DATE */ 
char daysOfTheWeek[7][12] = {
  "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
};

/* SECURE CLIENT FOR TELEGRAM HTTPS REQUESTS */ 
WiFiClientSecure client;

/* TELEGRAM BOT OBJECT */ 
UniversalTelegramBot bot(BOT_TOKEN, client);

/* TIMER VARIABLES */ 
unsigned long previousMillis = 0;    /* FOR TRACKING TIME BETWEEN LOOP EXECUTIONS */ 
const unsigned long interval = 1000; /* LOOP INTERVAL = 1000MS = 1 SECOND */ 

/* SCHEDULE TIME VARIABLES */ 
int onHour = -1, onMinute = -1;     /* TIME TO TURN LED ON */ 
int offHour = -1, offMinute = -1;   /* TIME TO TURN LED OFF */ 

/* MANUAL OVERRIDE VARIABLES */ 
bool manualOverride = false; /* TRUE WHEN LED IS CONTROLLED MANUALLY */ 
bool manualOn = false;       /* TRUE IF LED SHOULD STAY ON DURING MANUAL OVERRIDE */ 

/* FUNCTION TO FORCE DS3231 TO UPDATE INTERNAL TEMPERATURE VALUE */ 
void forceTemperatureConversion() {
  Wire.beginTransmission(0x68); /* START COMMUNICATION WITH DS3231 */ 
  Wire.write(0x0E); /* CONTROL REGISTER ADDRESS */ 
  Wire.endTransmission();

  Wire.requestFrom(0x68, 1); /* REQUEST 1 BYTE FROM DS3231 */ 
  byte controlReg = Wire.read(); /* READ CONTROL REGISTER */ 

  controlReg |= 0x20; /* SET CONV BIT (BIT 5) TO FORCE TEMPERATURE CONVERSION */ 

  Wire.beginTransmission(0x68);
  Wire.write(0x0E); /* CONTROL REGISTER */ 
  Wire.write(controlReg); /* WRITE BACK WITH CONV BIT SET */ 
  Wire.endTransmission();
}

void setup() {
  Serial.begin(115200); /* START SERIAL MONITOR FOR DEBUGGING */ 

  Wire.begin(SDA_PIN, SCL_PIN); /* INITIALIZE I2C WITH CUSTOM PINS */ 

  pinMode(LED_PIN, OUTPUT);   /* SET LED PIN AS OUTPUT */ 
  digitalWrite(LED_PIN, LOW); /* INITIALLY TURN LED OFF */ 

  /* START RTC */ 
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1) delay(10); /* STOP HERE IF RTC NOT FOUND */ 
  }

  /* SET RTC TIME IF POWER LOST */ 
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, setting time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); /* SET TO COMPILE TIME */ 
  }

  /* CONNECT TO WIFI */ 
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n📶 WiFi connected");

  client.setInsecure(); /* ACCEPT INSECURE SSL CONNECTION (NO CERTIFICATE VERIFICATION) */ 
}

/* FUNCTION TO HANDLE TELEGRAM MESSAGES */ 
void handleTelegramMessages() {
  /* GET NEW MESSAGES SINCE LAST PROCESSED MESSAGE */ 
  int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

  while (numNewMessages) {
    for (int i = 0; i < numNewMessages; i++) {
      String text = bot.messages[i].text; /* USER MESSAGE */ 
      String chat_id = bot.messages[i].chat_id; /* SENDER'S CHAT ID */ 

      /* HANDLE /setime COMMAND (SET SCHEDULE) */ 
      if (text.startsWith("/setime ")) {
        int sep1 = text.indexOf(":");
        int sep2 = text.indexOf("to");

        /* FORMAT: /setime HH:MM to HH:MM */ 
        if (sep1 > 8 && sep2 > sep1 && text.length() > sep2 + 6) {
          String onStr = text.substring(8, sep2 - 1); /* EXTRACT ON TIME */ 
          String offStr = text.substring(sep2 + 3);   /* EXTRACT OFF TIME */ 

          /* PARSE ON TIME */ 
          int onH = onStr.substring(0, onStr.indexOf(":")).toInt();
          int onM = onStr.substring(onStr.indexOf(":") + 1).toInt();

          /* PARSE OFF TIME */ 
          int offH = offStr.substring(0, offStr.indexOf(":")).toInt();
          int offM = offStr.substring(offStr.indexOf(":") + 1).toInt();

          /* STORE TIMES */ 
          onHour = onH;
          onMinute = onM;
          offHour = offH;
          offMinute = offM;

          /* SEND CONFIRMATION MESSAGE */ 
          String msg = "✅ Schedule set:\n LED ON at " + String(onHour) + ":" + String(onMinute) +
                       "\n LED OFF at " + String(offHour) + ":" + String(offMinute);
          bot.sendMessage(chat_id, msg, "");
        } else {
          bot.sendMessage(chat_id, "❌ Invalid format. Use /setime HH:MM to HH:MM", "");
        }

      } 
      /* HANDLE /status COMMAND */ 
      else if (text == "/status") {
        String msg = "🔧 LED Schedule:\n";
        msg += "🟢 LED ON : " + (onHour >= 0 ? (String(onHour) + ":" + String(onMinute)) : "Not Set") + "\n";
        msg += "🔴 LED OFF: " + (offHour >= 0 ? (String(offHour) + ":" + String(offMinute)) : "Not Set") + "\n";
        msg += String("🛑 Manual Override: ") + (manualOn ? "ACTIVE (ON forever)" : "Inactive") + "\n";
        bot.sendMessage(chat_id, msg, "");
      } 
    }
    /* KEEP CHECKING NEW MESSAGES */ 
    numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  }
}

void loop() {
  handleTelegramMessages(); /* CHECK FOR NEW TELEGRAM COMMANDS */ 

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis; /* UPDATE LOOP TIMER */ 

    DateTime now = rtc.now(); /* GET CURRENT TIME */ 

    /* FORMAT DATE AND TIME */ 
    String yearStr = String(now.year());
    String monthStr = (now.month() < 10 ? "0" : "") + String(now.month());
    String dayStr = (now.day() < 10 ? "0" : "") + String(now.day());
    String hourStr = (now.hour() < 10 ? "0" : "") + String(now.hour());
    String minuteStr = (now.minute() < 10 ? "0" : "") + String(now.minute());
    String secondStr = (now.second() < 10 ? "0" : "") + String(now.second());
    String dayOfWeek = daysOfTheWeek[now.dayOfTheWeek()];

    /* UPDATE TEMPERATURE FROM RTC */ 
    forceTemperatureConversion();
    float rtcTemp = rtc.getTemperature();

    /* LED STATUS LOGIC BASED ON TEMPERATURE */ 
    bool ledStatus = digitalRead(LED_PIN);
    String tempCondition;

    if (rtcTemp >= 31 && rtcTemp <= 33) {
      tempCondition = " | (COLD)";
      if (!manualOverride) ledStatus = false;
    } else if (rtcTemp > 33 && rtcTemp < 34) {
      tempCondition = " | (NORMAL)";
      if (!manualOverride) ledStatus = false;
    } else if (rtcTemp >= 34 && rtcTemp <= 35) {
      tempCondition = " | (HOT)";
      if (!manualOverride) ledStatus = true;
    }

    /* TURN OFF LED AT SCHEDULED OFF TIME */ 
    if (offHour >= 0 && now.hour() == offHour && now.minute() == offMinute) {
      manualOverride = false;
      manualOn = false;
    }

    /* TURN ON LED AT SCHEDULED ON TIME */ 
    if (!manualOverride) {
      if (onHour >= 0 && now.hour() == onHour && now.minute() == onMinute) {
        manualOverride = true;
        manualOn = true;
      }
    }

    /* IF IN MANUAL OVERRIDE, USE THAT VALUE */ 
    if (manualOverride) {
      ledStatus = manualOn;
    }

    /* UPDATE LED OUTPUT */ 
    digitalWrite(LED_PIN, ledStatus ? HIGH : LOW);

    String ledStr = ledStatus ? "💡 LED : ON" : "💡 LED : OFF";

    /* CREATE MESSAGE WITH CURRENT TIME AND TEMP */ 
    String message = "📅 " + dayOfWeek + " | " + dayStr + "/" + monthStr + "/" + yearStr + "\n" +
                     "🕒 " + hourStr + ":" + minuteStr + ":" + secondStr + "\n" +
                     "🌡️ Temp: " + String(rtcTemp, 2) + "ºC" + tempCondition + "\n" +
                     ledStr;

    Serial.println(message); /* PRINT TO SERIAL MONITOR */ 
    Serial.println();

    bot.sendMessage(CHAT_ID, message, "Markdown"); /* SEND MESSAGE TO TELEGRAM */ 
  }
}
