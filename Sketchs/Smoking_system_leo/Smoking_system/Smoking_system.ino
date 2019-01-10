#include <Wire.h>
#include <rgb_lcd.h>

#define RELAY1 5
#define RELAY2 6

#define D1 A0
#define D2 A1
#define D3 A2
#define D4 A3

#define POT A6

int offset = 0;
int pot_val = 0;
int pot_end = 0;

boolean s = false;

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
  int val4 = analogRead(D4);  
  
  pot_val = analogRead(POT);
  
  Serial.print("POT:");
  Serial.println(pot_val);
  
  pot_end = map(pot_val,0,1023,1,60);
  
  int val11 = map(val1,0,1023,0,99);
  int val22 = map(val2,0,1023,0,99);
  int val33 = map(val3,0,1023,0,99);
  int val44 = map(val4,0,1023,0,99);
  
  int limit = offset + pot_end;
  
  Serial.print("D1:");
  Serial.print(val1);
  Serial.print(" - ");
  Serial.println(val11);
  
  lcd.setCursor(0,0);
  if (val11 < 10) {
    lcd.print(" ");
  }
  lcd.print(val11); 
  
  Serial.print("D2:");
  Serial.print(val2);
  Serial.print(" - ");
  Serial.println(val22);
  
  lcd.setCursor(4,0);
  if (val22 < 10) {
    lcd.print(" ");
  }
  lcd.print(val22); 
  
  Serial.print("D3:");
  Serial.print(val3);
  Serial.print(" - ");
  Serial.println(val33);
  
  lcd.setCursor(8,0);
  if (val33 < 10) {
    lcd.print(" ");
  }
  lcd.print(val33); 
  
  Serial.print("D4:");
  Serial.print(val4);
  Serial.print(" - ");
  Serial.println(val44);
  
  lcd.setCursor(12,0);
  if (val44 < 10) {
    lcd.print(" ");
  }
  lcd.print(val44); 
  
  Serial.print("LIMIT:");
  Serial.println(limit);
  
  lcd.setCursor(0,1);
  lcd.print("LIMIT: ");
  if (limit < 10) {
    lcd.print(" ");
  }
  lcd.print(limit); 
  
  if (val11 > limit || val22 > limit || val33 > limit || val44 > limit) {
    if ((val1 < 1022) && (val2 < 1022) && (val3 < 1022) && (val4 < 1022) && (val1 > 1) && (val2 > 1) && (val3 > 1) && (val4 > 1)) {
      digitalWrite(RELAY1,1);
      digitalWrite(RELAY2,1);
      s = true;
      lcd.setCursor(13,1);
      lcd.print(" ON");
      delay(15000);
      //turn off after
      digitalWrite(RELAY1,0);
      digitalWrite(RELAY2,0);
      s = false;
      lcd.setCursor(13,1);
      lcd.print("OFF");
      delay(15000);
      //offset = 38;
    } else {
      digitalWrite(RELAY1,0);
      digitalWrite(RELAY2,0);
      s = false;
      lcd.setCursor(13,1);
      lcd.print("OFF");
    }
  }
  lcd.setCursor(13,1);
  if (s == true) {
    lcd.print(" ON");
  } else {
    lcd.print("OFF");
  }
  Serial.println(s);
  delay(1000);
}

