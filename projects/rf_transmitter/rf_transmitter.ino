/*#define RF_SEND_PIN 2  // MX-FS-03V DATA-Pin

unsigned int codeAnAus[]   = {401,1218,401,1218,1218,401,1218,401,401,1218,1218,401,1218,401,1218,401,401,1218,1218,401,401,1218,1218,401,1218,401,1218,401,401,1218,401,1218,401,1218,401,1218,401,1218,401,1218,401,1218,401,1218,1218,401,1218,401,401};

unsigned int codeHeller[]  = {401,1217,401,1217,1217,401,1217,401,401,1217,1217,401,1217,401,1217,401,401,1217,1217,401,401,1217,1217,401,1217,401,1217,401,401,1217,401,1217,401,1217,401,1217,401,1217,401,1217,401,1217,1217,401,401,1217,1217,401,401};

unsigned int codeDunkler[] = {403,1215,403,1215,1215,403,1215,403,403,1215,1215,403,1215,403,1215,403,403,1215,1215,403,403,1215,1215,403,1215,403,1215,403,403,1215,403,1215,403,1215,403,1215,403,1215,403,1215,1215,403,403,1215,1215,403,1215,403,403};

void sendRawRF(unsigned int* durations, int len, int repeats) {
  noInterrupts();  // Timing-Praezision sicherstellen
  for (int r = 0; r < repeats; r++) {
    bool level = HIGH;
    for (int i = 0; i < len; i++) {
      digitalWrite(RF_SEND_PIN, level);
      delayMicroseconds(durations[i]);
      level = !level;
    }
    digitalWrite(RF_SEND_PIN, LOW);
    delayMicroseconds(10000);
  }
  interrupts();  // wieder aktivieren
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
      sendRawRF(codeAnAus, sizeof(codeAnAus)/sizeof(codeAnAus[0]), 10);
      Serial.println("Gesendet: An/Aus");
    }
    else if (cmd == "heller") {
      sendRawRF(codeHeller, sizeof(codeHeller)/sizeof(codeHeller[0]), 10);
      Serial.println("Gesendet: Heller");
    }
    else if (cmd == "dunkler") {
      sendRawRF(codeDunkler, sizeof(codeDunkler)/sizeof(codeDunkler[0]), 10);
      Serial.println("Gesendet: Dunkler");
    }
    else {
      Serial.println("Unbekannter Befehl!");
    }
  }
}*/