#include <IRremote.h>

int RECV_PIN = 2;
IRrecv irrecv(RECV_PIN);
decode_results results;

void setup() {
  Serial.begin(9600);
  Serial.println("Starting...");
  irrecv.enableIRIn();
  Serial.println("Waiting...");
}

void loop() {
  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    irrecv.resume();
  }
}
