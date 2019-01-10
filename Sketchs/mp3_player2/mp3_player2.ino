#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>
#include <Wire.h>
#include <Adafruit_Si4713.h>

#define RESETPIN 2

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

int volume = 15;

int fileIndex = 0;
int fileNumber = 0;

int potPin = 0;

char* fileList[10];

void setup()
{
  
  Serial.begin(9600);

  Serial.println(F("[DEBUG] - MP3 files Player based on Adafruit VS1053 shield"));

  if (!musicPlayer.begin())
  {
     Serial.println(F("[ERROR] - Couldn't find VS1053 (see the pins defined)"));
     while(1);
  }
  Serial.println(F("[DEBUG] - VS1053 found"));

  // initialise the SD card
  SD.begin(CARDCS);

  // timer interrupts are not suggested, better to use DREQ interrupt!
  //musicPlayer.useInterrupt(VS1053_FILEPLAYER_TIMER0_INT); // timer int

  // if DREQ is on an interrupt pin (on uno, #2 or #3) we can do background audio playing
  musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);  // DREQ int

  // set volume for left, right channels. lower numbers == louder volume!
  musicPlayer.setVolume(volume, volume);

  createFileList();

  Serial.print(F("[DEBUG] - Number of file="));
  Serial.println(fileNumber);

  if (!fileList[fileIndex])
  {
     Serial.println(F("[DEBUG] - File not found in directory"));
     while(1);
  }
  Serial.print(F("[DEBUG] - Reading="));
  Serial.println(fileList[fileIndex]);

  musicPlayer.startPlayingFile(fileList[fileIndex]);
/*
  if (! radio.begin()) {  // begin with address 0x63 (CS high default)
    Serial.println("Couldn't find radio?");
    while (1);
  }
  
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

  radio.setGPIOctrl(_BV(1) | _BV(2)); 
  */
}

void loop()
{
  if (Serial.available())
  {
    char c = Serial.read();

    updateVolume(c);

    updateState(c);p
    

    Serial.print(F("[DEBUG] - Reading="));
    Serial.println(fileList[fileIndex]);
  }
  delay(500);
  //new
  Serial.print(F("[DEBUG] - TPIN="));
  int reading = analogRead(potPin);
  Serial.println(reading);
  /*
  radio.readASQ();
  Serial.print("\tCurr ASQ: 0x"); 
  Serial.println(radio.currASQ, HEX);
  Serial.print("\tCurr InLevel:"); 
  Serial.println(radio.currInLevel);
  // toggle GPO1 and GPO2
  /*
  radio.setGPIO(_BV(1));
  delay(500);
  radio.setGPIO(_BV(2));
  delay(500);
  */
}

// update or decrease volume if needed
void updateVolume(char command)
{
  if ('u' == command)
  {
      volume--;
  }
  else if ('d' == command)
  {
    volume++;
  }
  else
  {
    return;
  }
  Serial.print("[DEBUG] - Volume=");
  Serial.println(volume);

  musicPlayer.setVolume(volume, volume);
}

// back, forward, pause or stop track if needed
void updateState(char command)
{
  if ('b' == command && fileIndex > 0)
  {
    musicPlayer.startPlayingFile(fileList[--fileIndex]);
    Serial.println(F("[DEBUG] - Back command"));
  }
  if ('f' == command && (fileIndex < fileNumber - 1))
  {
    musicPlayer.startPlayingFile(fileList[++fileIndex]);
    Serial.println(F("[DEBUG] - Forward command"));
  }
  if ('s' == command)
  {
    musicPlayer.stopPlaying();
    Serial.println("[DEBUG] - Stopped");
  }
  if ('p' == command)
  {
    if (!musicPlayer.paused())
    {
      musicPlayer.pausePlaying(true);
      Serial.println(F("[DEBUG] - Paused"));
    }
    else
    {
      musicPlayer.pausePlaying(false);
      Serial.println(F("[DEBUG] - Resumed"));
    }
  }
}

// file listing helper
void createFileList()
{
  File root = SD.open("/");

  for (int i = 0; i < 100; i++)
  {
    File entry = root.openNextFile();

    if (!entry)
    {
      break;
    }
    fileList[i] = strdup(entry.name());

    Serial.print(F("[DEBUG] - Found file="));
    Serial.println(fileList[i]);

    fileNumber++;

    entry.close();
  }
}


