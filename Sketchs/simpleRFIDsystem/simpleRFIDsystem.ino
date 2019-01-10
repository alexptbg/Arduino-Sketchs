/* Simple RFID access system
 John Boxall - tronixstuff.com/projects 
 4th Feb 2011 - CC by-sa-nc 
 18 March 2013 */

int newtag[14] = {
  0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // used for read comparisons
int testtag[14]= {
  0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // used for read comparisons
int ok=0;
char data1;
#include "Wire.h" // to use I2C bus
#define eeprom 0x50 // I2C bus address for the EEPROM chip
int ondelay=1000; // relay activation time when correct tag read

void setup()
{
  Serial.begin(9600); // enable serial port to receive data from RFID reader
  Wire.begin();                 // start the I2C bus
  Serial.flush();               // need to flush serial buffer, otherwise first tag read from reset/power on may not be correct
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, INPUT);
  pinMode(12, INPUT);
  pinMode(13, OUTPUT);
  pinMode(14, OUTPUT); // using analog0 as a digital output
}

boolean comparetag(int aa[14], int bb[14])
//  compares two arrrays, returns true if identical - good for comparing tags
{
  boolean ff=false;
  int fg=0;
  for (int cc=0; cc<14; cc++)
  {
    if (aa[cc]==bb[cc])
    {
      fg++;
    }
  }
  if (fg==14)
  {
    ff=true;
  }
  return ff;
}

byte readData(int device, unsigned int add) // reads a byte of data from memory location 'add' in chip at I2C address 'device'
{
  byte result;  // returned value
  Wire.beginTransmission(device); //  these three lines set the pointer position in the EEPROM
  Wire.write((int)(add >> 8));   // left-part of pointer address
  Wire.write((int)(add & 0xFF)); // and the right
  Wire.endTransmission();
  Wire.requestFrom(device,1); // now get the byte of data...
  result = Wire.read();  
  return result; // and return it as a result of the function readData
}

void writeData(int device, unsigned int add, byte data)
// writes a byte of data 'data' to the eeprom at I2C address 'device', in memory location 'add'
{
  Wire.beginTransmission(device);
  Wire.write((int)(add >> 8));   // left-part of pointer address
  Wire.write((int)(add & 0xFF)); // and the right
  Wire.write(data);
  Wire.endTransmission();
  delay(10);
}

void readEEPROMtag(int tagpos)
// reads the stored tag number for tag # tagpos from the EEPROM
{
  int q1=0;
  q1=tagpos*14;
  for (int q=0; q<14; q++)
  {
    testtag[q]=readData(eeprom,q+q1);
  }
}

void writeEEPROMtag(int tagpos, int tt[14])
// writes tag data to tag slot# tagpos
{
  int q1=0;
  q1=tagpos*14;
  for (int q=0; q<14; q++)
  {
    writeData(eeprom,q+q1,tt[q]);
  }
}


void readTags()
{
  if (Serial.available() > 0) // if a read has been attempted
  {
    // we get the numbers of the tag being read and store in newtag[]
    delay(100);  // Needed to allow time for the data to come in from the serial buffer. 
    for (int z=0; z<14; z++) // read the rest of the tag
    {
      data1=Serial.read();
      newtag[z]=data1;
    }
    // now to match tags up
    ok=0;
    for (int z=0; z<8; z++) // we check newtag[] against the tag data stored in the EEPROM
    {
      readEEPROMtag(z); // reads tag number LOOP from EEPROM and puts it into testtag
      if (comparetag(testtag,newtag)==true)
      {
        ok=1;  
        digitalWrite(10, LOW);
        digitalWrite(9, HIGH);
        if (z==0)
        {
          digitalWrite(14, HIGH);
        } 
        else if (z!=0)
        {        
          digitalWrite(z+1, HIGH);
        }
        digitalWrite(13, HIGH);
        delay(ondelay);
        digitalWrite(10, HIGH);
        if (z==0)
        {
          digitalWrite(14, LOW);
        } 
        else if (z!=0)
        {        
          digitalWrite(z+1, LOW);
        }
        digitalWrite(13, LOW);
        digitalWrite(9, LOW);
      }
    }
    if (ok==0) // incorrect tag read
    {
      for (int aa=0; aa<5; aa++)
      {
        digitalWrite(9, HIGH);
        digitalWrite(10, HIGH);
        delay(250);
        digitalWrite(9, LOW);
        digitalWrite(10, LOW);
        delay(250);
      }
    }
    Serial.flush(); // stops multiple reads
  }
}


void learnTags()
{
  int escape = 0;
  int tagloc = 0;
  delay(400); // for debounce
  digitalWrite(9, HIGH);
  digitalWrite(10,HIGH);
  if (Serial.available() > 0) // if a read has been attempted
  {
    // read the incoming number on serial RX
    delay(100);  // Needed to allow time for the data to come in from the serial buffer. 
    for (int z=0; z<14; z++) // read the rest of the tag
    {
      data1=Serial.read();
      newtag[z]=data1;
    }
  }
  Serial.flush();               // need to flush serial buffer
  for (int aa=0; aa<2; aa++)
  {
    digitalWrite(9, HIGH);
    digitalWrite(10, HIGH);
    delay(250);
    digitalWrite(9, LOW);
    digitalWrite(10, LOW);
    delay(250);
  }
  do
  {
    if (tagloc==0)
    {
      digitalWrite(14, HIGH);
    } 
    else if (tagloc!=0)
    {        
      digitalWrite(tagloc+1, HIGH);
    }
    digitalWrite(13, LOW);
    digitalWrite(9, LOW);
    if (digitalRead(11)==HIGH)
    {
      delay(400); // for debounce
      if (tagloc==0)
      {
        digitalWrite(14, LOW);
      } 
      else if (tagloc!=0)
      {        
        digitalWrite(tagloc+1, LOW);
      }
      digitalWrite(13, LOW);
      digitalWrite(9, LOW);
      tagloc++;
      if (tagloc>7)
      { 
        tagloc=0; 
      }

      if (tagloc==0)
      {
        digitalWrite(14, LOW);
      } 
      else if (tagloc!=0)
      {        
        digitalWrite(tagloc+1, LOW);
      }
      digitalWrite(13, LOW);
      digitalWrite(9, LOW);
    }
    if (digitalRead(12)==HIGH)
    {
      delay(400); // for debounce
      writeEEPROMtag(tagloc,newtag);
      escape=1;
      if (tagloc==0)
      {
        digitalWrite(14, LOW);
      } 
      else if (tagloc!=0)
      {        
        digitalWrite(tagloc+1, LOW);
      }
    }
  }
  while (escape!=1);
  Serial.flush();              
}

void loop() // the simple bit...
{
  digitalWrite(10, HIGH);
  readTags();
  if (digitalRead(11)==HIGH)
  {
    learnTags();
  }
}




