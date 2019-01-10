#include <IRremote.h>

IRsend irsend;

const int RFPin = 3;
const int led = 13;

char inData[20]; // Allocate some space for the string
char inChar=-1; // Where to store the character read
byte index = 0; // Index into array; where to store the character

void setup() {
  pinMode(RFPin, OUTPUT);
  pinMode(led, OUTPUT);
  Serial.begin(9600);
  Serial.println("Waiting...");
         Serial.println("SEND CODE");
       irsend.sendNEC(0x807FC03F,32);
}

void loop() {
  /*
  if (stringComplete) {
    Serial.println(inputString);
    if (inputString == "s") {
       digitalWrite(led, HIGH);
       Serial.println("SEND CODE");
       irsend.sendNEC(0x807FC03F,32);
       delay(500);
       digitalWrite(led, LOW);
    }
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
  */

}
