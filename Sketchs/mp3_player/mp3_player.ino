#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>
#include <Wire.h>
#include <Adafruit_Si4713.h>
#include <Adafruit_NeoPixel.h>

#define RESETPIN 2

#define LEDPIN 6
#define NUMPIXELS 1

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, LEDPIN, NEO_GRB + NEO_KHZ800);

#define FMSTATION 9590      // 10230 == 102.30 MHz

Adafruit_Si4713 radio = Adafruit_Si4713(RESETPIN);

#define BREAKOUT_RESET  9      // VS1053 reset pin (output)
#define BREAKOUT_CS     10     // VS1053 chip select pin (output)
#define BREAKOUT_DCS    8      // VS1053 Data/command select pin (output)

// These are the pins used for the music maker shield
#define SHIELD_CS     7      // VS1053 chip select pin (output)
#define SHIELD_DCS    6      // VS1053 Data/command select pin (output)

// These are common pins between breakout and shield
#define CARDCS 4     // Card chip select pin
// DREQ should be an Int pin, see http://arduino.cc/en/Reference/attachInterrupt
#define DREQ 3       // VS1053 Data request, ideally an Interrupt pin

Adafruit_VS1053_FilePlayer musicPlayer = 
  // create breakout-example object!
  Adafruit_VS1053_FilePlayer(BREAKOUT_RESET, BREAKOUT_CS, BREAKOUT_DCS, DREQ, CARDCS);
  // create shield-example object!
  //Adafruit_VS1053_FilePlayer(SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);

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
  strip.setBrightness(25);
  Serial.begin(9600);
  Serial.println("Adafruit VS1053 Simple Test");
  if (!musicPlayer.begin()) { // initialise the music player
     Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
     while (1);
  }
  musicPlayer.sineTest(0x44, 500);    // Make a tone to indicate VS1053 is working
  musicPlayer.setVolume(20,20);
  //if (! musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT))
  //  Serial.println(F("DREQ pin is not an interrupt pin"));
  Serial.println("Adafruit Radio - Si4713 Test");
  if (! radio.begin()) {  // begin with address 0x63 (CS high default)
    Serial.println("Couldn't find radio?");
    while (1);
  }

  // Uncomment to scan power of entire range from 87.5 to 108.0 MHz
  /*
  for (uint16_t f  = 8750; f<10800; f+=10) {
   radio.readTuneMeasure(f);
   Serial.print("Measuring "); Serial.print(f); Serial.print("...");
   radio.readTuneStatus();
   Serial.println(radio.currNoiseLevel);
   }
   */

  Serial.print("\nSet TX power");
  radio.setTXpower(115);  // dBuV, 88-115 max

  Serial.print("\nTuning into "); 
  Serial.print(FMSTATION/100); 
  Serial.print('.'); 
  Serial.println(FMSTATION % 100);
  radio.tuneFM(FMSTATION); // 102.3 mhz

  // This will tell you the status in case you want to read it from the chip
  radio.readTuneStatus();
  Serial.print("\tCurr freq: "); 
  Serial.println(radio.currFreq);
  Serial.print("\tCurr freqdBuV:"); 
  Serial.println(radio.currdBuV);
  Serial.print("\tCurr ANTcap:"); 
  Serial.println(radio.currAntCap);

  // begin the RDS/RDBS transmission
  radio.beginRDS();
  radio.setRDSstation("AdaRadio");
  radio.setRDSbuffer( "Adafruit g0th Radio!");

  Serial.println("RDS on!");  

  radio.setGPIOctrl(_BV(1) | _BV(2));  // set GP1 and GP2 to output
  Serial.println(F("VS1053 found"));
  
  SD.begin(CARDCS);    // initialise the SD card
  

  printDirectory(SD.open("/"), 0);
  // Timer interrupts are not suggested, better to use DREQ interrupt!
  //musicPlayer.useInterrupt(VS1053_FILEPLAYER_TIMER0_INT); // timer int

  // If DREQ is on an interrupt pin (on uno, #2 or #3) we can do background
  // audio playing
  musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);  // DREQ int
  
  // Play one file, don't return until complete
  //Serial.println(F("Playing track 001"));
  //musicPlayer.playFullFile("t01.mp3");
  
  // Play another file in the background, REQUIRES interrupts!
  //Serial.println(F("Playing track 002"));
  //musicPlayer.startPlayingFile("t02.mp3");
  // Play another file in the background, REQUIRES interrupts!
  //Serial.println(F("Playing track 003"));
  //musicPlayer.startPlayingFile("t03.mp3");
  // Play another file in the background, REQUIRES interrupts!
  //Serial.println(F("Playing track 004"));
  //musicPlayer.startPlayingFile("t04.mp3");
}

void loop() {
  Serial.println(F("Playing track 002"));
  musicPlayer.startPlayingFile("t02.mp3");
  while (musicPlayer.playingMusic) {
    strip.setPixelColor(0,green);
    strip.show();
    // file is now playing in the 'background' so now's a good time
    // to do something else like handling LEDs or buttons :)
    Serial.print(".");
    delay(500);
  }
  //Play another file in the background, REQUIRES interrupts!
  Serial.println(F("Playing track 004"));
  musicPlayer.startPlayingFile("t04.mp3");
  while (musicPlayer.playingMusic) {
    strip.setPixelColor(0,green);
    strip.show();
    // file is now playing in the 'background' so now's a good time
    // to do something else like handling LEDs or buttons :)
    Serial.println(".");
  radio.readASQ();
  Serial.print("\tCurr ASQ: 0x"); 
  Serial.println(radio.currASQ, HEX);
  Serial.print("\tCurr InLevel:"); 
  Serial.println(radio.currInLevel);
    delay(500);
  }
  // Play another file in the background, REQUIRES interrupts!
  //Serial.println(F("Playing track 004"));
  //musicPlayer.startPlayingFile("t04.mp3");
  // File is playing in the background
  if (musicPlayer.stopped()) {
    Serial.println("Done playing music");
    strip.setPixelColor(0,red);
    strip.show();
    while (1);
  }
  if (Serial.available()) {
    char c = Serial.read();
    
    // if we get an 's' on the serial console, stop!
    if (c == 's') {
      musicPlayer.stopPlaying();
    }
    
    // if we get an 'p' on the serial console, pause/unpause!
    if (c == 'p') {
      if (! musicPlayer.paused()) {
        Serial.println("Paused");
        musicPlayer.pausePlaying(true);
      } else { 
        Serial.println("Resumed");
        musicPlayer.pausePlaying(false);
      }
    }
  }

  delay(100);
  radio.readASQ();
  Serial.print("\tCurr ASQ: 0x"); 
  Serial.println(radio.currASQ, HEX);
  Serial.print("\tCurr InLevel:"); 
  Serial.println(radio.currInLevel);
  // toggle GPO1 and GPO2
  radio.setGPIO(_BV(1));
  delay(500);
  radio.setGPIO(_BV(2));
  delay(500);
}

/// File listing helper
void printDirectory(File dir, int numTabs) {
   while(true) {
     
     File entry =  dir.openNextFile();
     if (! entry) {
       // no more files
       //Serial.println("**nomorefiles**");
       break;
     }
     for (uint8_t i=0; i<numTabs; i++) {
       Serial.print('\t');
     }
     Serial.print(entry.name());
     if (entry.isDirectory()) {
       Serial.println("/");
       printDirectory(entry, numTabs+1);
     } else {
       // files have sizes, directories do not
       Serial.print("\t\t");
       Serial.println(entry.size(), DEC);
     }
     entry.close();
   }
}
