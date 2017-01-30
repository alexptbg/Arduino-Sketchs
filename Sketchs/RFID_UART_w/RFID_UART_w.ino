#include <Arduino.h>

#include <SoftwareSerial.h>
#include <SeeedRFID.h>
#include <SPI.h>

#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"

#define BUFSIZE                        128   // Size of the read buffer for incoming data
#define VERBOSE_MODE                   true  // If set to 'true' enables debug output

#define BLUEFRUIT_SPI_CS               8
#define BLUEFRUIT_SPI_IRQ              7
#define BLUEFRUIT_SPI_RST              4    // Optional but recommended, set to -1 if unused

#define FACTORYRESET_ENABLE         1
#define MODE_LED_BEHAVIOUR          "MODE"

Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

#define RFID_RX_PIN 10
#define RFID_TX_PIN 11

#define LED    12
#define SOUND    13

//#define DEBUG
#define TEST

SeeedRFID RFID(RFID_RX_PIN, RFID_TX_PIN);
RFIDdata tag;

void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}

void setup() {
	Serial.begin(57600);
	
	Serial.println("Hello, double bk!");
  /* Initialise the module */
  Serial.print(F("Initialising the Bluefruit LE module: "));

  if ( !ble.begin(VERBOSE_MODE) )
  {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }
  Serial.println( F("OK!") );

  if ( FACTORYRESET_ENABLE )
  {
    /* Perform a factory reset to make sure everything is in a known state */
    Serial.println(F("Performing a factory reset: "));
    if ( ! ble.factoryReset() ){
      error(F("Couldn't factory reset"));
    }
  }

  /* Disable command echo from Bluefruit */
  
  ble.echo(false);

  Serial.println("Requesting Bluefruit info:");
  /* Print Bluefruit information */
  
  ble.info();

  ble.verbose(false);  // debug info is a little annoying after this point!

  /* Wait for connection */
  
  while (!ble.isConnected()) {
      delay(300);
  }
  
  Serial.println(F("******************************"));


    ble.sendCommandCheckOK("AT+HWModeLED=" MODE_LED_BEHAVIOUR);

    ble.sendCommandCheckOK("AT+BLEPOWERLEVEL=-4");
  // Set module to DATA mode
  Serial.println( F("Switching to DATA mode!") );
  ble.setMode(BLUEFRUIT_MODE_DATA);
  delay(100);
  if (!ble.sendCommandCheckOK(F("AT+GAPDEVNAME=BLUE_RFID")) ) {
    error(F("Could not set device name?"));
  }

  Serial.println(F("******************************"));
  
  pinMode(LED, OUTPUT);
  pinMode(SOUND, OUTPUT);
  digitalWrite(LED, LOW);
  digitalWrite(SOUND, LOW);
}

void loop() { 
	if(RFID.isAvailable()){
		tag = RFID.data();
		Serial.print("RFID card number: ");
		Serial.println(RFID.cardNumber());
    
    #ifdef TEST
    String test = "";
	  Serial.print("RFID raw data: ");
	  for(int i=0; i<4; i++){
    
      if (tag.raw[i] < 16) {
        test += '0'+String(tag.raw[i],HEX);
      } else {
        test += String(tag.raw[i],HEX);
      }
      
	    Serial.print(tag.raw[i],HEX);
	    Serial.print('\t');
		}
    Serial.println();
    Serial.println(test);
    Serial.println(String(StrToInt(test)));
    #endif
    //ble.print(String(StrToInt(test)));
    digitalWrite(LED, HIGH);
    tone(SOUND, 2500, 50);
    delay(50);
    tone(SOUND, 4000, 80);
    delay(100);
    digitalWrite(LED, LOW);
	}
 
  while ( ble.available() )
  {
    int c = ble.read();
    Serial.print((char)c);
    /*
    // Hex output too, helps w/debugging!
    Serial.print(" [0x");
    if (c <= 0xF) Serial.print(F("0"));
    Serial.print(c, HEX);
    Serial.print("] ");
    */
  }
  
}

unsigned long StrToInt(String ValAsString) {
  char ch = ' ';
  byte bt = 0;
  unsigned long ival = 0;
  for (byte i = 0; i < ValAsString.length(); i++) {
    ch = ValAsString[i];
    bt = ch;
    if ((ch >= 48 && ch <= 57) || (ch >= 97 && ch <= 102)) {
      ival = ival * 16;
      if (bt < 97) {
        bt = bt - 48;
      } else {
        bt = bt - 87;
      }
      ival = ival + bt;
    } else {
      ival = 0;
      return ival;
    }
  }
  return ival;
}

unsigned int hexToDec(String hexString) {
  unsigned int decValue = 0;
  int nextInt;
  for (int i = 0; i < hexString.length(); i++) {
    nextInt = int(hexString.charAt(i));
    if (nextInt >= 48 && nextInt <= 57) nextInt = map(nextInt, 48, 57, 0, 9);
    if (nextInt >= 65 && nextInt <= 70) nextInt = map(nextInt, 65, 70, 10, 15);
    if (nextInt >= 97 && nextInt <= 102) nextInt = map(nextInt, 97, 102, 10, 15);
    nextInt = constrain(nextInt, 0, 15);
    
    decValue = (decValue * 16) + nextInt;
  }
  return decValue;
}

