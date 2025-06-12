const int Button1Pin = 1; /* PIN FOR BUTTON 1 */   
const int Button2Pin = 2; /* PIN FOR BUTTON 2 */ 
const int RelayPin = 37;  /* PIN FOR THE RELAY  */  

/* TIMING VARIABLES FOR DEBOUNCING  */  
unsigned long lastDebounceTime1 = 0;  
unsigned long lastDebounceTime2 = 0;  
unsigned long debounceDelay = 50; /* MILLISECONDS */   

/* CURRENT BUTTON STATES   */ 
int button1State = HIGH; /* INITIAL STATE (HIGH MEANS NOT PRESSED) */   
int button2State = HIGH; /* INITIAL STATE (HIGH MEANS NOT PRESSED) */   
int relayState = LOW;    /* RELAY INITIAL STATE */   

void setup() {  
  pinMode(Button1Pin, INPUT_PULLUP);  /* BUTTON 1 INPUT WITH INTERNAL PULL-UP RESISTOR  */  
  pinMode(Button2Pin, INPUT_PULLUP);  /* BUTTON 2 INPUT WITH INTERNAL PULL-UP RESISTOR  */  
  pinMode(RelayPin, OUTPUT);          /* RELAY PIN AS OUTPUT  */  
  digitalWrite(RelayPin, relayState); /* INITIALIZE RELAY STATE */   
}  

void loop() {  
  /* READ BUTTON STATES */  
  int reading1 = digitalRead(Button1Pin);  
  int reading2 = digitalRead(Button2Pin);  

  /* HANDLE BUTTON 1 (TURN ON RELAY)  */  
  if (reading1 != button1State) {  
    lastDebounceTime1 = millis(); /* RESET THE DEBOUNCE TIMER  */  
  }  
  
  if ((millis() - lastDebounceTime1) > debounceDelay) {  
    if (reading1 == LOW) { /* BUTTON PRESSED  */  
      relayState = HIGH;   /* SET RELAY ON */   
      digitalWrite(RelayPin, relayState);  
    }  
  }  
  
  /* HANDLE BUTTON 2 (TURN ON RELAY)  */   
  if (reading2 != button2State) {  
    lastDebounceTime2 = millis(); /* RESET THE DEBOUNCE TIMER  */  
  }  
  
  if ((millis() - lastDebounceTime2) > debounceDelay) {  
    if (reading2 == LOW) { /* BUTTON PRESSED  */  
      relayState = LOW;    /* SET RELAY OFF */  
      digitalWrite(RelayPin, relayState);  
    }  
  }  

  /* UPDATE BUTTON STATES  */  
  button1State = reading1;  
  button2State = reading2;  
}  