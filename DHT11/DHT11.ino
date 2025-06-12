#include "DHT.h"  

#define DHTPIN 8        /* PIN DHT11 */
#define DHTTYPE DHT11   /* DHT 11  (AM2302), AM2321 */   


DHT dht(DHTPIN, DHTTYPE);  

void setup() {  
  Serial.begin(9600);  
  Serial.println(F("DHTxx test!"));  

  dht.begin();  
}  

void loop() {  
  /* READ THE HUMIDITY AND TEMPERATURE VALUES FROM THE DHT11 SENSOR */   
  float h = dht.readHumidity();  
  float t = dht.readTemperature();  
  float f = dht.readTemperature(true);  

  /* CHECK IF ANY READS FAILED AND EXIT EARLY (TO TRY AGAIN). */   
  if (isnan(h) || isnan(t) || isnan(f)) {  
    Serial.println(F("Failed to read from DHT sensor!"));  
    return;  
  }  

  /* PRINT TEMPERATURE AND HUMIDITY TO THE SERIAL MONITOR  */  
  Serial.print(F("Humidity: "));  
  Serial.print(h);  
  Serial.print(F("% | Temperature: "));  
  Serial.print(t);  
  Serial.println(F("°C "));  
 
  delay(2000); /* WAIT 2 SECONDS BEFORE THE NEXT LOOP */   
}  