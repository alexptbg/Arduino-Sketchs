#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_HX8357.h>

// These are 'flexible' lines that can be changed
#define TFT_CS 10
#define TFT_DC 9
#define TFT_RST 8 // RST can be set to -1 if you tie it to Arduino's reset

// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
Adafruit_HX8357 tft = Adafruit_HX8357(TFT_CS, TFT_DC, TFT_RST);

void setup() {
  Serial.begin(9600);
  tft.begin(HX8357D);
  tft.setRotation(1);
  tft.fillScreen(HX8357_BLACK);
}

void loop(void) {
  tft.setCursor(0,0);
  tft.setTextColor(HX8357_WHITE); 
  tft.setTextSize(5);
  tft.println("Hello World!");
}

