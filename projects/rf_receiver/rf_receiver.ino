#define RF_RECEIVE_PIN 2
#define MAX_PULSES 200
#define GAP_THRESHOLD 5000     // us - Pause die einen neuen Burst markiert
#define MIN_PULSES 10          // Mindestanzahl Pulse, damit ein Burst überhaupt zählt
#define SIMILARITY_TOLERANCE 150  // us - wie ähnlich zwei Pulse sein müssen
#define MATCHES_NEEDED 2       // wie viele ähnliche Bursts hintereinander nötig sind

volatile unsigned long lastTime = 0;
volatile int pulseIndex = 0;
volatile unsigned int pulses[MAX_PULSES];
volatile bool capturing = false;
volatile bool newBurst = false;
volatile int newBurstLength = 0;

unsigned int lastBurst[MAX_PULSES];
int lastBurstLength = 0;
int matchCount = 0;

void handleInterrupt() {
  unsigned long now = micros();
  unsigned long duration = now - lastTime;
  lastTime = now;

  if (duration > GAP_THRESHOLD) {
    if (pulseIndex >= MIN_PULSES) {
      newBurstLength = pulseIndex;
      newBurst = true;
    }
    pulseIndex = 0;
    capturing = true;
    return;
  }

  if (capturing && pulseIndex < MAX_PULSES) {
    pulses[pulseIndex++] = duration;
  }
}

bool burstsSimilar(unsigned int* a, int lenA, unsigned int* b, int lenB) {
  if (abs(lenA - lenB) > 2) return false; // Laenge muss fast gleich sein
  int minLen = min(lenA, lenB);
  int mismatches = 0;
  for (int i = 0; i < minLen; i++) {
    if (abs((int)a[i] - (int)b[i]) > SIMILARITY_TOLERANCE) mismatches++;
  }
  return mismatches < (minLen / 5); // max 20% Abweichung erlaubt
}

void setup() {
  Serial.begin(9600);
  pinMode(RF_RECEIVE_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(RF_RECEIVE_PIN), handleInterrupt, CHANGE);
  Serial.println("Gefilterter RF Sniffer bereit. Fernbedienung druecken...");
}

void loop() {
  if (newBurst) {
    unsigned int currentBurst[MAX_PULSES];
    int len = newBurstLength;
    noInterrupts();
    for (int i = 0; i < len; i++) currentBurst[i] = pulses[i];
    newBurst = false;
    interrupts();

    if (lastBurstLength > 0 && burstsSimilar(currentBurst, len, lastBurst, lastBurstLength)) {
      matchCount++;
    } else {
      matchCount = 0;
    }

    // Aktuellen Burst als "letzten" merken
    for (int i = 0; i < len; i++) lastBurst[i] = currentBurst[i];
    lastBurstLength = len;

    if (matchCount >= MATCHES_NEEDED) {
      Serial.println("=== ECHTES SIGNAL ERKANNT ===");
      Serial.print("Laenge: ");
      Serial.println(len);
      for (int i = 0; i < len; i++) {
        Serial.print(currentBurst[i]);
        Serial.print(" ");
      }
      Serial.println();
      Serial.println("==============================");
      matchCount = 0; // Reset, damit nicht dauerhaft spammt
    }
  }
}