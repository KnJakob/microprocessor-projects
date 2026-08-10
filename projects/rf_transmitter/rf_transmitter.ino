#define RF_SEND_PIN 2  // MX-FS-03V DATA-Pin

unsigned int codeAnAus[] = {400,1216,396,1224,1208,432,1176,456,340,1256,1188,440,1180,444,1180,440,384,1232,1192,436,384,1236,1188,428,1196,436,1192,432,372,1240,368,1256,360,1256,356,1252,360,1256,364,1248,376,1244,372,1232,1212,416,1196,436,396};

unsigned int codeHeller[] = {380,1236,388,1224,1204,424,1204,420,388,1228,1184,452,1176,460,1156,460,356,1248,1192,428,380,1232,1200,428,1188,432,1188,444,368,1236,380,1244,364,1252,364,1244,356,1264,356,1260,356,1244,1184,444,368,1244,1196,436,396};

unsigned int codeDunkler[] = {372,1244,380,1220,1212,408,1204,416,388,1224,1200,428,1192,432,1184,436,380,1236,1184,452,348,1284,1148,452,1172,444,1188,436,376,1236,372,1240,376,1236,376,1252,364,1244,364,1240,1188,432,364,1260,1164,464,1160,464,368};

void sendRawRF(unsigned int* durations, int len, int repeats) {
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
      sendRawRF(codeAnAus, sizeof(codeAnAus)/sizeof(codeAnAus[0]), 5);
      Serial.println("Gesendet: An/Aus");
    }
    else if (cmd == "heller") {
      sendRawRF(codeHeller, sizeof(codeHeller)/sizeof(codeHeller[0]), 5);
      Serial.println("Gesendet: Heller");
    }
    else if (cmd == "dunkler") {
      sendRawRF(codeDunkler, sizeof(codeDunkler)/sizeof(codeDunkler[0]), 5);
      Serial.println("Gesendet: Dunkler");
    }
    else {
      Serial.println("Unbekannter Befehl!");
    }
  }
}