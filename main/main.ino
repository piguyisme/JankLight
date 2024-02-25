#include <FastLED.h>

#define DATA_PIN 7
#define POT_PIN A0
#define NUM_LEDS 16

CRGB leds[NUM_LEDS];
int potVal = 0;

int circlePattern[8] = {1, 2, 4, 11, 13, 14, 8, 7};

/**
EEPROM Memory Addresses:
0: mode [rainbow, solid, breathe]
1: speed [256]
2: red
3: green
4: blue
**/

/**
Light indexes:
+ -- -- -- -- +
| 00 01 02 03 |
| 07 06 05 04 |
| 08 09 10 11 |
| 15 14 13 12 |
+ -- -- -- -- +
**/

void setup() {
  // put your setup code here, to run once:
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  Serial.begin(9600);
}

int i = 0;
int mode = 6;
void loop() {
  if (mode == 0) { //Rainbow
    i = i % 255;
    // Set all LEDs to red
    fill_solid(leds, NUM_LEDS, CHSV(i, 255, 255));
    FastLED.show();
    ++i;
    delay(50);
  } else if (mode == 4) {
    i = i % 16;
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    leds[i] = CRGB::White;
    FastLED.show();
    delay(100);
    ++i;
  } else if (mode == 5) { //Color Sections
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    leds[1] = CRGB::Red;
    leds[2] = CRGB::Red;
    leds[4] = CRGB::Green;
    leds[11] = CRGB::Green;
    leds[7] = CRGB::Yellow;
    leds[8] = CRGB::Yellow;
    leds[14] = CRGB::Blue;
    leds[13] = CRGB::Blue;
    FastLED.show();
    delay(1000);
  } else if (mode == 6) {
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    i = i % 8;
    int redLed = circlePattern[i];
    int blueLed = circlePattern[(i + 4) % 8];
    Serial.print(redLed); Serial.print(" : "); Serial.print(i); Serial.print("\n");
    leds[redLed] = CRGB::Green;
    leds[blueLed] = CRGB::Orange;
    FastLED.show();
    ++i;
    delay(50);
  }
}