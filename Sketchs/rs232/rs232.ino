#define REDLED 2
#define GREENLED 3
#define BLUELED 4
#define SOUND 5

boolean stopit = false;

String readString;
String m = "69";
int z1;
int i;

void setup() {
  Serial.begin(38400);
  pinMode(REDLED,OUTPUT);
  pinMode(GREENLED,OUTPUT);
  pinMode(BLUELED,OUTPUT);
  digitalWrite(GREENLED,LOW);
  digitalWrite(REDLED,LOW);
  digitalWrite(BLUELED,LOW);
  tone(SOUND, 5000, 200);
  delay(150);
  tone(SOUND, 4600, 200);
  delay(150);
  tone(SOUND, 4200, 200);
  delay(150);
  tone(SOUND, 3800, 200);
  delay(150);
  tone(SOUND, 3400, 200);
  delay(150);
  tone(SOUND, 3000, 200);
  delay(150);
}

void loop() {

  if (Serial.available())  {

    char c = Serial.read();
    if (c == '\n') {
      stopit = true;
      if (stopit == true) {
        Serial.write("OK\r\n");
        //Serial.println(readString);
        z1 = readString.indexOf(',');
        m = readString.substring(0, z1);

        Serial.print("MODE=");
        Serial.println(m);

        Serial.println();

        if (m.toInt() == 0) {
          //0
          Serial.println("MODE 0");
        } else if (m.toInt() == 1) {
          //1
          Serial.println("MODE 1");
        } else if (m.toInt() == 2) {
          //2
          Serial.println("MODE 2");
        } else if (m.toInt() == 3) {
          //3
          Serial.println("MODE 3");
        } else if (m.toInt() == 4) {
          //4
          Serial.println("MODE 4");
        } else {
          Serial.println(">1,2,3,4,5,6,7,8,9,0<");
        }

        //m = "";

        delay(999);
      }
      readString = ""; //clears variable for new input
      stopit = false;
    } else {
      readString += c; //makes the string readString
    }

  } 

  if (stopit == false) {
    i++;
    digitalWrite(GREENLED,HIGH);
    Serial.print(">");
    Serial.print(m);
    Serial.print(",");
    Serial.print(i);
    Serial.print(",");
    Serial.print(random(100,999));
    Serial.print(",");
    Serial.print(random(100,999));
    Serial.print(",");
    Serial.print(random(100,999));
    Serial.print(",");
    Serial.print(random(100,999));
    Serial.print(",");
    Serial.print(random(100,999));
    Serial.print(",");
    Serial.print(random(100,999));
    Serial.print(",");
    Serial.print(random(100,999));
    Serial.print(",");
    Serial.print(random(100,999));
    Serial.print(",");
    Serial.print(random(100,999));
    Serial.print(",");
    Serial.print(random(100,999));
    Serial.print(",");
    Serial.print(random(100,999));
    Serial.print(",");
    Serial.print(random(100,999));
    Serial.print(",");
    Serial.print(random(100,999));
    Serial.print(",");
    Serial.print(random(100,999));
    Serial.print(",");
    Serial.print(random(100,999));
    Serial.print(",");
    Serial.print(random(100,999));
    Serial.print(",");
    Serial.print(random(100,999));
    Serial.print(",");
    Serial.print(random(100,999));
    Serial.println("<");
    delay(99);
    digitalWrite(GREENLED,LOW);
    delay(900);
  }
}
