#include <FastLED.h>

#define FASTLED_ALLOW_INTERRUPTS 0

#define LED_PIN 2
#define PIR_PIN 4
#define NUM_LEDS 119

CRGBArray<NUM_LEDS> leds;

void setup() {
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 500);
  FastLED.clear();
  FastLED.show();
  FastLED.setBrightness(100);
  // Set LEDs as out and sensor as in
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PIR_PIN, INPUT);
  // Beginn communication between arduino board
  // and another computer
  Serial.begin(9600);
  // wait before first sensor reading (high sensitivity)
  delay(3000);
}

void loop() {
  bool pir = digitalRead(PIR_PIN);
  Serial.println(pir);

  if (pir == 1) {
    FastLED.setBrightness(80);
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB::Yellow2;
    }
  } else {
    FastLED.setBrightness(5);
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB::Azure;
    }
  }

  FastLED.show();
  delay(100);
}
