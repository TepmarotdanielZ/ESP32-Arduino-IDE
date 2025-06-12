// #include <WiFi.h>
// #include <Firebase_ESP_Client.h>


// // #define WIFI_SSID "Boyloy"
// // #define WIFI_PASSWORD "011570035"

// #define WIFI_SSID "OGGY"
// #define WIFI_PASSWORD "22223333"

// #define API_KEY "AIzaSyCgdxYoVqFNrJaOc9Q0UGMk-qC7VOE-X90"
// #define DATABASE_URL "https://app-control-50530-default-rtdb.asia-southeast1.firebasedatabase.app/"  
// FirebaseData fbdo;
// FirebaseAuth auth;
// FirebaseConfig config;

// #define LAMP_PIN 5
 
// void setup() {
//   Serial.begin(115200);
//   pinMode(LAMP_PIN, OUTPUT);
//   digitalWrite(LAMP_PIN, LOW); // turn off initially

//   WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
//   Serial.print("Connecting to Wi-Fi");
//   while (WiFi.status() != WL_CONNECTED) {
//     Serial.print(".");
//     delay(500);
//   }
//   Serial.println("\nConnected to Wi-Fi");

//   // Firebase config
//   config.api_key = API_KEY;
//   config.database_url = DATABASE_URL;

//   auth.user.email = "";
//   auth.user.password = "";

//   Firebase.begin(&config, &auth);
//   Firebase.reconnectWiFi(true);
// }

// void loop() {
//   if (Firebase.RTDB.getInt(&fbdo, "/lamp")) {
//     int lampState = fbdo.intData();
//     Serial.print("Lamp value from Firebase: ");
//     Serial.println(lampState);
//     digitalWrite(LAMP_PIN, lampState == 1 ? HIGH : LOW);
//   } else {
//     Serial.print("Firebase get failed: ");
//     Serial.println(fbdo.errorReason());
//   }

//   delay(1000); // poll every second
// }




///

#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "Neatic"
#define WIFI_PASSWORD "011570035"

// Insert Firebase project API Key
#define API_KEY "AIzaSyCgdxYoVqFNrJaOc9Q0UGMk-qC7VOE-X90"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://app-control-50530-default-rtdb.asia-southeast1.firebasedatabase.app/" 

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int intValue;
float floatValue;
bool signupOK = false;

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("ok");
    signupOK = true;
  }
  else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  pinMode(14,OUTPUT);
 
}

void loop() {
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 1000 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();
    if (Firebase.RTDB.getString(&fbdo, "/App_Control/value")) {
      intValue = fbdo.stringData().toInt();
      if (intValue == 1){
         digitalWrite(14,HIGH);
      }else if (intValue == 0){
         digitalWrite(14,LOW);
      }
      
    }

    else {
      Serial.println(fbdo.errorReason());
    }
  }
}