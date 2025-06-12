

/**
 * @file      TESTING_ESP32_FIREBASE
 * @author    Tep marotdaniel (tepmarot050403@gmail.com)
 * @license   MIT
 * @copyright Copyright (c) 2024  AI FARM ROBOTICS FACTORY
 * @date      2024-12-01
*  @Time      03:51 PM
 * @last-update 2024-12-03
 *
 */

#include <Arduino.h>  
#if defined(ESP32)  
  #include <WiFi.h>  
#elif defined(ESP8266)  
  #include <ESP8266WiFi.h>  
#endif  
#include <Firebase_ESP_Client.h>  
#include <Wire.h>  
#include <Adafruit_Sensor.h>  


/* PROVIDE THE TOKEN GENERATION PROCESS INFO. */ 
  #include "addons/TokenHelper.h"  
  
/* PROVIDE THE RTDB PAYLOAD PRINTING INFO AND OTHER HELPER FUNCTIONS. */   
  #include "addons/RTDBHelper.h"  

#define WIFI_SSID "DREAMSLAB UNIFI"  
#define WIFI_PASSWORD "@DREAMSLAB2023"  
#define API_KEY "AIzaSyDM9hcFzlLNO2tJnLD7rLxZkgWgFJ3bTTM"  
#define USER_EMAIL "tepmarot050403@gmail.com"  
#define USER_PASSWORD "@-Tepmarot050403@&$#%"  
#define DATABASE_URL "https://fir-96cdf-default-rtdb.asia-southeast1.firebasedatabase.app/" 

/* DEFINE FIREBASE OBJECTS */  
  FirebaseData fbdo;  
  FirebaseAuth auth;  
  FirebaseConfig config;  

/* VARIABLE TO SAVE USER UID */   
  String uid;  

/* VARIABLES TO SAVE DATABASE PATHS */ 
  String databasePath;  
  String tempPath;  
  String humPath;  
  String presPath;  

/* BME280 SENSOR */   
/* ADAFRUIT_BME280 BME; I2C */   
  float temperature;  
  float humidity;  
  float pressure;  
  float contador = 1.0;  

/* TIMER VARIABLES (SEND NEW READINGS EVERY FIVE SECONDS) */  
  unsigned long sendDataPrevMillis = 0;  
  unsigned long timerDelay = 5000;  

/* INITIALIZE WIFI */  
void initWiFi() {  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);  
  Serial.print("Connecting to WiFi ..");  
  while (WiFi.status() != WL_CONNECTED) {  
    Serial.print('.');  
    delay(1000);  
  }  
  Serial.println(WiFi.localIP());  
  Serial.println();  
}  

/* WRITE FLOAT VALUES TO THE DATABASE */ 
void sendFloat(String path, float value) {  
  if (Firebase.RTDB.setFloat(&fbdo, path.c_str(), value)) {  
    Serial.print("Writing value: ");  
    Serial.print(value);  
    Serial.print(" on the following path: ");  
    Serial.println(path);  
    Serial.println("PASSED");  
    Serial.println("PATH: " + fbdo.dataPath());  
    Serial.println("TYPE: " + fbdo.dataType());  
  } else {  
    Serial.println("FAILED");  
    Serial.println("REASON: " + fbdo.errorReason());  
  }  
}  

void setup() {  
  Serial.begin(115200);  
  initWiFi();  

  /* ASSIGN THE API KEY (REQUIRED) */   
    config.api_key = API_KEY;  

  /* ASSIGN THE USER SIGN IN CREDENTIALS */ 
    auth.user.email = USER_EMAIL;  
    auth.user.password = USER_PASSWORD;  

  /* ASSIGN THE RTDB URL (REQUIRED) */ 
    config.database_url = DATABASE_URL;  

    Firebase.reconnectWiFi(true);  
    fbdo.setResponseSize(4096);  

  /* ASSIGN THE CALLBACK FUNCTION FOR THE LONG RUNNING TOKEN GENERATION TASK */ 
    config.token_status_callback = tokenStatusCallback; /*SEE ADDONS/TOKENHELPER.H */ 

  /* ASSIGN THE MAXIMUM RETRY OF TOKEN GENERATION */ 
    config.max_token_generation_retry = 5;  

  /* INITIALIZE THE LIBRARY WITH THE FIREBASE AUTHEN AND CONFIG */  
    Firebase.begin(&config, &auth);  

  /* GETTING THE USER UID MIGHT TAKE A FEW SECONDS */  
    Serial.println("Getting User UID");  
    while ((auth.token.uid) == "") {  
      Serial.print('.');  
      delay(1000);  
    }  

  /*PRINT USER UID */   
    uid = auth.token.uid.c_str();  
    Serial.print("User UID: ");  
    Serial.println(uid);  

  /* UPDATE DATABASE PATH */ 
    databasePath = "/UsersData/" + uid;  

  /* UPDATE DATABASE PATH FOR SENSOR READINGS  */   
    tempPath = databasePath + "/temperature"; /* --> USERSDATA/<USER_UID>/TEMPERATURE */ 
    humPath = databasePath + "/humidity";     /* --> USERSDATA/<USER_UID>/HUMIDITY */ 
    presPath = databasePath + "/pressure";    /* --> USERSDATA/<USER_UID>/PRESSURE */ 
}  

void loop() {  
  /* SEND NEW READINGS TO DATABASE */   
    if (Firebase.ready() && (millis() - sendDataPrevMillis > timerDelay || sendDataPrevMillis == 0)) {  
      sendDataPrevMillis = millis();  

    /* SIMULATED SENSOR READINGS FOR TESTING */ 
      if (contador > 20) { contador = 1.0; }  
      contador = contador + 1;  
      temperature = contador * 0.1;  
      humidity = contador * 0.2;  
      pressure = contador * 0.3;  

    /* SEND READINGS TO DATABASE */   
      sendFloat(tempPath, temperature);  
      sendFloat(humPath, humidity);  
      sendFloat(presPath, pressure);  
  }  
}



