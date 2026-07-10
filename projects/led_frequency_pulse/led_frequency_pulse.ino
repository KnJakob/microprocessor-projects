// ============================================================
// Musikreaktiver LED-Streifen mit Frequenzspektrum
// Arduino Nano (klassisch, ATmega328) + MAX4466 (Mikrofon) + WS2812 LED-Streifen
// ============================================================
// Benötigte Bibliotheken (über Library Manager installieren):
//   - FastLED
//   - arduinoFFT (Version 2.x, von Enrique Condes)
//
// WICHTIG: Der klassische Nano hat nur 2KB RAM und einen 10-Bit-ADC.
// Daher ist die FFT-Größe hier viel kleiner als bei einer ESP32-Version.
// Nyquist-Grenze bei 4000Hz Sampling = 2000Hz -> sehr hohe Höhen (Becken,
// Hi-Hats) werden dadurch nur schwach erfasst, Bässe/Mitten funktionieren gut.
// ============================================================

#include <FastLED.h>
#include <arduinoFFT.h>

// ---------------- Konfiguration ----------------
#define LED_PIN     2
#define NUM_LEDS    100      // <-- HIER die exakte Anzahl deiner LEDs eintragen
#define BRIGHTNESS  80       // 0-255, begrenzt Helligkeit (Stromverbrauch!)
#define MIC_PIN     A0

#define SAMPLES         64     // klein gehalten wegen 2KB RAM-Limit
#define SAMPLING_FREQ   4000   // Hz -> Nyquist-Grenze 2000Hz
#define NUM_BANDS       8      // weniger Bänder, da weniger Frequenzauflösung

// ---------------- Globale Variablen ----------------
CRGB leds[NUM_LEDS];

float vReal[SAMPLES];
float vImag[SAMPLES];
ArduinoFFT<float> FFT = ArduinoFFT<float>(vReal, vImag, SAMPLES, SAMPLING_FREQ);

float bandValues[NUM_BANDS];   // geglättete Anzeigewerte je Band
const float attack = 0.6;      // wie schnell ein Band ansteigt (0-1)
const float decay  = 0.08;     // wie schnell ein Band abfällt (0-1)
const int noiseFloor = 6;      // Rauschschwelle - kleiner als bei ESP32-Version,
                                // weil der 10-Bit-ADC kleinere Rohwerte liefert

int bandBinStart[NUM_BANDS];
int bandBinEnd[NUM_BANDS];

// ============================================================
void setup() {
  Serial.begin(115200);

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();
  FastLED.show();

  // Kein analogReadResolution() nötig/verfügbar auf AVR - Standard ist 10-Bit (0-1023)

  calculateBandRanges();
}

// Berechnet, welche FFT-Bins zu welchem Band gehören (logarithmisch verteilt).
void calculateBandRanges() {
  int minBin = 1;
  int maxBin = SAMPLES / 2;

  float logMin = log((float)minBin);
  float logMax = log((float)maxBin);
  float step = (logMax - logMin) / NUM_BANDS;

  for (int i = 0; i < NUM_BANDS; i++) {
    bandBinStart[i] = (int)round(exp(logMin + step * i));
    bandBinEnd[i]   = (int)round(exp(logMin + step * (i + 1)));
    if (bandBinEnd[i] <= bandBinStart[i]) {
      bandBinEnd[i] = bandBinStart[i] + 1;
    }
  }
}

// Sampelt SAMPLES Werte vom Mikrofon in exakt getaktetem Intervall
// und entfernt den DC-Offset (Signal schwingt sonst um ~512 statt um 0).
void sampleAudio() {
  unsigned long sampleDelayUs = (unsigned long)round(1000000.0 / SAMPLING_FREQ);
  long sum = 0;

  for (int i = 0; i < SAMPLES; i++) {
    unsigned long start = micros();

    int raw = analogRead(MIC_PIN);
    vReal[i] = raw;
    vImag[i] = 0;
    sum += raw;

    while (micros() - start < sampleDelayUs) {
      // warten, bis exaktes Sample-Intervall erreicht ist
    }
  }

  float mean = (float)sum / SAMPLES;
  for (int i = 0; i < SAMPLES; i++) {
    vReal[i] -= mean;
  }
}

void computeFFT() {
  FFT.windowing(FFTWindow::Hamming, FFTDirection::Forward);
  FFT.compute(FFTDirection::Forward);
  FFT.complexToMagnitude();
}

// Fasst FFT-Bins zu Bändern zusammen und glättet die Werte (Attack/Decay).
void updateBands() {
  for (int b = 0; b < NUM_BANDS; b++) {
    float bandSum = 0;
    int count = 0;

    for (int bin = bandBinStart[b]; bin < bandBinEnd[b] && bin < SAMPLES / 2; bin++) {
      bandSum += vReal[bin];
      count++;
    }

    float avg = (count > 0) ? (bandSum / count) : 0;
    if (avg < noiseFloor) avg = 0;

    if (avg > bandValues[b]) {
      bandValues[b] += (avg - bandValues[b]) * attack;
    } else {
      bandValues[b] += (avg - bandValues[b]) * decay;
    }
  }
}

// Zeichnet die Bänder auf den Streifen: Bässe an einem Ende (warmes Rot/Orange),
// Höhen am anderen Ende (kühles Blau/Violett), Helligkeit = Lautstärke des Bands.
void renderToStrip() {
  int ledsPerBand = NUM_LEDS / NUM_BANDS;

  for (int b = 0; b < NUM_BANDS; b++) {
    // Skalierungsfaktor höher als bei der ESP32-Version, da die Rohwerte
    // durch den 10-Bit-ADC kleiner ausfallen
    int brightness = constrain((int)(bandValues[b] * 4.0), 0, 255);

    uint8_t hue = map(b, 0, NUM_BANDS - 1, 0, 200); // 0=Rot ... 200=Blau/Violett
    CRGB color = CHSV(hue, 255, brightness);

    int startLed = b * ledsPerBand;
    int endLed = startLed + ledsPerBand;
    for (int i = startLed; i < endLed && i < NUM_LEDS; i++) {
      leds[i] = color;
    }
  }

  FastLED.show();
}

// ============================================================
void loop() {
  sampleAudio();
  computeFFT();
  updateBands();
  renderToStrip();
}
