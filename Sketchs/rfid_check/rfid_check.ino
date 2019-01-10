#include <SoftwareSerial.h>
int val;
const int ledPin = 12;
const int rxpin = 2;
const int txpin = 3;
SoftwareSerial mySerial(rxpin, txpin);
int incomingByte = 0; 
int led_state=0;

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  Serial.println("RFID Ready!");
  pinMode(ledPin, OUTPUT);
}

void loop() 
{
    if(RFID()==1)
      {
      if(led_state==0)
        {
        digitalWrite(ledPin, HIGH);
        led_state=1;
        }
      else
        {
        digitalWrite(ledPin, LOW);
        led_state=0;
        }
      while(RFID()==1);          
      }
 
}

int RFID() 
  {
  byte i = 0;
  byte val = 0;
  byte code[6];
  byte checksum = 0;
  byte bytesread = 0;
  byte tempbyte = 0;
  int result = 0;
  int z = 0;

  if(mySerial.available()) {
    result=1;
    //Serial.println(mySerial.read());
    if((val = mySerial.read()) == 0x02) {// check for header 
      bytesread = 0; 
      Serial.println("I read it");
      while (bytesread < 12) {// read 10 digit code + 2 digit checksum
        if(mySerial.available()) { 
          val = mySerial.read();
          //Serial.println(val,HEX);
          if((val == 0x0D)||(val == 0x0A)||(val == 0x03)||(val == 0x02)) { // if header or stop bytes 
            break; // stop reading
          }

          // Do Ascii/Hex conversion:
          if ((val >= '0') && (val <= '9')) {
            val = val - '0';
          } else if ((val >= 'A') && (val <= 'F')) {
            val = 10 + val - 'A';
          }

          // Every two hex-digits, add byte to code:
          if (bytesread & 1 == 1) {
            // make some space for this hex-digit by
            // shifting the previous hex-digit with 4 bits to the left:
            code[bytesread >> 1] = (val | (tempbyte << 4));

            if (bytesread >> 1 != 5) {// If we're at the checksum byte,
              checksum ^= code[bytesread >> 1]; // Calculate the checksum... (XOR)
            };
          } else {
            tempbyte = val; // Store the first hex digit first...
          };
          bytesread++; // ready to read next digit
        } 
      } 

      // Output to Serial:

      if (bytesread == 12) { // if 12 digit read is complete
        Serial.print("5-byte code: ");
        for (i=1; i<5; i++) {
          if (code[i] < 16) mySerial.print("0");
          Serial.print(code[i],HEX);
          Serial.print("");
        }
        Serial.println();
        digitalWrite(ledPin, LOW);
        Serial.print("Checksum: ");
        Serial.print(code[5],HEX);
        Serial.println(code[5] == checksum ? " -- passed." : " -- error.");
        if(code[5] == checksum)
        result=1;
        Serial.println();  
      }
      bytesread = 0;
    }
  }
  return result;
}
