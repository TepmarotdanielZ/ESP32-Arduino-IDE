const int BuzzerPin = 7;  
unsigned long previousMillis = 0;  

 
const long beepInterval = 1000;  
bool buzzerState = false;  

void setup() {  
  pinMode(BuzzerPin, OUTPUT);  
}  

void loop() {   
  unsigned long currentMillis = millis();  
  if (currentMillis - previousMillis >= beepInterval) {  
    previousMillis = currentMillis;  
    buzzerState = !buzzerState; 
    digitalWrite(BuzzerPin, buzzerState ? HIGH : LOW);
  }   
}  