/*
  Claude Status Indicator - ESP32 (USB Serial version)
  ------------------------------------------------------
  Drives 3 LEDs to show what Claude is doing:
    - WAITING    -> idle, waiting for you to type something
    - RUNNING    -> actively working (using a tool, thinking, etc.)
    - ATTENTION  -> needs you (permission prompt, error, or finished a task)

  No WiFi needed. The ESP32 just listens on its USB serial port for
  one word per line: "running", "waiting", or "attention".

  From your machine, a hook can set the state with a one-liner, e.g.:
    echo "running" > /dev/ttyUSB0        (Linux)
    echo "running" > /dev/cu.usbserial-0001   (macOS, adjust to your port)

  Wiring:
    PIN_WAITING   -> GREEN  LED + resistor (~220-330 ohm) -> GND
    PIN_ATTENTION -> YELLOW LED + resistor                -> GND
    PIN_RUNNING   -> RED    LED + resistor                -> GND
*/

// ---- CONFIG: edit these if your wiring differs ----
const int PIN_WAITING   = 3; // GREEN
const int PIN_RUNNING   = 4; // RED
const int PIN_ATTENTION = 5; // YELLOW
const int PIN_PIEZO     = 7; // BUZZER

const unsigned long BAUD_RATE = 115200;

String currentState = "waiting";
unsigned long lastSignalMillis = 0;
String inputBuffer = "";

void applyState(String state) {
  state.trim();
  state.toLowerCase();

  digitalWrite(PIN_WAITING, LOW);
  digitalWrite(PIN_RUNNING, LOW);
  digitalWrite(PIN_ATTENTION, LOW);

  if (state == "running") {
    digitalWrite(PIN_RUNNING, HIGH);
    currentState = "running";

  } else if (state == "running_alert") {
    tone(PIN_PIEZO, 500);
    digitalWrite(PIN_RUNNING, HIGH);
    currentState = "running_alert";
    delay(200);
    noTone(PIN_PIEZO);

  } else if (state == "attention") {
    digitalWrite(PIN_ATTENTION, HIGH);
    currentState = "attention";

  } else if (state == "attention_alert") {
    tone(PIN_PIEZO, 700);
    digitalWrite(PIN_ATTENTION, HIGH);
    currentState = "attention_alert";
    delay(200);
    noTone(PIN_PIEZO);

  } else if (state == "waiting") {
    tone(PIN_PIEZO, 1100);
    digitalWrite(PIN_WAITING, HIGH);
    currentState = "waiting";
    delay(500);
    noTone(PIN_PIEZO);
    
  } else if (state == "power") {
    digitalWrite(PIN_WAITING, HIGH);
    currentState = "power";
    
  } else if (state == "exit") {
    tone(PIN_PIEZO, 1200);
    delay(150);
    tone(PIN_PIEZO, 800);
    delay(150);
    tone(PIN_PIEZO, 400);
    delay(150);
    noTone(PIN_PIEZO);
    currentState = "exit";
    
  } else {
    // Unknown word on the line - ignore it, don't change state.
    return;
  }
  lastSignalMillis = millis();
}

void setup() {
  pinMode(PIN_WAITING, OUTPUT);
  pinMode(PIN_RUNNING, OUTPUT);
  pinMode(PIN_ATTENTION, OUTPUT);
  applyState("power");

  Serial.begin(BAUD_RATE);
  // Give the host a moment; not required but harmless.
  delay(200);
  Serial.println("Claude status indicator ready. Send: running | waiting | attention");

}

void loop() {
  // Read serial one line at a time so partial writes don't glitch the state.
  while (Serial.available() > 0) {
    char c = (char)Serial.read();
    if (c == '\n' || c == '\r') {
      if (inputBuffer.length() > 0) {
        applyState(inputBuffer);
        inputBuffer = "";
      }
    } else {
      inputBuffer += c;
      // Guard against runaway buffer growth if something writes garbage.
      if (inputBuffer.length() > 32) {
        inputBuffer = "";
      }
    }
  }

  // Safety net: if stuck on "running" with no update for 10 minutes,
  // fall back to waiting so a dropped connection doesn't lie forever.
  if (currentState == "running" && millis() - lastSignalMillis > 10UL * 60UL * 1000UL) {
    applyState("power");
  }

}
