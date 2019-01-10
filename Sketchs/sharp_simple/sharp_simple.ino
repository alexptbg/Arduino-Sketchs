#define sensorIR A5

float sensorValue;
float cm;

void setup() {
  Serial.begin(9600);
}

void loop() {
  sensorValue = analogRead(sensorIR);
  cm = 10650.08 * pow(sensorValue,-0.935) - 10;
  delay(500);
  Serial.print("CM: ");
  Serial.println(cm);
}
