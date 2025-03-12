
/* TWO LED AND TWO LED */

const int Button1Pin = 2; /* PIN FOR BUTTON 1 (TO TURN ON THE LEDS)  */  
const int Button2Pin = 1; /* PIN FOR BUTTON 2 (TO TURN ON THE LEDS)  */   
const int LEDPin1 = 5;     /* PIN FOR THE FIRST LED  */  
const int LEDPin2 = 6;     /* PIN FOR THE SECOND LED */  

/* DEBOUNCE TIMING VARIABLES  */  
unsigned long lastDebounceTime1 = 0;  
unsigned long lastDebounceTime2 = 0;  
unsigned long debounceDelay = 50; /* MILLISECONDS */   

/* CURRENT BUTTON STATES  */  
int button1State = HIGH;  /* INITIAL STATE FOR BUTTON 1 (NOT PRESSED) */   
int button2State = HIGH;  /* INITIAL STATE FOR BUTTON 2 (NOT PRESSED) */   
bool ledState = LOW;      /* INITIAL STATE FOR THE LEDS (OFF) */   

void setup() {  
  pinMode(Button1Pin, INPUT_PULLUP);  /* BUTTON 1 INPUT WITH INTERNAL PULL-UP RESISTOR  */  
  pinMode(Button2Pin, INPUT_PULLUP);  /* BUTTON 1 INPUT WITH INTERNAL PULL-UP RESISTOR  */  
  pinMode(LEDPin1, OUTPUT);           /* LED PIN 1 AS OUTPUT */   
  pinMode(LEDPin2, OUTPUT);           /* LED PIN 2 AS OUTPUT */  
  digitalWrite(LEDPin1, ledState);    /* INITIALIZE LED 1 STATE (OFF) */   
  digitalWrite(LEDPin2, ledState);    /* INITIALIZE LED 2 STATE (OFF) */  
}  

void loop() {  
  /* READ THE CURRENT STATE OF THE BUTTONS */   
  int reading1 = digitalRead(Button1Pin);  
  int reading2 = digitalRead(Button2Pin);  

  /* HANDLE BUTTON 1 (TURN ON LEDS)  */  
  if (reading1 != button1State) {  
    lastDebounceTime1 = millis(); /* RESET THE DEBOUNCE TIMER  */  
  }  
  
  if ((millis() - lastDebounceTime1) > debounceDelay) {  
    if (reading1 == LOW) { /* BUTTON 1 PRESSED  */  
      ledState = HIGH;     /* SET LED STATE TO ON */   
      digitalWrite(LEDPin1, ledState); /* UPDATE LED 1 */  
      digitalWrite(LEDPin2, ledState); /* UPDATE LED 2 */  
    }  
  }  
  
  /* HANDLE BUTTON 2 (TURN OFF LEDS) */  
  if (reading2 != button2State) {  
    lastDebounceTime2 = millis(); /* RESET THE DEBOUNCE TIMER  */   
  }  
  
  if ((millis() - lastDebounceTime2) > debounceDelay) {  
    if (reading2 == LOW) { /* BUTTON 2 PRESSED   */ 
      ledState = LOW;      /* SET LED STATE TO OFF */   
      digitalWrite(LEDPin1, ledState); /* UPDATE LED 1 */  
      digitalWrite(LEDPin2, ledState); /* UPDATE LED 2 */ 
    }  
  }  

  /* UPDATE BUTTON STATES FOR THE NEXT ITERATION  */  
  button1State = reading1;  
  button2State = reading2;  
}  