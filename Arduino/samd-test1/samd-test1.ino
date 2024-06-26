int c = 0;

void setup() {
  Serial.begin(115200);
}

void loop() {
  Serial.print("test ");
  Serial.println(c);
  delay(1000);
  c++;
}
