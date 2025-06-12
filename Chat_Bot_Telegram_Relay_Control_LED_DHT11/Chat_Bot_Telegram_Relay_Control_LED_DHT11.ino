#include <WiFi.h>  
#include <WiFiClientSecure.h>  
#include <UniversalTelegramBot.h>  
#include "DHT.h"  

#define WIFI_SSID "Neatic"          /* TYPE YOUR WIFI NAME */ 
#define WIFI_PASSWORD "011570035"   /* TYPE YOUR WIFI PASSWORD  */  
#define BOT_TOKEN "8084553369:AAHgqoTlB1nURTs69WxGDE2KLjEXDW4soAw"   /* PASTE YOUR TOKEN NUMBER HERE  */  

const unsigned long BOT_MTBS = 1000; /* MEAN TIME BETWEEN SCAN MESSAGES  */  

WiFiClientSecure secured_client;  
UniversalTelegramBot bot(BOT_TOKEN, secured_client);  
unsigned long bot_lasttime; /* LAST TIME MESSAGES' SCAN HAS BEEN DONE */   


const int relayPin = 37; /* PIN RELAY */
const int DHTPIN = 8;    /* PIN FOR DHT11 SENSOR  */  
#define DHTTYPE DHT11    /* DHT 11 (AM2302), AM2321 */   

DHT dht(DHTPIN, DHTTYPE);  
int ledStatus = 0;  

void handleNewMessages(int numNewMessages)  
{  
  Serial.print("handleNewMessages ");  
  Serial.println(numNewMessages);  

  for (int i = 0; i < numNewMessages; i++)  
  {  
    String chat_id = bot.messages[i].chat_id;  
    String text = bot.messages[i].text;  
    Serial.println(text);  
    String from_name = bot.messages[i].from_name;  
    if (from_name == "")  
      from_name = "Guest";  

    if (text == "/ledon")  
    {  
      digitalWrite(relayPin, LOW); /* TURN THE LED ON (HIGH IS THE VOLTAGE LEVEL) */     
      ledStatus = 1;  
      bot.sendMessage(chat_id, "Led is ON", "", 0);  
    }  

    if (text == "/ledoff")  
    {  
      ledStatus = 0;  
      digitalWrite(relayPin, HIGH); /* TURN THE LED OFF (HIGH IS THE VOLTAGE LEVEL) */  
      bot.sendMessage(chat_id, "Led is OFF", "");  
    }  

    if (text == "/status")  
    {  
      if (ledStatus)  
      {  
        bot.sendMessage(chat_id, "Led is ON", "");  
      }  
      else  
      {  
        bot.sendMessage(chat_id, "Led is OFF", "");  
      }  
    }  

    if (text == "/dht")  
    {  
      float h = dht.readHumidity();  
      float t = dht.readTemperature();  

      if (isnan(h) || isnan(t)) {  
        bot.sendMessage(chat_id, "Failed to read from DHT sensor!", "");  
      } else {
        Serial.print("Humidity: ");  
        Serial.print(h);  
        Serial.print("% | Temperature: ");  
        Serial.print(t);  
        Serial.println("°C");    
        String message = "Humidity: " + String(h) + "% | Temperature: " + String(t) + "°C";  
        bot.sendMessage(chat_id, message, "");  
      }  
    }  

    if (text == "/start")  
    {  
      String welcome = "Telegram Chat Bot Control LED.\n";  
      welcome += "/ledon :  Led ON\n";  
      welcome += "/ledoff : Led OFF\n";  
      welcome += "/status : Returns current status of LED\n";  
      welcome += "/dht : Get current temperature and humidity from DHT sensor\n";  
      bot.sendMessage(chat_id, welcome, "Boyloy");  
    }  
  }  
}  

void setup()  
{  
  Serial.begin(115200);  
  Serial.println();  
  pinMode(relayPin, OUTPUT); /* INITIALIZE DIGITAL RELAY PIN AS AN OUTPUT. */   
  dht.begin(); /* INITIALIZE DHT11 SENSOR   */ 
  delay(10);  
  Serial.print("Connecting to Wifi SSID ");  
  Serial.print(WIFI_SSID);  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);  
  secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT); /* ADD ROOT CERTIFICATE FOR API.TELEGRAM.ORG  */  
  while (WiFi.status() != WL_CONNECTED)  
  {  
    Serial.print(".");  
    delay(500);  
  }  
  Serial.print("\nWiFi connected. IP address: ");  
  Serial.println(WiFi.localIP());  
}  

void loop()  
{  
  if (millis() - bot_lasttime > BOT_MTBS)  
  {  
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);  

    while (numNewMessages)  
    {  
      Serial.println("got response");  
      handleNewMessages(numNewMessages);  
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);  
    }  

    bot_lasttime = millis();  
  }  
}  