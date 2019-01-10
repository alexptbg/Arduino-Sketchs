
void setup() {
  pinMode (2,OUTPUT);
  pinMode (5,OUTPUT);
  Serial.begin(9600);
}

void loop()
{
digitalWrite(2, HIGH);
  long duration, inches, cm, mm;

  pinMode(3, OUTPUT);
  digitalWrite(3, LOW);
  delayMicroseconds(2);
  digitalWrite(3, HIGH);
  delayMicroseconds(5);
  digitalWrite(3, LOW);

  pinMode (4, INPUT);
  duration = pulseIn(4, HIGH);

  inches = microsecondsToInches(duration);
  cm = microsecondsToCentimeters(duration);
  
  Serial.print(inches);
  Serial.print("in, ");
  Serial.print(cm);
  Serial.print("cm,");
  Serial.println();
  delay(1000);
}

long microsecondsToInches(long microseconds)
{
  return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds)
{
  return microseconds / 29 / 2;
}

