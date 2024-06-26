#include <Adafruit_NeoPixel.h>
//PIRINTEX LED CONTROLLER BY ALEX

//9696
#define LED 4
#define RGB 3
#define VOLT 2
#define SOUND 5
#define NUMPIXELS 1

boolean stopit = false;

//Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS,RGB,NEO_GRB+NEO_KHZ800);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, RGB, NEO_RGB + NEO_KHZ800);

String readString, m, r, g, b, t, s, f, x, p, z;
int z1, z2, z3, z4, z5, z6, z7, z8, z9, z10;

//Color definitions
uint32_t magenta = strip.Color(255, 0, 255);
uint32_t white = strip.Color(255, 255, 255);
uint32_t yellow = strip.Color(255, 255, 0);
uint32_t red = strip.Color(255, 0, 0);
uint32_t d_red = strip.Color(200, 0, 0);
uint32_t l_green = strip.Color(0, 255, 0);
uint32_t d_green = strip.Color(0, 153, 0);
uint32_t orange = strip.Color(255, 127, 0);
uint32_t purple = strip.Color(102, 0, 102);
uint32_t pink = strip.Color(255, 0, 127);
uint32_t blue = strip.Color(51, 51, 255);
uint32_t d_blue = strip.Color(0, 0, 255);
uint32_t l_blue = strip.Color(0, 255, 255);
uint32_t teal = strip.Color(0, 153, 153);
uint32_t off = strip.Color(0, 0, 0);

void setup() {
  Serial.begin(38400);
  pinMode(LED, OUTPUT);
  //pinMode(VOLT, OUTPUT);
  digitalWrite(LED, HIGH);
  //digitalWrite(VOLT, HIGH);
  strip.begin();
  strip.setBrightness(255);
  strip.setPixelColor(0, pink);
  strip.show();
  tone(SOUND, 4600, 200);
  delay(200);
  strip.setPixelColor(0, teal);
  strip.show();
  tone(SOUND, 4200, 200);
  delay(200);
  strip.setPixelColor(0, d_blue);
  strip.show();
  tone(SOUND, 3800, 200);
  delay(200);
  strip.setPixelColor(0, l_green);
  strip.show();
  tone(SOUND, 3400, 200);
  delay(200);
  strip.setPixelColor(0, red);
  strip.show();
  tone(SOUND, 3000, 200);
  delay(200);
  strip.setPixelColor(0, 0, 0, 0);
  strip.show();
  Serial.println("PIRINTEX LED CONTROLLER BY ALEX");
  Serial.println("Initialization...Ok");
  set_init();
}

void loop() {
  if (Serial.available())  {
    char c = Serial.read();
    if (c == '\n') {
      Serial.write("OK\r\n");
      //Serial.println(readString);
      z1 = readString.indexOf(',');
      m = readString.substring(0, z1);
      /*
      z2 = readString.indexOf(',', z1 + 1);
      r = readString.substring(z1 + 1, z2 + 1);
      z3 = readString.indexOf(',', z2 + 1);
      g = readString.substring(z2 + 1, z3 + 1);
      z4 = readString.indexOf(',', z3 + 1 );
      b = readString.substring(z3 + 1, z4 + 1);
      z5 = readString.indexOf(',', z4 + 1 );
      z = readString.substring(z4 + 1, z5 + 1);
      z6 = readString.indexOf(',', z5 + 1 );
      t = readString.substring(z5 + 1, z6 + 1);
      z7 = readString.indexOf(',', z6 + 1 );
      s = readString.substring(z6 + 1, z7 + 1);
      z8 = readString.indexOf(',', z7 + 1 );
      f = readString.substring(z7 + 1, z8 + 1);
      z9 = readString.indexOf(',', z8 + 1 );
      x = readString.substring(z8 + 1, z9 + 1);
      z10 = readString.indexOf(',', z9 + 1);
      p = readString.substring(z9 + 1);
      */
      Serial.print("M = ");
      Serial.println(m);
      /*
      Serial.print("R = ");
      Serial.println(r);
      Serial.print("G = ");
      Serial.println(g);
      Serial.print("B = ");
      Serial.println(b);
      Serial.print("Brightness = ");
      Serial.println(z);
      Serial.print("time = ");
      Serial.println(t);
      Serial.print("Sound? = ");
      Serial.println(s);
      Serial.print("F = ");
      Serial.println(f);
      Serial.print("Times = ");
      Serial.println(x);
      Serial.print("Pause = ");
      Serial.println(p);
      */
      Serial.println();

      if (m.toInt() == 0) {
        //off
        Serial.println("LED OFF");
        strip.setPixelColor(0, off);
        strip.show();
        //0
      } else if (m.toInt() == 1) {
        //led always on without buzzer
        z2 = readString.indexOf(',', z1 + 1);
        r = readString.substring(z1 + 1, z2 + 1);
        z3 = readString.indexOf(',', z2 + 1);
        g = readString.substring(z2 + 1, z3 + 1);
        z4 = readString.indexOf(',', z3 + 1 );
        b = readString.substring(z3 + 1, z4 + 1);
        z5 = readString.indexOf(',', z4 + 1 );
        z = readString.substring(z4 + 1);
        if (z.toInt() > 220) {
          strip.setBrightness(220);
        } else {
          strip.setBrightness(z.toInt());
        }
        strip.setPixelColor(0, r.toInt(), g.toInt(), b.toInt());
        Serial.println("LED ON");
        strip.show();
        //1,255,0,0,255
      } else if (m.toInt() == 2) {
        //led and buzzer with interrupt
        z2 = readString.indexOf(',', z1 + 1);
        r = readString.substring(z1 + 1, z2 + 1);
        z3 = readString.indexOf(',', z2 + 1);
        g = readString.substring(z2 + 1, z3 + 1);
        z4 = readString.indexOf(',', z3 + 1 );
        b = readString.substring(z3 + 1, z4 + 1);
        z5 = readString.indexOf(',', z4 + 1 );
        z = readString.substring(z4 + 1, z5 + 1);
        z6 = readString.indexOf(',', z5 + 1 );
        t = readString.substring(z5 + 1, z6 + 1);
        z7 = readString.indexOf(',', z6 + 1 );
        s = readString.substring(z6 + 1, z7 + 1);
        z8 = readString.indexOf(',', z7 + 1 );
        f = readString.substring(z7 + 1, z8 + 1);
        z9 = readString.indexOf(',', z8 + 1 );
        x = readString.substring(z8 + 1, z9 + 1);
        z10 = readString.indexOf(',', z9 + 1);
        p = readString.substring(z9 + 1);
        for (int i = 1; i <= x.toInt(); i++) {
          Serial.println("LED ON");
          strip.setBrightness(z.toInt());
          strip.setPixelColor(0, r.toInt(), g.toInt(), b.toInt());
          strip.show();
          if (s.toInt() == 1) {
            tone(SOUND, f.toInt(), t.toInt());
          }
          delay(t.toInt());
          Serial.println("LED OFF");
          strip.setPixelColor(0, off);
          strip.show();
          delay(p.toInt());
        }
        //2,255,0,0,255,250,1,4500,3,250
      } else if (m.toInt() == 3) {
        //only buzzer
        z2 = readString.indexOf(',', z1 + 1);
        t = readString.substring(z1 + 1, z2 + 1);
        z3 = readString.indexOf(',', z2 + 1);
        f = readString.substring(z2 + 1, z3 + 1);
        z4 = readString.indexOf(',', z3 + 1 );
        x = readString.substring(z3 + 1, z4 + 1);
        z5 = readString.indexOf(',', z4 + 1 );
        p = readString.substring(z4 + 1);
        strip.setPixelColor(0, off);
        strip.show();
        for (int i = 1; i <= x.toInt(); i++) {
          Serial.println("BUZZER ON");
          tone(SOUND, f.toInt(), t.toInt());
          delay(t.toInt());
          Serial.println("BUZZER OFF");
          delay(p.toInt());
        }
        //3,25,4200,30,250
      } else if (m.toInt() == 4) {
        //led always on with buzzer interrupted
        z2 = readString.indexOf(',', z1 + 1);
        r = readString.substring(z1 + 1, z2 + 1);
        z3 = readString.indexOf(',', z2 + 1);
        g = readString.substring(z2 + 1, z3 + 1);
        z4 = readString.indexOf(',', z3 + 1 );
        b = readString.substring(z3 + 1, z4 + 1);
        z5 = readString.indexOf(',', z4 + 1 );
        z = readString.substring(z4 + 1, z5 + 1);
        z6 = readString.indexOf(',', z5 + 1 );
        t = readString.substring(z5 + 1, z6 + 1);
        z7 = readString.indexOf(',', z6 + 1 );
        s = readString.substring(z6 + 1, z7 + 1);
        z8 = readString.indexOf(',', z7 + 1 );
        f = readString.substring(z7 + 1, z8 + 1);
        z9 = readString.indexOf(',', z8 + 1 );
        x = readString.substring(z8 + 1, z9 + 1);
        z10 = readString.indexOf(',', z9 + 1);
        p = readString.substring(z9 + 1);
        Serial.println("LED ON");
        if (z.toInt() > 220) {
          strip.setBrightness(220);
        } else {
          strip.setBrightness(z.toInt());
        }
        strip.setPixelColor(0, r.toInt(), g.toInt(), b.toInt());
        strip.show();
        for (int i = 1; i <= x.toInt(); i++) {
          Serial.println("BUZZER ON");
          tone(SOUND, f.toInt(), t.toInt());
          delay(t.toInt());
          Serial.println("BUZZER OFF");
          delay(p.toInt());
        }
        //4,255,0,0,255,250,1,4500,3,250
      } else {
        Serial.println("LED OFF");
        strip.setPixelColor(0, off);
        strip.show();
      }

      m = "";
      r = "";
      g = "";
      b = "";
      t = "";
      f = "";
      s = "";
      x = "";
      p = "";
      z = "";
      readString = ""; //clears variable for new input
    }
    else {
      readString += c; //makes the string readString
    }
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;
  for (j = 0; j < 256; j++) {
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

void set_init() {
  strip.setPixelColor(0, l_green);
  strip.setBrightness(220);
  strip.show();
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
