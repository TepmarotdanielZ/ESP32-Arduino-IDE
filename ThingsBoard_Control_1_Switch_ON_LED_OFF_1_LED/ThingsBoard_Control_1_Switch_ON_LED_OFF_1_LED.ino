
#include <WiFi.h>
#include <PubSubClient.h>

/* WIFI CREDENTIALS */  
const char* ssid = "Neatic";  
const char* password = "011570035";  

/* THINGSBOARD CREDENTIALS */  
const char* mqttServer = "chiptree-iot.aifarm.dev";  
const int mqttPort = 1883;  
const char* mqttUser = "B7L7sgrIBDFSg69hpiXu";  
const char* telemetryTopic = "v1/devices/me/telemetry";  
const char* rpcTopic = "v1/devices/me/rpc/request/+";

/* LED CONFIGURATION */  
const int ledPin = 5;  /* GPIO 5 FOR LED */  

/* CREATE A WIFI AND MQTT CLIENT */ 
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  /* INITIALIZE SERIAL COMMUNICATION */ 
  Serial.begin(115200);
  
  /* SET LED PIN AS OUTPUT */ 
  pinMode(ledPin, OUTPUT);
  
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
}

void connectToWiFi() {
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");
}

void connectToMQTT() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect("ESP32Client", mqttUser, "")) {
      Serial.println("connected");
      client.subscribe(rpcTopic);  /* SUBSCRIBE TO RPC TOPIC */ 
    } else {
      Serial.print("failed, rc=");
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

/*
  CHECK FOR LED STATE BASED ON THE NEW MESSAGE FORMAT  
  ASSUMING YOUR LED STATE IS INTENDED TO BE TRANSMITTED AS A BOOLEAN 
*/ 
  if (message.indexOf("\"params\":false") != -1) {  
    digitalWrite(ledPin, HIGH); /* TURN LED OFF */  
    Serial.println("LED State: ON");  
  } else if (message.indexOf("\"params\":true") != -1) {  
    digitalWrite(ledPin, LOW); /* TURN LED ON  */ 
    Serial.println("LED State: OFF");  
  }  
}  
