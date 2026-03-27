#define PIR_PIN 2
#define LED_PIN 3

/* HC-SR501 Sensor 
Jumper (yellow) controls if sensor is activated once (L) or everytime (H) when seeing a movement.
When L it starts counting down after the first movement, when H after the last movement.

Resistor on the right (when you look from the front - the white halfsphere) controls the timer after a movement.
-> left is shorter, right is longer
Resistor on the left controls the distance in which it detects motion.

Both of them seem to not work properly, the timer does more or less.

Arduino - HC-SR501
VIN - VCC
GND - GND
D* - OUT/DATA
*/

void setup() {
  pinMode(PIR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);

  Serial.begin(9600);
  delay(3000);

  Serial.println("PIR test started...");
}

void loop() {
  bool value = digitalRead(PIR_PIN);

  if (value == 1) {
    Serial.println("ON");
    digitalWrite(LED_PIN, HIGH);
  } else {
    Serial.println("OFF");
    digitalWrite(LED_PIN, LOW);
  }

  delay(10);
}