#include <SPI.h>

#include <Adafruit_NeoPixel.h>
#include <DHT.h>

#define L1 2

#define DHTPIN 3
#define DHTTYPE DHT11

Adafruit_NeoPixel strip = Adafruit_NeoPixel(8,L1,NEO_GRB + NEO_KHZ800);

DHT dht(DHTPIN,DHTTYPE);

// Color definitions
uint32_t magenta = strip.Color(255,0,255);
uint32_t white = strip.Color(255,255,255);
uint32_t yellow = strip.Color(255,255,0);
uint32_t red = strip.Color(255,0,0);
uint32_t green = strip.Color(0,255,0);
uint32_t orange = strip.Color(255,130,0);
uint32_t purple = strip.Color(130,0,255);
uint32_t d_blue = strip.Color(0,0,255);
uint32_t l_blue = strip.Color(0,255,255);

void setup() {
  strip.begin();
  strip.show();
  strip.setBrightness(20);
  Serial.begin(9600); 
  dht.begin();
}

void loop() {
  strip.setPixelColor(0,green);
  strip.setPixelColor(1,d_blue);
  strip.show();
  delay(500);
  strip.setPixelColor(1,0);
  strip.setPixelColor(7,0);
  strip.show();
  delay(500); 
  float h = dht.readHumidity();
  //float t = dht.readTemperature();
  float f = dht.readTemperature(true);
  float t = (f - 32) * 5 / 9;
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    strip.setPixelColor(7,red);
    strip.show();
    return;
  } else {
    strip.setPixelColor(7,magenta);
    strip.show();
  }
  float hi = dht.computeHeatIndex(f,h);
  float index = (hi - 32) * 5 / 9 - 1;
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print(f);
  Serial.print(" *F\t");
  Serial.print("Index: ");
  Serial.print(index);
  Serial.println(" *C");
}



