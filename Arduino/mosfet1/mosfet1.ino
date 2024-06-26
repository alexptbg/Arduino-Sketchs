#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h>
#endif

#define PIXEL_PIN 4
#define NUMPIXELS 64
#define BRIGHTNESS 230
Adafruit_NeoPixel strip(NUMPIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

const int signalinput=5;
boolean state = HIGH;

void setup() {
  strip.setBrightness(BRIGHTNESS);
  strip.begin(); // Initialize NeoPixel strip object (REQUIRED)
  //strip.show();  // Initialize all pixels to 'off'
  pinMode(signalinput,INPUT_PULLUP);
  colorWipe(strip.Color(255,0,0),0);//Red
  strip.show();
  Serial.begin(9600);
}

void loop() {
  state = digitalRead(signalinput);
  if(state==LOW) {
    //digitalWrite(LampControl,HIGH);
    colorWipe(strip.Color(0,255,0),0);//Green
    strip.show();
    delay(50);
  } else {
    //digitalWrite(LampControl,LOW);
    colorWipe(strip.Color(255,0,0),0);//Red
    strip.show();
  }
  //Serial.println(state);
}

void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}
