//includes
#include <Adafruit_NeoPixel.h>
#include <math.h>

//Status led
#define SLED 4

//Hardware Setup
//Number of pixels in strand
#define N_PIXELS_L 60
#define N_PIXELS_R 60
//Microphone is attached to this analog pin
#define MIC_PIN_L A1
#define MIC_PIN_R A0
//NeoPixel LED strand is connected to this pin
#define LED_PIN_L 6
#define LED_PIN_R 9

//Configuration
//Sample window for average level
#define SAMPLE_WINDOW 4
//Time of pause before peak dot falls
#define PEAK_HANG 2
//Rate of falling peak dot
#define PEAK_FALL 2
//Lower range of analogRead input
#define INPUT_FLOOR 20
//Max range of analogRead input, the lower the value the more sensitive (1023 = max)
//#define INPUT_CEILING 200
int INPUT_CEILING = 200;

//Left channel
//Peak level of column; used for falling dots
byte peak_L = 16;
//int samples
unsigned int sample_L;
//Frame counter for peak dot
byte dotCount_L = 0;
//Frame counter for holding peak dot
byte dotHangCount_L = 0;

//Right channel
//Peak level of column; used for falling dots
byte peak_R = 16;
//int samples
unsigned int sample_R;
//Frame counter for peak dot
byte dotCount_R = 0;
//Frame counter for holding peak dot
byte dotHangCount_R = 0;

//Statis init
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(1, SLED, NEO_GRB + NEO_KHZ800);

//Pixels init
Adafruit_NeoPixel strip_L = Adafruit_NeoPixel(N_PIXELS_L, LED_PIN_L, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip_R = Adafruit_NeoPixel(N_PIXELS_R, LED_PIN_R, NEO_GRB + NEO_KHZ800);

//Potentiometer Dim init
int dimPin = 1;
int dimVal = 0;
int dimT = 20;

//Potentiometer Peak init
int peakPin = 2;
int peakVal = 0;
int peakT = 20;

//Potentiometer Color init
int colorPin = A3;
int colorVal = 0;
int colorT = 0;

//Button init
/*
int buttonPin = 2;  // button pin variable
boolean currentState = LOW;//stroage for current button state
boolean lastState = LOW;//storage for last button state
int currentColor = 0;//storage for current color
*/

//Color definitions
uint32_t magenta = strip_L.Color(255,0,255);
uint32_t white = strip_L.Color(255,255,255);
uint32_t yellow = strip_L.Color(255,255,0);
uint32_t red = strip_L.Color(255,0,0);
uint32_t d_red = strip_L.Color(200,0,0);
uint32_t l_green = strip_L.Color(0,255,0);
uint32_t d_green = strip_L.Color(0,153,0);
uint32_t orange = strip_L.Color(255,127,0);
uint32_t purple = strip_L.Color(102,0,102);
uint32_t pink = strip_L.Color(255,0,127);
uint32_t blue = strip_L.Color(51,51,255);
uint32_t d_blue = strip_L.Color(0,0,255);
uint32_t l_blue = strip_L.Color(0,255,255);
uint32_t teal = strip_L.Color(0,153,153);
uint32_t wheel;

//Colors init
uint32_t colors[] = { d_blue,magenta,white,yellow,red,d_red,l_green,d_green,orange,purple,pink,l_blue,blue,teal,wheel,wheel };
uint32_t s_color = colors[colorT];
//Setup
void setup() {
  //This is only needed on 5V Arduinos (Uno, Leonardo, etc.).
  //Connect 3.3V to mic AND TO AREF ON ARDUINO and enable this line. Audio samples are 'cleaner' at 3.3V.
  //COMMENT OUT THIS LINE FOR 3.3V ARDUINOS (FLORA, ETC.):
  //analogReference(EXTERNAL);
  //Status init
  pixels.begin();
  pixels.setBrightness(25);
  pixels.show();
  //Left pixels init
  strip_L.begin();
  strip_L.show();
  strip_L.setBrightness(dimT);
  //Right pixels init
  strip_R.begin();
  strip_R.show();
  strip_R.setBrightness(dimT);
  //Button init
  /*
  pinMode(buttonPin, INPUT);
  */
  //Init Start
  rainbowCycle(1);
  //Serial start
  Serial.begin(9600);
  Serial.println("Init Start!");
}
//End of setup
//Loop
void loop() {
  //Status
  if (s_color == wheel) {
    pixels.setPixelColor(0,l_green);
  } else {
    pixels.setPixelColor(0,s_color);
  }
  pixels.show();
  //Pixels Brightness
  strip_L.setBrightness(dimT);
  strip_R.setBrightness(dimT);
  //Dim Potentiometer
  dimVal = analogRead(dimPin);
  dimT = map(dimVal,0,1023,20,120);
  //Peak potentiometer
  peakVal = analogRead(peakPin);
  Serial.println(peakVal);
  INPUT_CEILING = map(peakVal,0,1023,100,800);
  //Serial.println(INPUT_CEILING);
  //Color potentiometer
  colorVal = analogRead(colorPin);
  colorT = map(colorVal,0,1023,0,16);
  Serial.println(colorT);
  if(colorT > 15) {
    colorT = 0;
  } else if (colorT < 0) {
    colorT = 0;
  } else {
    s_color = colors[colorT];
  }
  //Read button
  /*
  currentState = digitalRead(buttonPin);
  if (currentState == HIGH && lastState == LOW){
    //if button has just been pressed
    //Serial.println("pressed");
    delay(1);//crude form of button debouncing
    currentColor++;
    if (currentColor > 14) { currentColor = 0; }
    s_color = colors[currentColor];
  }
  lastState = currentState;
  */
  //Start of sample window
  unsigned long startMillis= millis(); 
  //peak-to-peak level
  float peakToPeak_L = 0;
  float peakToPeak_R = 0;
  //levels
  unsigned int signalMax_L = 0;
  unsigned int signalMin_L = 1023;
  unsigned int signalMax_R = 0;
  unsigned int signalMin_R = 1023;
  //Left channel
  unsigned int c_L, y_L;
  //right channel
  unsigned int c_R, y_R;
  //Collect data for length of sample window (in mS)
  while (millis() - startMillis < SAMPLE_WINDOW) {
    sample_L = analogRead(MIC_PIN_L);
    sample_R = analogRead(MIC_PIN_R);
    //Toss out spurious readings
    if (sample_L < 1024) {
      if (sample_L > signalMax_L) {
        //Save just the max levels
        signalMax_L = sample_L; 
      } else if (sample_L < signalMin_L) {
        //Save just the min levels
        signalMin_L = sample_L; 
      }
    }
    if (sample_R < 1024) {
      if (sample_R > signalMax_R) {
        //Save just the max levels
        signalMax_R = sample_R; 
      } else if (sample_R < signalMin_R) {
        //Save just the min levels
        signalMin_R = sample_R; 
      }
    }
  }
  //max - min = peak-peak amplitude
  peakToPeak_L = signalMax_L - signalMin_L; 
  peakToPeak_R = signalMax_R - signalMin_R; 
  //Fill the strip with rainbow gradient
  for (int i=0;i<=strip_L.numPixels()-1;i++) {
    if (s_color == wheel) {
      strip_L.setPixelColor(i,Wheel_L(map(i,0,strip_L.numPixels()-1,30,150)));
    } else {
      strip_L.setPixelColor(i,s_color);
    } 
  }
  for (int x=0;x<=strip_R.numPixels()-1;x++) {
    if (s_color == wheel) {
      strip_R.setPixelColor(x,Wheel_R(map(x,0,strip_R.numPixels()-1,30,150)));
    } else {
      strip_R.setPixelColor(x,s_color);
    } 
  }
  //Scale the input logarithmically instead of linearly
  c_L = fscale(INPUT_FLOOR, INPUT_CEILING, strip_L.numPixels(), 0, peakToPeak_L, 2);
  c_R = fscale(INPUT_FLOOR, INPUT_CEILING, strip_R.numPixels(), 0, peakToPeak_R, 2);
  //Peaks
  if(c_L < peak_L) {
    peak_L = c_L; // Keep dot on top
    dotHangCount_L = 0; // make the dot hang before falling
  }
  if(c_R < peak_R) {
    peak_R = c_R; // Keep dot on top
    dotHangCount_R = 0; // make the dot hang before falling
  }
  // Fill partial column with off pixels
  if (c_L <= strip_L.numPixels()) { 
    drawLine_L(strip_L.numPixels(), strip_L.numPixels()-c_L,strip_L.Color(0, 0, 0));
  }
  if (c_R <= strip_R.numPixels()) {
    drawLine_R(strip_R.numPixels(), strip_R.numPixels()-c_R,strip_R.Color(0, 0, 0));
  }
  // Set the peak dot to match the rainbow gradient
  y_L = strip_L.numPixels() - peak_L;
  y_R = strip_R.numPixels() - peak_R;
  //Get start
  if (s_color == wheel) {
    //y_L+1
    //y_R+1
    strip_L.setPixelColor(y_L-1,Wheel_L(map(y_L,0,strip_L.numPixels()-1,30,150)));
    strip_R.setPixelColor(y_R-1,Wheel_R(map(y_R,0,strip_R.numPixels()-1,30,150)));
  } else {
    strip_L.setPixelColor(y_L-1,red);
    strip_R.setPixelColor(y_R-1,red); 
  }
  //Show strips
  strip_L.show();
  strip_R.show();
  // Frame based peak dot animation
  if(dotHangCount_L > PEAK_HANG) { //Peak pause length
    if(++dotCount_L >= PEAK_FALL) { //Fall rate
      peak_L++;
      dotCount_L = 0;
    }
  } else {
    dotHangCount_L++;
  }
  if(dotHangCount_R > PEAK_HANG) { //Peak pause length
    if(++dotCount_R >= PEAK_FALL) { //Fall rate
      peak_R++;
      dotCount_R = 0;
    }
  } else {
    dotHangCount_R++;
  }
}
//End of loop

//Extends
//Used to draw a line between two points of a given color
void drawLine_L(uint8_t from, uint8_t to, uint32_t c) {
  uint8_t fromTemp;
  if (from > to) {
    fromTemp = from;
    from = to;
    to = fromTemp;
  }
  for(int l=from; l<=to; l++){
    strip_L.setPixelColor(l, c);
  }
}

void drawLine_R(uint8_t from, uint8_t to, uint32_t c) {
  uint8_t fromTemp;
  if (from > to) {
    fromTemp = from;
    from = to;
    to = fromTemp;
  }
  for(int r=from; r<=to; r++){
    strip_R.setPixelColor(r, c);
  }
}

float fscale(float originalMin, float originalMax, float newBegin, float newEnd, float inputValue, float curve) {
  float OriginalRange = 0;
  float NewRange = 0;
  float zeroRefCurVal = 0;
  float normalizedCurVal = 0;
  float rangedValue = 0;
  boolean invFlag = 0;
  //condition curve parameter
  //limit range
  if (curve > 10) curve = 10;
  if (curve < -10) curve = -10;
  curve = (curve * -.1) ; // - invert and scale - this seems more intuitive - postive numbers give more weight to high end on output
  curve = pow(10, curve); // convert linear scale into lograthimic exponent for other pow function
  /*
  Serial.println(curve * 100, DEC); // multply by 100 to preserve resolution
  Serial.println();
  */
  // Check for out of range inputValues
  if (inputValue < originalMin) {
    inputValue = originalMin;
  }
  if (inputValue > originalMax) {
    inputValue = originalMax;
  }
  //Zero Refference the values
  OriginalRange = originalMax - originalMin;
  if (newEnd > newBegin){
    NewRange = newEnd - newBegin;
  } else {
    NewRange = newBegin - newEnd;
    invFlag = 1;
  }
  zeroRefCurVal = inputValue - originalMin;
  normalizedCurVal = zeroRefCurVal / OriginalRange; // normalize to 0 - 1 float
  //Check for originalMin > originalMax - the math for all other cases i.e. negative numbers seems to work out fine
  if (originalMin > originalMax ) {
    return 0;
  }
  if (invFlag == 0){
    rangedValue = (pow(normalizedCurVal, curve) * NewRange) + newBegin;
  } else { 
    // invert the ranges
    rangedValue = newBegin - (pow(normalizedCurVal, curve) * NewRange);
  } 
  return rangedValue;
}
//Input a value 0 to 255 to get a color value.
//The colours are a transition r - g - b - back to r.
uint32_t Wheel_L(byte WheelPos) {
  if(WheelPos < 85) {
    return strip_L.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
    WheelPos -= 85;
    return strip_L.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
    WheelPos -= 170;
    return strip_L.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
uint32_t Wheel_R(byte WheelPos) {
  if(WheelPos < 85) {
    return strip_R.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
    WheelPos -= 85;
    return strip_R.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
    WheelPos -= 170;
    return strip_R.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
//Init effect
// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;
  for(j=0; j<256*3; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip_L.numPixels(); i++) {
      strip_L.setPixelColor(i, Wheel_L(((i * 256 / strip_L.numPixels()) + j) & 255));
      strip_R.setPixelColor(i, Wheel_R(((i * 256 / strip_R.numPixels()) + j) & 255));
    }
    strip_L.setBrightness(dimT);
    strip_R.setBrightness(dimT);
    strip_L.show();
    strip_R.show();
    delay(wait);
  }
}
