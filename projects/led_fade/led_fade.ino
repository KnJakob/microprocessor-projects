#include <FastLED.h>
#define LED_PIN 2
#define NUM_LEDS 119

CRGB leds[NUM_LEDS];

void setup() {
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 500);
  FastLED.setBrightness(50);
  FastLED.clear();
  FastLED.show();
  //pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  for(int i=0; i<NUM_LEDS; i++){
    leds[i] = CRGB(255-i*2, i*2, 255);
    FastLED.show();
  }

  FastLED.show();
}
