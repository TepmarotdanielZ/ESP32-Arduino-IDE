#include <FastLED.h>  

#define LED_PIN    4  
#define NUM_LEDS   8  

CRGB leds[NUM_LEDS];  

/* VARIABLES TO HOLD TIMING  */  
unsigned long previousMillis = 0;  

/* INTERVAL FOR COLOR CHANGE (IN MILLISECONDS) */   
const long colorChangeInterval = 1000; /* CHANGE COLOR EVERY SECOND  */  
int currentColor = 0;                  /* INDEX FOR COLORS ARRAY  */  

/* ARRAY OF COLORS FOR THE RGB LED  */  
CRGB colors[] = {  
  CRGB::Red,  
  CRGB::Green,  
  CRGB::Blue,  
  CRGB::Yellow,  
  CRGB::Cyan,  
  CRGB::Magenta,  
  CRGB::White,  
  CRGB::Orange  
};  

void setup() {  
  /* INITIALIZE THE LED STRIP   */ 
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);  
  FastLED.clear(); /* CLEAR THE LEDS TO START */   
  FastLED.show();  /* UPDATE THE LED STRIP  */ 
}  

void loop() {  
  /* GET THE CURRENT TIME  */  
  unsigned long currentMillis = millis();  

  /* CHECK IF IT'S TIME TO CHANGE THE COLOR  */  
  if (currentMillis - previousMillis >= colorChangeInterval) {  
    previousMillis = currentMillis; /* SAVE THE LAST TIME THE COLOR CHANGED */   

    /* SET THE NEW COLOR FOR ALL LEDS  */  
    fill_solid(leds, NUM_LEDS, colors[currentColor]);  
    FastLED.show(); /* UPDATE THE LEDS */   

    /* MOVE TO THE NEXT COLOR  */  
    currentColor++;  
    if (currentColor >= (sizeof(colors) / sizeof(colors[0]))) {  
      currentColor = 0; /* RESET TO THE FIRST COLOR IF AT THE END OF THE ARRAY  */  
    }  
  }  
}  