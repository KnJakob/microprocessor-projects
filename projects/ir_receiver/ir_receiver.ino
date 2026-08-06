#include <IRremote.hpp>

#define IR_RECEIVE_PIN 2  // Use a digital pin; pin 2 or 3 preferred (supports interrupts)

void setup() {
  Serial.begin(9600);

  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  Serial.println("IR Receiver ready. Point a remote at it and press buttons...");
}

void loop() {
  if (IrReceiver.decode()) {
    Serial.println("---- Raw Array (fuer sendRaw) ----");
    IrReceiver.printIRResultRawFormatted(&Serial, true); // gibt rawData[] in µs aus
    Serial.print("Protocol: ");
    Serial.print(getProtocolString(IrReceiver.decodedIRData.protocol));

    Serial.print(" | Address: 0x");
    Serial.print(IrReceiver.decodedIRData.address, HEX);

    Serial.print(" | Command: 0x");
    Serial.print(IrReceiver.decodedIRData.command, HEX);

    Serial.print(" | Raw: 0x");
    Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);

    Serial.println("---- Fertiges Array (copy-paste) ----");
    IrReceiver.printIRResultAsCArray(&Serial);
    Serial.println();

    IrReceiver.resume();
  }
}