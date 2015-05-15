#include <Wire.h>
#include <rgb_lcd.h>

#define RELAY1 2
#define RELAY2 3

#define D1 A0
#define D2 A1
#define D3 A2

#define POT A3

int offset = 0;
int pot_val = 0;
int pot_end = 0;

//LCD
rgb_lcd lcd;
const int colorR = 0;
const int colorG = 255;
const int colorB = 255;

void setup() {
  //SERIAL
  Serial.begin(9600); 
  //DISPLAY
  Wire.begin();// Activate I2C
  //display
  lcd.begin(16,2);
  lcd.setRGB(colorR,colorG,colorB);
  lcd.setCursor(0,0);
  lcd.print("KA-EX.NET");
  lcd.setCursor(0,1);
  lcd.print("SOARES  KARADJOV"); 
  delay(1000);
  lcd.clear();
  //RELAYS
  pinMode(RELAY1,OUTPUT);
  pinMode(RELAY2,OUTPUT);
}

void loop() {

  int val1 = analogRead(D1);
  int val2 = analogRead(D2);
  int val3 = analogRead(D3);
  
  pot_val = analogRead(POT);
  
  Serial.print("POT:");
  Serial.println(pot_val);
  
  pot_end = map(pot_val,0,1023,1,41);
  
  int val11 = map(val1,0,1023,0,99);
  int val22 = map(val2,0,1023,0,99);
  int val33 = map(val3,0,1023,0,99);  
  
  int limit = offset + pot_end;
  
  Serial.print("D1:");
  Serial.print(val1);
  Serial.print(" - ");
  Serial.println(val11);
  
  lcd.setCursor(0,0);
  lcd.print("1:"); 
  if (val11 < 10) {
    lcd.print(" ");
  }
  lcd.print(val11); 
  
  Serial.print("D2:");
  Serial.print(val2);
  Serial.print(" - ");
  Serial.println(val22);
  
  lcd.setCursor(6,0);
  lcd.print("2:");
  if (val22 < 10) {
    lcd.print(" ");
  }
  lcd.print(val22); 
  
  Serial.print("D3:");
  Serial.print(val3);
  Serial.print(" - ");
  Serial.println(val33);
  
  lcd.setCursor(12,0);
  lcd.print("3:");
  if (val33 < 10) {
    lcd.print(" ");
  }
  lcd.print(val33); 
  
  Serial.print("LIMIT:");
  Serial.println(limit);
  
  lcd.setCursor(0,1);
  lcd.print("LIMIT: ");
  if (limit < 10) {
    lcd.print(" ");
  }
  lcd.print(limit); 
  
  if (val11 > limit || val22 > limit || val33 > limit) {
    digitalWrite(RELAY1,1);
    digitalWrite(RELAY2,1);
    delay(10000);
  } else {
    digitalWrite(RELAY1,0);
    digitalWrite(RELAY2,0);
  }
  delay(1000);
}

