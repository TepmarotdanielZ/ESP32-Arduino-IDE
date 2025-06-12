
#include <WiFi.h>  
#include <WiFiClientSecure.h>  
#include <UniversalTelegramBot.h>  
#include "DHT.h"  

#define WIFI_SSID "Sonin KH"      /* YOUR WIFI SSID */  
#define WIFI_PASSWORD "77771112"  /* YOUR WIFI PASSWORD */ 
#define BOT_TOKEN "8084553369:AAHgqoTlB1nURTs69WxGDE2KLjEXDW4soAw" //  

const unsigned long BOT_MTBS = 1000; /* YOUR TELEGRAM BOT TOKEN */  

WiFiClientSecure secured_client;  
UniversalTelegramBot bot(BOT_TOKEN, secured_client);  
unsigned long bot_lasttime; /* LAST TIME MESSAGES' SCAN HAS BEEN DONE  */  

const int DHTPIN = 8; /* PIN FOR DHT-11 SENSOR  */  
#define DHTTYPE DHT11 /* DEFINE THE TYPE OF DHT-11 SENSOR */   

DHT dht(DHTPIN, DHTTYPE);  

void setup() {  
  Serial.begin(115200);  
  Serial.println();  
  
  dht.begin(); /* INITIALIZE DHT SENSOR  */  

  /* CONNECT TO WI-FI */   
  Serial.print("Connecting to WiFi SSID: ");  
  Serial.print(WIFI_SSID);  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);  
  
  /* ENSURE A SECURE CONNECTION */   
  secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT); /* ADD ROOT CERTIFICATE FOR API.TELEGRAM.ORG  */  
  
  while (WiFi.status() != WL_CONNECTED) {  
    Serial.print(".");  
    delay(500);  
  }  
  
  Serial.print("\nWiFi connected. IP address: ");  
  Serial.println(WiFi.localIP());  
}  

void handleNewMessages(int numNewMessages) {  
  Serial.print("handleNewMessages: ");  
  Serial.println(numNewMessages);  
  
  for (int i = 0; i < numNewMessages; i++) {  
    String chat_id = bot.messages[i].chat_id;  
    String text = bot.messages[i].text;  
    Serial.println(text);  

    /* RESPOND TO COMMANDS */   
    if (text == "/dht") {  
      float h = dht.readHumidity();  
      float t = dht.readTemperature();  
      
      /* CHECK FOR READING ERRORS */  
      if (isnan(h) || isnan(t)) {  
        bot.sendMessage(chat_id, "Failed to read from DHT sensor!", "");  
      } else {  
        /* PRINT TEMPERATURE AND HUMIDITY TO SERIAL MONITOR */  
        Serial.print("Humidity: ");  
        Serial.print(h);  
        Serial.print("% | Temperature: ");  
        Serial.print(t);  
        Serial.println("°C");  

        String message = "Humidity: " + String(h) + "% | Temperature: " + String(t) + "°C";  
        bot.sendMessage(chat_id, message, "");  
      }  
    }  

    if (text == "/start") {  
      String welcome = "Welcome to the DHT11 Telegram Bot.\n";  
      welcome += "/dht - Get current temperature and humidity from DHT11 sensor\n";  
      bot.sendMessage(chat_id, welcome, "");  
    }  
  }  
}  

void loop() {  
  if (millis() - bot_lasttime > BOT_MTBS) {  
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);  

    while (numNewMessages) {  
      Serial.println("Got response");  
      handleNewMessages(numNewMessages);  
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);  
    }  

    bot_lasttime = millis();  
  }  
}  