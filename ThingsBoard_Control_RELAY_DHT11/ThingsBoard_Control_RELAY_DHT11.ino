

#include <WiFi.h>  
#include <PubSubClient.h>  
#include "DHT.h"  

/* WIFI CREDENTIALS */  
const char* ssid = "Neatic";  
const char* password = "011570035";  

/* THINGSBOARD CREDENTIALS */  
const char* mqttServer = "chiptree-iot.aifarm.dev";  
const int mqttPort = 1883;  
const char* mqttUser = "B7L7sgrIBDFSg69hpiXu";  
const char* telemetryTopic = "v1/devices/me/telemetry";  
const char* rpcTopic = "v1/devices/me/rpc/request/+";  

/* DHT CONFIGURATION */  
#define DHTPIN 8        /* PIN WHERE THE DHT11 SENSOR IS CONNECTED */   
#define DHTTYPE DHT11   /* DHT 11 (AM2302), AM2321 */   

DHT dht(DHTPIN, DHTTYPE);  

/* RELAY CONFIGURATION */  
const int RelayPin = 37;  /* GPIO 37 FOR RELAY */  

/* CREATE A WIFI AND MQTT CLIENT */  
WiFiClient espClient;  
PubSubClient client(espClient);  

/* TIMING VARIABLES */   
unsigned long lastDHTRead = 0;  
const long dhtReadInterval = 300;  /* 3 SECONDS FOR DHT DATA READ */   

void setup() {  
    /* INITIALIZE SERIAL COMMUNICATION */   
    Serial.begin(115200);  
    Serial.println(F("DHTxx test!"));  

    /* INITIALIZE DHT SENSOR */   
    dht.begin();  

    /* SET RELAY PIN AS OUTPUT */   
    pinMode(RelayPin, OUTPUT);  

    /* CONNECT TO WI-FI */   
    connectToWiFi();  

    /* SET UP MQTT CLIENT */   
    client.setServer(mqttServer, mqttPort);  
    client.setCallback(mqttCallback);  

    /* CONNECT TO MQTT BROKER */   
    connectToMQTT();  
}  

void loop() {  
    /* MAINTAIN MQTT CONNECTION */   
    if (!client.connected()) {  
        connectToMQTT();  
    }  
    client.loop();  

    /* CHECK IF IT'S TIME TO READ DATA FROM DHT */  
    unsigned long currentMillis = millis();  
    if (currentMillis - lastDHTRead >= dhtReadInterval) {  
        lastDHTRead = currentMillis;  /* SAVE THE LAST READ TIME */   
        readAndPublishDHTData();      /* READ AND PUBLISH DATA FROM THE DHT SENSOR */   
    }  
}  

void connectToWiFi() {  
    Serial.print("Connecting to WiFi...");  
    WiFi.begin(ssid, password);  

    while (WiFi.status() != WL_CONNECTED) {  
        delay(1000);  /* USE DELAY HERE TO KEEP CHECKING UNTIL WI-FI IS CONNECTED */   
        Serial.print(".");  
    }  
    Serial.println(" Connected to WiFi");  
}  

void connectToMQTT() {  
    while (!client.connected()) {  
        Serial.print("Connecting to MQTT...");  
        if (client.connect("ESP32Client", mqttUser, "")) {  
            Serial.println(" connected");  
            client.subscribe(rpcTopic);  /* SUBSCRIBE TO RPC TOPIC */  
        } else {  
            Serial.print(" failed, rc=");  
            Serial.print(client.state());  
            delay(2000);  
        }  
    }  
}  

void mqttCallback(char* topic, byte* payload, unsigned int length) {  
    /* CONVERT PAYLOAD TO STRING */   
    String message;  
    for (int i = 0; i < length; i++) {  
        message += (char)payload[i];  
    }  

    Serial.print("Message received: ");  
    Serial.println(message);  

    /* CHECK FOR RELAY STATE BASED ON THE MESSAGE FORMAT */  
    if (message.indexOf("\"params\":false") != -1) {  
        digitalWrite(RelayPin, HIGH); /* TURN RELAY OFF */  
        Serial.println("RELAY STATE: OFF");  
    } else if (message.indexOf("\"params\":true") != -1) {  
        digitalWrite(RelayPin, LOW); /* TURN RELAY ON */  
        Serial.println("RELAY STATE: ON");  
    }  
}  

void readAndPublishDHTData() {  
    /* READ HUMIDITY AND TEMPERATURE */  
    float h = dht.readHumidity();  
    float t = dht.readTemperature();  

    /* CHECK FOR READ ERRORS */   
    if (isnan(h) || isnan(t)) {  
        Serial.println(F("Failed to read from DHT sensor!"));  
        return;  
    }  

    /* PUBLISH THE SENSOR READINGS TO THINGSBOARD */   
    String payload = "{\"humidity\": " + String(h) + ", \"temperature\": " + String(t) + "}";  
    if (client.publish(telemetryTopic, payload.c_str())) {  
        Serial.println("Published: " + payload);  
    } else {  
        Serial.println("Failed to publish data");  
    }  
}  