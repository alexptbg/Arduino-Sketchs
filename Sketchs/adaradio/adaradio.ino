#include <Wire.h>
#include <Adafruit_Si4713.h>

#define RESETPIN 2

#define FMSTATION 9590      // 10230 == 102.30 MHz

Adafruit_Si4713 radio = Adafruit_Si4713(RESETPIN);

void setup() {
  Serial.begin(9600);
  Serial.println("Adafruit Radio - Si4713 Test");

  if (! radio.begin()) {  // begin with address 0x63 (CS high default)
    Serial.println("Couldn't find radio?");
    while (1);
  }

  for (uint16_t f  = 8750; f<10800; f+=10) {
   radio.readTuneMeasure(f);
   Serial.print("Measuring "); Serial.print(f); Serial.print("...");
   radio.readTuneStatus();
   Serial.println(radio.currNoiseLevel);
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

  radio.setGPIOctrl(_BV(1) | _BV(2));  // set GP1 and GP2 to output
}



void loop() {
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
