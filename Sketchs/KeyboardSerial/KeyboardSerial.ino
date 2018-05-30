#include <SoftwareSerial.h>
#include "Keyboard.h"

SoftwareSerial mySerial(10,11); //RX,TX

const int ledPin = 13;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(38400);
  /*
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  */
  Serial.println("KEYEMUBYALEX");
  // set the data rate for the SoftwareSerial port
  mySerial.begin(38400);
  mySerial.println("KEYEMUBYALEX");
  pinMode(ledPin, OUTPUT);
  Keyboard.begin();
}

void loop() {
  if (mySerial.available()) {
    char inChar = mySerial.read();
    Serial.write(inChar);
    //Keyboard.write(inChar);
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }
  if (Serial.available()) {
    mySerial.write(Serial.read());
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }
}

