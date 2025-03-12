const int LED1 = 5;  /* GPIO 5 */
const int LED2 = 6;  /* GPIO 6 */

unsigned long previousMillis1 = 0; /* WILL STORE THE LAST TIME LED1 WAS UPDATED  */  
unsigned long previousMillis2 = 0; /* WILL STORE THE LAST TIME LED2 WAS UPDATED  */   
const long interval1 = 1000; /* INTERVAL AT WHICH TO BLINK LED1 (MILLISECONDS) */   
const long interval2 = 500;  /* INTERVAL AT WHICH TO BLINK LED2 (MILLISECONDS) */   

void setup() {  
  pinMode(LED1, OUTPUT);  
  pinMode(LED2, OUTPUT);  
}  

void loop() {  
  unsigned long currentMillis = millis();  

  /* CHECK IF IT'S TIME TO TOGGLE LED1 */   
  if (currentMillis - previousMillis1 >= interval1) {  
    previousMillis1 = currentMillis; /* SAVE THE LAST TIME LED1 WAS UPDATED  */  
    digitalWrite(LED1, !digitalRead(LED1)); /* TOGGLE LED1 */   
  }  

  /* CHECK IF IT'S TIME TO TOGGLE LED2 */   
  if (currentMillis - previousMillis2 >= interval2) {  
    previousMillis2 = currentMillis; /* SAVE THE LAST TIME LED2 WAS UPDATED  */    
    digitalWrite(LED2, !digitalRead(LED2)); /* TOGGLE LED2 */  
  }  
}  