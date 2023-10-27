#include <FastLED.h>

#define LED_TYPE WS2812
#define NUM_LEDS 63
#define DATA_PIN 8
#define MILLI_AMPS 2400
#define BRIGHTNESS 64

CRGB LEDs[NUM_LEDS];

// Settings //
unsigned long countdownDuration = 60000;
unsigned long startTime;
unsigned long currentTime;
unsigned long previousTime = 0; // Will store last time display was updated
CRGB countdownColor = CRGB::Green;

// Each row represents the different digits the display can make. Each column is the induvidual LED that makes up the digit.
const int digits [10][21] = {
  {0, 1, 2, 3, 4, 5, 6, 7, 8, 20, 19, 18, 17, 16, 15, 14, 13, 12},            // 0
  {0, 1, 2, 12, 13, 14},                                                      // 1
  {5, 4, 3, 2, 1, 0, 11, 10, 9, 20, 19, 18, 17, 16, 15},                      // 2
  {5, 4, 3, 2, 1, 0, 9, 10, 11, 12, 13, 14, 15, 16, 17},                      // 3
  {6, 7, 8, 9, 10, 11, 2, 1, 0, 12, 13, 14},                                  // 4
  {3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17},                      // 5
  {3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20},          // 6
  {5, 4, 3, 2, 1, 0, 12, 13, 14},                                             // 7
  {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20}, // 8
  {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}                          // 9
};

const int arrLen [] = {18, 6, 15, 15, 12, 15, 18, 9, 21, 15}; // Length of each digit array

void setup() {
  FastLED.addLeds<LED_TYPE, DATA_PIN, GRB>(LEDs, NUM_LEDS);  
  FastLED.setDither(false);
  FastLED.setCorrection(TypicalLEDStrip);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, MILLI_AMPS);
  FastLED.setBrightness(BRIGHTNESS);

  allBlank();
  startTime = millis();
}

void loop() {
  currentTime = millis(); // Store the current time

  if (currentTime - previousTime >= 1000) {
    previousTime = currentTime;
    updateCountdown();
    FastLED.show();
  } 
}

void displayNumber(byte number, byte segment, CRGB color) {
  /*

      __ __ __        __ __ __        _5 _4 _3  
    __        __    __        __    _6        _2
    __        __    __        __    _7        _1
    __        42    __        21    _8        _0
      __ __ __        __ __ __        _9 10 11  
    __        __    __        __    20        12
    __        __    __        __    19        13
    __        __    __        __    18        14
      __ __ __        __ __ __        17 16 15  

  */
 
  // segment from left to right: 2, 1, 0
  int startindex = 0;
  switch (segment) {
    case 0:
      startindex = 0;
      break;
    case 1:
      startindex = 21;
      break;
    case 2:
      startindex = 42;
      break; 
  }

  for(int i = 0; i < arrLen[number]; i++) {
    int shiftedPosition = digits[number][i] + startindex;
    LEDs[shiftedPosition] = color;
  }
}

void allBlank() {
  for (int i = 0; i < NUM_LEDS; i++) {
    LEDs[i] = CRGB::Black;
  }
  FastLED.show();
}

void blink() {
  for (int i = 0; i <= 5; i++) {
  fill_solid(LEDs, NUM_LEDS, CRGB::Red);
  FastLED.show();
  delay(1000);
  allBlank();
  delay(500);
  }
}

void endCountdown() {
  allBlank();
  for (int i = 0; i < NUM_LEDS; i++) {
    if (i > 0)
      LEDs[i-1] = CRGB::Black;
      LEDs[i] = CRGB::Red;
      FastLED.show();
      delay(25);
  }
  blink();
}

void updateCountdown () {
  if (countdownDuration == 0)
    return;

  unsigned long elapsedTime = currentTime - startTime;
  unsigned long remainingTime = countdownDuration - elapsedTime;

  unsigned long hours = ((remainingTime / 1000) / 60) / 60;
  unsigned long minutes = ((remainingTime / 1000) / 60);
  unsigned long seconds = remainingTime / 1000;
  int remMinutes = minutes - (hours * 60);
  int remSeconds = seconds - (minutes * 60);

  int h1 = hours / 10;
  int h2 = hours % 10;
  int m1 = remMinutes / 10;
  int m2 = remMinutes % 10;  
  int s1 = remSeconds / 10;
  int s2 = remSeconds % 10;

  if (remainingTime <= 60000) {
    countdownColor = CRGB::Red;
  }

  allBlank();

  if (hours > 0) {
    // hh:mm
    displayNumber(h2,2,countdownColor);
    displayNumber(m1,1,countdownColor);
    displayNumber(m2,0,countdownColor);  
  } else if (minutes > 0) {
    // mm:ss   
    displayNumber(m2,2,CRGB::Blue);
    displayNumber(s1,1,countdownColor);
    displayNumber(s2,0,countdownColor);  
  } else {
    // x:ss
    displayNumber(s1,1,countdownColor);
    displayNumber(s2,0,countdownColor);
  }

  if (hours <= 0 && remMinutes <= 0 && remSeconds <= 0) {
    endCountdown();
    countdownDuration = 0;
    return;
  }
}