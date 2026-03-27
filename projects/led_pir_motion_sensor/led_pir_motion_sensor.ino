#include <FastLED.h>

#define FASTLED_ALLOW_INTERRUPTS 0

#define LED_PIN 2
#define PIR_PIN 4
#define NUM_LEDS 119

CRGBArray<NUM_LEDS> leds;

int lightChaser;

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
  lightChaser = 0;
}

void loop() {
  bool pir = digitalRead(PIR_PIN);
  Serial.println(pir);

  if (pir == 1) {
    lightChaser++;
    lightChaser = lightChaser % NUM_LEDS;
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB::Yellow2;
      if(i == lightChaser){
          leds[i] = CRGB::Black;
      }else if(i == lightChaser -1){
          FastLED.setBrightness(20);
      }else if(i == lightChaser -2){
          FastLED.setBrightness(50);
      }else if(i == lightChaser +1){
          FastLED.setBrightness(20);
      }else if(i == lightChaser +2){
          FastLED.setBrightness(50);
      }
    }
  } else {
    lightChaser = 0;
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB::Black;
    }
  }

  FastLED.show();
  delay(50);
}
