/*
 * MARTI test
 */

#include <Adafruit_NeoPixel.h>

int button = 5;
int led = 13;
int status = false;

// SETUP YOUR OUTPUT PIN AND NUMBER OF PIXELS
#define PIN 2
#define NUM_PIXELS 64
#define BRIGHTNESS 240
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, PIN, NEO_GRB + NEO_KHZ800);
uint32_t ENDCOLOR = strip.Color(255,255,255);

void setup(){
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  pinMode(button, INPUT_PULLUP); // set the internal pull up resistor, unpressed button is HIGH
  Serial.begin(9600);
  strip.setBrightness(BRIGHTNESS);
  strip.begin();
  clearStrip(); // Initialize all pixels to 'off'
  delay(1000);
}

void loop(){
  //a) if the button is not pressed the false status is reversed by !status and the LED turns on
  //b) if the button is pressed the true status is reveresed by !status and the LED turns off
  if (digitalRead(button) == true) {
    status = !status;
    digitalWrite(led, status);
    if(status==0) {
      Serial.println("ON");
      //marti(1, 32, 2, 0xFF1000); // Cycles, Speed, Width, RGB Color (original orange-red)
      marti2(strip.Color(255,255,255),50);    // white
    } else {
      Serial.println("OFF");
      marti2back(strip.Color(0,0,0),50);    //Black/off
      //clearStrip();
    }
  }
  while(digitalRead(button) == true);
  delay(50); // keeps a small delay
}

// Cycles - one cycle is scanning through all pixels left then right (or right then left)
// Speed - how fast one cycle is (32 with 16 pixels is default KnightRider speed)
// Width - how wide the trail effect is on the fading out LEDs.  The original display used
//         light bulbs, so they have a persistance when turning off.  This creates a trail.
//         Effective range is 2 - 8, 4 is default for 16 pixels.  Play with this.
// Color - 32-bit packed RGB color value.  All pixels will be this color.
// knightRider(cycles, speed, width, color);
void knightRider(uint16_t cycles, uint16_t speed, uint8_t width, uint32_t color) {
  uint32_t old_val[NUM_PIXELS]; // up to 256 lights!
  // Larson time baby!
  for(int i = 0; i < cycles; i++){
    for (int count = 1; count<NUM_PIXELS; count++) {
      strip.setPixelColor(count, color);
      old_val[count] = color;
      for(int x = count; x>0; x--) {
        old_val[x-1] = dimColor(old_val[x-1], width);
        strip.setPixelColor(x-1, old_val[x-1]); 
      }
      strip.show();
      delay(speed);
    }
    for (int count = NUM_PIXELS-1; count>=0; count--) {
      strip.setPixelColor(count, color);
      old_val[count] = color;
      for(int x = count; x<=NUM_PIXELS ;x++) {
        old_val[x-1] = dimColor(old_val[x-1], width);
        strip.setPixelColor(x+1, old_val[x+1]);
      }
      strip.show();
      delay(speed);
    }
  }
}

uint32_t dimColor(uint32_t color, uint8_t width) {
   return (((color&0xFF0000)/width)&0xFF0000) + (((color&0x00FF00)/width)&0x00FF00) + (((color&0x0000FF)/width)&0x0000FF);
}

void clearStrip() {
  for( int i = 0; i<NUM_PIXELS; i++){
    strip.setPixelColor(i, 0x000000);
    //strip.show();
  }
  strip.show();
}

void marti(uint16_t cycles, uint16_t speed, uint8_t width, uint32_t color) {
  uint32_t old_val[NUM_PIXELS]; // up to 256 lights!
  // Larson time baby!
  for(int i = 0; i < cycles; i++){
    for (int count = 1; count<NUM_PIXELS; count++) {
      strip.setPixelColor(count, color);
      old_val[count] = color;
      for(int x = count; x>0; x--) {
        old_val[x-1] = dimColor(old_val[x-1], width);
        strip.setPixelColor(x-1, old_val[x-1]); 
      }
      strip.show();
      delay(speed);
    }
    clearStrip();
    for (int count = NUM_PIXELS-1; count>=0; count--) {
      strip.setPixelColor(count, color);
      old_val[count] = color;
      for(int x = count; x<=NUM_PIXELS ;x++) {
        old_val[x-1] = dimColor(old_val[x-1], width);
        strip.setPixelColor(x+1, old_val[x+1]);
      }
      strip.show();
      delay(speed);
    }
    clearStrip();
    for( int i = 0; i<NUM_PIXELS; i++){
      strip.setPixelColor(i, ENDCOLOR);
      //strip.show();
    }
    strip.show();
  }
}

void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}

void marti2(uint32_t color, int wait) {
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}

void marti2back(uint32_t color, int wait) {
  for(int i=NUM_PIXELS-1; i>=0; i--) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}
