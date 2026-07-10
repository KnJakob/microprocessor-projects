int buzzerPin = 7;
int i;

void setup() {
  pinMode(buzzerPin, OUTPUT);
  i = 0;
}

void loop() {
  i += 50;
  tone(buzzerPin, i);  // play a 1000 Hz tone
  delay(500);
  noTone(buzzerPin);      // stop
  delay(500);
}
