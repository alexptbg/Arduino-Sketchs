#define sensorIR1 A4
#define sensorIR2 A5

#define RXpin 6
#define TXpin 7

#include <SoftwareSerial.h>

SoftwareSerial mySerial(RXpin,TXpin);

const int buttonPin = 3;
int greenled1 = 8;
int greenled2 = 9;
int sendled1 = 10;
int sendled2 = 11;

int buttonState = 1;
int buttonPushCounter = 0;
int lastButtonState = 0;

float sensorValue1;
float inches1;
float cm1;
float total1;

float sensorValue2;
float inches2;
float cm2;
float total2;

void setup() {
  Serial.begin(38400);
  mySerial.begin(38400);
  pinMode(greenled1, OUTPUT);
  pinMode(greenled2, OUTPUT);
  pinMode(sendled1, OUTPUT);
  pinMode(sendled2, OUTPUT);
  pinMode(buttonPin, INPUT);
  digitalWrite(greenled1, HIGH);
  delay(200);
  digitalWrite(greenled1, LOW);
  digitalWrite(greenled2, HIGH);
  delay(200);
  digitalWrite(greenled2, LOW);
  digitalWrite(sendled1, HIGH);
  delay(200);
  digitalWrite(sendled1, LOW);
  digitalWrite(sendled2, HIGH);
  delay(200);
  digitalWrite(sendled2, LOW);
  Serial.println("START");
  //mySerial.println("Hello, world?");
  delay(200);
  //Serial.print("Pushes: ");
  //Serial.println(buttonPushCounter);
}

void loop() {
  buttonState = digitalRead(buttonPin);

  if (buttonState != lastButtonState) {
    if (buttonState == LOW) {
      buttonPushCounter++;
      //Serial.println("ON");
      //Serial.print("Pushes: ");
      //Serial.println(buttonPushCounter);
      if (buttonPushCounter==1) {
        //Serial.println(total2,1);
        digitalWrite(sendled1, HIGH);
        mySerial.print(">L1");
        if (total1 < 100) { mySerial.print("0"); }
        mySerial.print(total1,1);
        mySerial.println("<");
        Serial.print("L1: ");
        Serial.println(total1,1);    
        delay(150);
        digitalWrite(sendled1, LOW);
      }
      if (buttonPushCounter==2) {
        //Serial.println("Status 2");
        digitalWrite(sendled2, HIGH);
        mySerial.print(">L2");
        if (total2 < 100) { mySerial.print("0"); }
        mySerial.print(total2,1);
        mySerial.println("<");
        Serial.print("L2: ");
        Serial.println(total2,1);    
        delay(150);
        digitalWrite(sendled2, LOW);
      }
    }
    else {
      //Serial.println("OFF");
    }
  }
  lastButtonState = buttonState;

  if (buttonPushCounter % 2 == 0) {
    buttonPushCounter = 0;
  } else {
    //do nothing 
  }
  //button inactive
  if (buttonState == 1) {
    //Serial.println("Status 0");
    
    sensorValue1 = analogRead(sensorIR1);
    inches1 = 4192.936 * pow(sensorValue1,-0.935) - 3.937;
    cm1 = 10650.08 * pow(sensorValue1,-0.935) - 10;
    total1 = cm1-1.0;
    
    sensorValue2 = analogRead(sensorIR2);
    inches2 = 4192.936 * pow(sensorValue2,-0.935) - 3.937;
    cm2 = 10650.08 * pow(sensorValue2,-0.935) - 10;
    total2 = cm2-1.0;
    
    delay(10);
    digitalWrite(greenled1, HIGH);
    digitalWrite(greenled2, HIGH);
    
    if (buttonPushCounter==1) {
      mySerial.print(">L2");
      if (total2 < 100) { mySerial.print("0"); }
      mySerial.print(total2,1);
      mySerial.println("<");
    } else {
      mySerial.print(">L1");
      if (total1 < 100) { mySerial.print("0"); }
      mySerial.print(total1,1);
      mySerial.println("<");
    }
    //Serial.println(total1,1);
    //Serial.println(total2,1);
    
    delay(100);
    digitalWrite(greenled1, LOW);
    digitalWrite(greenled2, LOW);
    delay(100);
  }
}
