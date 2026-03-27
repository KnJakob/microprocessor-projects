#include <FastLED.h>
#define LED_PIN 2
#define NUM_LEDS 119
 
CRGBArray<NUM_LEDS> leds;
    //leds[i] = CRGB(229, 68, 223); // Lila
    //leds[i] = CRGB(230, 50, 143); // Pink
    //leds[i] = CRGB(0,0,0); // Out

void setup() {
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 500);
  FastLED.clear();
  FastLED.show();
  pinMode(LED_BUILTIN, OUTPUT);
  FastLED.setBrightness(100);
}

void loop() {
  for(int i=0; i<NUM_LEDS; i++){
    leds[i] = CRGB(255,0,255); // Lila
    FastLED.show();
  }

}
