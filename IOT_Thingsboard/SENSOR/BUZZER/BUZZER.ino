const int BuzzerPin = 7; // Pin for the buzzer  

// Variables to hold timing  
unsigned long previousMillis = 0;  

// Interval for buzzer beep (in milliseconds)  
const long beepInterval = 1000; // Buzzer beep every 1000 milliseconds (1 second)  
bool buzzerState = false; // Current state of the buzzer  

void setup() {  
  pinMode(BuzzerPin, OUTPUT); // Set the buzzer pin as an output  
}  

void loop() {  
  // Get the current time  
  unsigned long currentMillis = millis();  

  // Check if it's time to change the buzzer state  
  if (currentMillis - previousMillis >= beepInterval) {  
    previousMillis = currentMillis; // Save the time when the beep state changed  
    buzzerState = !buzzerState; // Toggle buzzer state  
    digitalWrite(BuzzerPin, buzzerState ? HIGH : LOW); // Set buzzer state  
  }  

  // Here you can add other code that needs to run while the buzzer is active  
}  