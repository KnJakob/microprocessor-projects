#define RF_SEND_PIN 3

// Geglaettete/quantisierte Codes (kurz ~400us, lang ~1220us)
unsigned int codeAnAus[] = {402, 1219, 402, 1219, 1219, 402, 1219, 402, 402, 1219, 1219, 402, 1219, 402, 1219, 402, 402, 1219, 1219, 402, 402, 1219, 1219, 402, 1219, 402, 1219, 402, 402, 1219, 402, 1219, 402, 1219, 402, 1219, 402, 1219, 402, 1219, 402, 1219, 402, 1219, 1219, 402, 1219, 402, 402};
//unsigned int codeAnAus[]   = {401,1218,401,1218,1218,401,1218,401,401,1218,1218,401,1218,401,1218,401,401,1218,1218,401,401,1218,1218,401,1218,401,1218,401,401,1218,401,1218,401,1218,401,1218,401,1218,401,1218,401,1218,401,1218,1218,401,1218,401,401};

unsigned int codeHeller[]  = {401,1217,401,1217,1217,401,1217,401,401,1217,1217,401,1217,401,1217,401,401,1217,1217,401,401,1217,1217,401,1217,401,1217,401,401,1217,401,1217,401,1217,401,1217,401,1217,401,1217,401,1217,1217,401,401,1217,1217,401,401};

unsigned int codeDunkler[] = {403,1215,403,1215,1215,403,1215,403,403,1215,1215,403,1215,403,1215,403,403,1215,1215,403,403,1215,1215,403,1215,403,1215,403,403,1215,403,1215,403,1215,403,1215,403,1215,403,1215,1215,403,403,1215,1215,403,1215,403,403};

void sendRawRF(unsigned int* durations, int len, int repeats) {
  noInterrupts();
  for (int r = 0; r < repeats; r++) {
    bool level = HIGH;
    for (int i = 0; i < len; i++) {
      digitalWrite(RF_SEND_PIN, level);
      delayMicroseconds(durations[i]);
      level = !level;
    }
    digitalWrite(RF_SEND_PIN, LOW);
    delayMicroseconds(10000); // Pause zwischen Wiederholungen
  }
  interrupts();
}

void setup() {
  Serial.begin(9600);
  pinMode(RF_SEND_PIN, OUTPUT);
  digitalWrite(RF_SEND_PIN, LOW);
  Serial.println("RF Sender bereit. Befehle: an, heller, dunkler");
}

void loop() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    cmd.toLowerCase();

    if (cmd == "an") {
      sendRawRF(codeAnAus, sizeof(codeAnAus)/sizeof(codeAnAus[0]), 8);
      Serial.println("Gesendet: An/Aus");
    }
    else if (cmd == "heller") {
      sendRawRF(codeHeller, sizeof(codeHeller)/sizeof(codeHeller[0]), 8);
      Serial.println("Gesendet: Heller");
    }
    else if (cmd == "dunkler") {
      sendRawRF(codeDunkler, sizeof(codeDunkler)/sizeof(codeDunkler[0]), 8);
      Serial.println("Gesendet: Dunkler");
    }
    else {
      Serial.println("Unbekannter Befehl!");
    }
  }
}
