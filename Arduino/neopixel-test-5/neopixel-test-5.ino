/*
 * Neopixel 8x8 matrix + microphone + potentiometar
 * 2023-11-22
*/
#include <fix_fft.h>    //library for fft 
#include <Adafruit_NeoPixel.h>
#define N_PIXELS  64  // Number of pixels in strand
#define PIN 6
Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_PIXELS, PIN, NEO_GRB + NEO_KHZ800);

//Color definitions
/*
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
*/
int audioPin = A0;
char im[128], data[128], data_avgs[14]; //FFt variables
int i, j;

int jcol[8][8] = { //Keep track of snaking neopixels up and down by reversing every other column.
  
{0, 1, 2, 3, 4, 5, 6, 7},
{8, 9, 10, 11, 12, 13, 14, 15},
{16, 17, 18, 19, 20, 21, 22, 23},
{24, 25, 26, 27, 28, 29, 30, 31},
{32, 33, 34, 35, 36, 37, 38, 39},
{40, 41, 42, 43, 44, 45, 46, 47},
{48, 49, 50, 51, 52, 53, 54, 55},
{56, 57, 58, 59, 60, 61, 62, 63}
};

void setup(){
  //Serial.begin(9600); // hardware serial
  strip.begin();
}

void loop() {
  int sensorValue = analogRead(A1); //read pot 1
  //int sensorValue2 = analogRead(A2); //read pot 2
  //int sensorValue = 300; //read pot 1
  int sensorValue2 = 1000; //read pot 2
  int sensitivity = map(sensorValue, 0, 1023, 5, 100); //map pot 1 to sensitivity between 5 and 100
  int brightness = map(sensorValue2, 0, 1023, 0, 100); //map pot 2 to brightness from 0 to 100
  // Build table of audio samples
  for (i = 0; i < 128; i++) {
    data[i] = analogRead(audioPin);
    im[i] = 0;
  }

  fix_fft(data, im, 7, 0);  // FFT processing

  for (i = 0; i < 64; i++)
    data[i] = sqrt(data[i] * data[i] + im[i] * im[i]); // Make values positive

  for (i = 0; i < 14; i++) {
    // Average values
    data_avgs[i] = (data[i*4] + data[i*4 + 1] + data[i*4 + 2] + data[i*4 + 3]);   // average together
    if (i == 0)
      data_avgs[i] >>= 1;  // KK: De-emphasize first audio band (too sensitive)
    data_avgs[i] = map(data_avgs[i], 0, sensitivity, 0, 9);
  }
    for(int x = 0; x < 8; x++) //rows
    {
      for(int y = 0; y < 8; y++) //cols
      {
        //int z = x * 2;
        if(y < data_avgs[8-x])
        {          
          if(y < 3) //blue
          {
            strip.setPixelColor(jcol[x][y], 0*brightness/100, 0*brightness/100, 255*brightness/100);
          }
          else if(y < 5) //green
          {
            strip.setPixelColor(jcol[x][y], 0*brightness/100, 153*brightness/100, 0*brightness/100);        
          }   
          else if(y < 7) //yellow
          {
            strip.setPixelColor(jcol[x][y], 255*brightness/100, 255*brightness/100, 0*brightness/100);          
          }
          else if(y < 8) //orange
          {
            strip.setPixelColor(jcol[x][y], 255*brightness/100, 100*brightness/100, 0*brightness/100);         
          }
          else //red
          {
            strip.setPixelColor(jcol[x][y], 255*brightness/100, 0, 0);  
          }
        }
        else
        {
          strip.setPixelColor(jcol[x][y], 0, 0, 0);   
        }
      }
    }
    
    strip.show();
    //delay(20);
}
