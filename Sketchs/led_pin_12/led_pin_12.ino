#include <stdint.h>
#include <TouchScreen.h>

#define YP A2  // must be an analog pin, use "An" notation!
#define XM A3  // must be an analog pin, use "An" notation!
#define YM A5   // can be a digital pin
#define XP A4   // can be a digital pin

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 150);

#include <Adafruit_GFX.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_ILI9341.h>

// For the Adafruit shield, these are the default.
#define TFT_DC 34
#define TFT_CS 32
#define TFT_MOSI 30
#define TFT_CLK 26
#define TFT_RST 41
#define TFT_MISO 28

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

#include <DueFlashStorage.h>
DueFlashStorage dueFlashStorage;

//pins
int motor = 40;
int redled = 24;
int pedalPin = 48;
int encoderPin1 = 50;
int encoderPin2 = 51;
int button1 = 52;
int button2 = 53;
int tempPin0 = A8;
int tempPin1 = A9;
int tempPin2 = A10;
int tempPin3 = A11;
int setPoint = 100;

//init
int r = 75;
float p = 0;
float start_ = 75;
int i = 0;

//states
volatile int pedalState = LOW;
int button1State = 0;
int button2State = 0;
volatile int lastEncoded = 0;
volatile long encoderValue = 0;

volatile int stop_ = 3000;
boolean motorStatus = 0;
long lastencoderValue = 0;
volatile long encoder0Pos = 0;

//percent = (val/255)*100
//inverse = (val/100)*255

// Color definitions
#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF
#define VIOLET 0xEC1D
#define TEAL 0x0410
#define LIME 0x07E0

void setup() {
  Serial.begin(9600);
  tft.begin();
  tft.fillScreen(BLACK);
  tft.setRotation(1);
  pinMode(motor,OUTPUT);
  pinMode(pedalPin, INPUT_PULLUP); 
  pinMode(button1,INPUT);
  pinMode(button2,INPUT);
  pinMode(encoderPin1, INPUT); 
  pinMode(encoderPin2, INPUT);
  pinMode(redled,OUTPUT);
  digitalWrite(encoderPin1, HIGH); //turn pullup resistor on
  digitalWrite(encoderPin2, HIGH); //turn pullup resistor on
  attachInterrupt(pedalPin, pedalS, FALLING);
  attachInterrupt(encoderPin1, updateEncoder, RISING);
  analogWrite(motor,0);
  digitalWrite(button1, HIGH); //turn pullup resistor on
  digitalWrite(button2, HIGH); //turn pullup resistor on
  if (dueFlashStorage.read(0) != 0) { r = dueFlashStorage.read(0); }
  //START
  //X//Y//SIZE//Y//
  tft.drawLine(0,40,280,40,WHITE);
  tft.drawLine(0,41,280,41,WHITE);
  
  tft.fillRect(10,180,90,50,BLUE);
  tft.setCursor(44,188);
  tft.setTextColor(WHITE);
  tft.setTextSize(5);
  tft.println("-");
  
  tft.fillRect(110,180,90,50,BLUE);
  tft.setCursor(144,188);
  tft.setTextColor(WHITE);
  tft.setTextSize(5);
  tft.println("+");
  
  tft.fillRect(210,180,96,50,TEAL);
  tft.setCursor(236,191);
  tft.setTextColor(WHITE);
  tft.setTextSize(4);
  tft.println("OK");
}

void loop() {
  Point t = ts.getPoint();
  
  if (t.z >= 10 && t.z <= 1000) {

     Serial.print("X = "); Serial.print(t.x);
     Serial.print("\tY = "); Serial.print(t.y);
     Serial.print("\tPressure = "); Serial.println(t.z);
     
      if((t.x > 150) && (t.x < (340))) {
        if ((t.y > 140) && (t.y <= 380)) {
          Serial.println("MINUS btn hit");
          setPoint = setPoint - 10;
        }
      }
      
      if((t.x > 150) && (t.x < (340))) {
        if ((t.y > 400) && (t.y <= 600)) {
          Serial.println("PLUS btn hit");
          setPoint = setPoint + 10; 
        }
      }
  }
  /*    
  Serial.println(encoder0Pos, DEC);
  
  Serial.print("SETPOINT; ");
  Serial.println(setPoint);
  */
  int button1State = digitalRead(button1);
  int button2State = digitalRead(button2);
  
  int v0 = analogRead(tempPin0);
  int v1 = analogRead(tempPin1);
  int v2 = analogRead(tempPin2);
  int v3 = analogRead(tempPin3);
  
  float voltage0 = v0 * 3.3;
  float voltage1 = v1 * 3.3;
  float voltage2 = v2 * 3.3;
  float voltage3 = v3 * 3.3;  
  
  voltage0 /= 1024.0;
  voltage1 /= 1024.0;
  voltage2 /= 1024.0;
  voltage3 /= 1024.0;  
  /*
  Serial.print(voltage0); 
  Serial.println(" volts");
  Serial.print(voltage1); 
  Serial.println(" volts");
  Serial.print(voltage2); 
  Serial.println(" volts");
  Serial.print(voltage3); 
  Serial.println(" volts");
  */
  
  float tempC0 = (voltage0 * 99);
  float tempC1 = (voltage1 * 99);
  float tempC2 = (voltage2 * 99);
  float tempC3 = (voltage3 * 99);
  
  tft.setCursor(0,0);
  tft.setTextColor(VIOLET,BLACK);
  tft.setTextSize(4);
  tft.print("TEMP_4: ");
  tft.println(tempC3);
  
  tft.setCursor(0,50);
  tft.setTextColor(LIME,BLACK);
  tft.setTextSize(4);
  tft.print("SETPOINT: ");
  tft.println(setPoint);
  
  if (Serial.available() > 0) {
    r = Serial.parseInt();
  }
  
  if(button1State == LOW && r >= 25) {
    r = r - 25;
    uint8_t b = r;
    dueFlashStorage.write(0,b);
  }   
  if (button2State == LOW && r <= 230) {
    r = r + 25;
    uint8_t b = r;
    dueFlashStorage.write(0,b);
  }
  /*
  Serial.print("PERCENT: ");
  p = (r/255)*100;
  Serial.println(p);
  Serial.print("INVERSED: ");
  Serial.println(r);
  */
  /*
  Serial.print("FLASH 0: ");
  Serial.println(dueFlashStorage.read(0));
  Serial.print("FLASH 1: ");
  Serial.println(dueFlashStorage.read(1));
  Serial.print("FLASH 2: ");
  Serial.println(dueFlashStorage.read(2));
  //end
  */
  delay(100);
  
}

void updateEncoder(){
  if (digitalRead(encoderPin1) == digitalRead(encoderPin2)) {
    encoder0Pos++;
  } else {
    encoder0Pos--;
  }
  rotate();
}

void pedalS() {
  int pinStatus = digitalRead(pedalPin);
  if (pinStatus == 0) {
    PlusP();
  } else {
    MinusP();
  }
}

void PlusP () {
  digitalWrite(redled,HIGH);
  attachInterrupt(50, updateEncoder, RISING);
  rotate();
}

void MinusP () {
  digitalWrite(redled,LOW);
  analogWrite(motor,0);
  detachInterrupt(50);  
}

void rotate(){
  if (encoder0Pos <= stop_) {      
    analogWrite(motor,r);
    motorStatus = 1;
  } else {
    encoder0Pos = 1;
    do_again();
  }  
}

void do_again() {
  for (int x=0; x <= 384; x++){
    motorStatus = 0;
    analogWrite(motor,0);
    Serial.println("PAUSE");
  }
  Serial.println(motorStatus);
  Serial.println("RESTART OPERATION");
}

