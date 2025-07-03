#include "DHT.h"  

#define DHTPIN 8     
#define DHTTYPE DHT11   
 

DHT dht(DHTPIN, DHTTYPE);  

void setup() {  
  Serial.begin(9600);  
  Serial.println(F("DHTxx test!"));  

  dht.begin();  
}  

void loop() {   
  float h = dht.readHumidity();  
  float t = dht.readTemperature();  
  float f = dht.readTemperature(true);  
 
  if (isnan(h) || isnan(t) || isnan(f)) {  
    Serial.println(F("Failed to read from DHT sensor!"));  
    return;  
  }  
 
  Serial.print(F("Humidity: "));  
  Serial.print(h);  
  Serial.print(F("% | Temperature: "));  
  Serial.print(t);  
  Serial.println(F("°C "));  
 
  delay(2000); 
}  