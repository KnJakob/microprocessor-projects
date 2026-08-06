#include <IRremote.hpp>
#define IR_SEND_PIN 4

// example remote
uint16_t codeHoch[]   = {330,620, 280,320, 630,320, 280,670, 280,320, 630,620, 630,620, 280,320, 630,670, 580,670, 280,320, 630};
uint16_t codeRunter[] = {280,670, 280,320, 580,320, 330,620, 330,320, 580,670, 580,670, 280,320, 630,620, 630,320, 280,670, 580};
uint16_t codeAnAus[] = {280,620, 280,320, 630,320, 330,620, 280,320, 630,670, 580,670, 280,320, 280,320, 630,670, 580,320, 330,620, 280};
uint16_t codeLauter[] = {330,620, 280,320, 630,320, 280,670, 280,320, 630,620, 630,620, 280,320, 630,670, 280,320, 630,320, 280,670, 280};
uint16_t codeLeiser[] = {280,620, 280,320, 630,320, 330,620, 280,320, 630,670, 580,670, 280,320, 630,620, 630,620, 630,620, 280};


void sendRawCode(uint16_t* data, uint8_t len, const char* name) {
  Serial.print("Sende: ");
  Serial.println(name);
  IrSender.sendRaw(data, len, 38); // 38kHz Traegerfrequenz
}

void setup() {
  Serial.begin(9600);
  IrSender.begin(IR_SEND_PIN);
  Serial.println("Bereit. Befehle: an, lauter, leiser, hoch, runter");
}

void loop() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    cmd.toLowerCase();

    if (cmd == "an")
      sendRawCode(codeAnAus, sizeof(codeAnAus)/sizeof(codeAnAus[0]), "An/Aus");
    else if (cmd == "lauter")
      sendRawCode(codeLauter, sizeof(codeLauter)/sizeof(codeLauter[0]), "Lauter");
    else if (cmd == "leiser")
      sendRawCode(codeLeiser, sizeof(codeLeiser)/sizeof(codeLeiser[0]), "Leiser");
    else if (cmd == "hoch")
      sendRawCode(codeHoch, sizeof(codeHoch)/sizeof(codeHoch[0]), "Hoch");
    else if (cmd == "runter")
      sendRawCode(codeRunter, sizeof(codeRunter)/sizeof(codeRunter[0]), "Runter");
    else
      Serial.println("Unbekannter Befehl!");
  }
}