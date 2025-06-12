
#include <Wire.h>  
#include <Adafruit_PN532.h>  

/* DEFINE I2C PINS FOR ESP32  */  
#define SDA_PIN 36  
#define SCL_PIN 35  

/* DEFINE RELAY PIN  */  
#define RELAY_PIN 37 

/* CREATE THE PN532 OBJECT USING THE I2C BUS  */  
Adafruit_PN532 nfc(SDA_PIN, SCL_PIN);  

/* VARIABLE TO TRACK THE SCAN COUNT */     
int scanCount = 0;  

void setup(void) {  
  Serial.begin(115200);  
  Serial.println("PN532 NFC Reader");  
  
  /* INITIALIZE THE RELAY PIN */   
  pinMode(RELAY_PIN, OUTPUT);  
  digitalWrite(RELAY_PIN, LOW);   /* ENSURE THE RELAY IS OFF AT STARTUP  */  

  /* INITIALIZE THE I2C INTERFACE  */  
  Wire.begin(SDA_PIN, SCL_PIN);  
  
  /* INITIALIZE THE PN532   */ 
  nfc.begin();  

  /* CONFIGURE THE PN532 TO COMMUNICATE WITH CARDS  */  
  nfc.SAMConfig();  
  Serial.println("Waiting for an NFC card...");  
}  

void loop(void) {  
  /* LISTEN FOR AN NFC CARD   */ 
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  
  uint8_t uidLength;  

  if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength)) {  
    Serial.println("Found an NFC card!");  

    /* INCREMENT THE SCAN COUNT   */ 
    scanCount++;  

    /* CONTROL THE RELAY BASED ON THE SCAN COUNT   */ 
    if (scanCount == 1) {  
      /* TURN ON THE RELAY FOR THE FIRST SCAN */  
      digitalWrite(RELAY_PIN, HIGH);  
      Serial.println("STATUS LED: OFF");  /* PRINT STATUS OF LED */ 
    } else if (scanCount == 2) {  
      // Turn off the LED for the second scan  
      digitalWrite(RELAY_PIN, LOW);  
      Serial.println("STATUS LED: ON");  /* PRINT STATUS ON LED */ 
      /* RESET THE SCAN COUNT TO ALLOW TOGGLING ON FURTHER SCANS */   
      scanCount = 0;  
    }  
    
    /* PRINT THE UID  */   
    Serial.print("UID Length: "); Serial.print(uidLength, DEC); Serial.println(" bytes");  
    Serial.print("UID Value: ");  
    for (uint8_t i = 0; i < uidLength; i++) {  
      Serial.print(" 0x"); Serial.print(uid[i], HEX);  
    }  
    Serial.println("");  

    /* WAIT FOR A SECOND BEFORE CHECKING FOR THE NEXT CARD  */  
    delay(1000);  
  }  
}  