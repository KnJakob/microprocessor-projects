#include <FastLED.h>
#define LED_PIN 2
#define NUM_LEDS 119
 
CRGBArray<NUM_LEDS> leds;

void setup() {
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 500);
  FastLED.clear();
  FastLED.show();
  pinMode(LED_BUILTIN, OUTPUT);
  for(int i=0; i<NUM_LEDS; i++){
    leds[i] = CRGB(255,0,255); // Lila
  }
  FastLED.show();
}

void loop() {
  for(int i=0; i<NUM_LEDS; i++){
    FastLED.setBrightness(3*i);
    delay(25);
    FastLED.show();
  }
  for(int i=NUM_LEDS; i>=0; i--){
    FastLED.setBrightness(3*i);
    delay(25);
    FastLED.show();
  }

}
