#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

const uint16_t kIrLed = 4;  // ESP8266 GPIO pin to use. Recommended: 4 (D2).

IRsend irsend(kIrLed);  // Set the GPIO to be used to sending the message.

void setup() {
  irsend.begin();
  Serial.begin(115200, SERIAL_8N1);
}

// sendLG2 (uint64_t data, uint16_t nbits=kLgBits, uint16_t repeat=kNoRepeat)

void loop() {
  long value = 0;

  if (Serial.available()) {
    value = Serial.parseInt();
  }

  if (value == 1) {
    Serial.println("TURN ON");
    irsend.sendLG2(0x8800B0B); //ON
    Serial.println("LG2 Turn On");
  }
  if (value == 2) {
    Serial.println("TURN OFF");
    irsend.sendLG2(0x88C0051); //OFF
    Serial.println("LG2 Turn Off");
  }
  if (value == 3)
  {
    Serial.println("UP");
    
    irsend.sendLG2(0x8808C04); //UP
    Serial.println("LG2 Up");
  }
  if (value == 4)
  {
    Serial.println("DOWN");
    irsend.sendLG2(0x8808B03); //DOWN
    Serial.println("LG2 Down");
  }
}
