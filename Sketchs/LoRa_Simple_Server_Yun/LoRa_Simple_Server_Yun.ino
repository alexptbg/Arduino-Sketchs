#define BAUDRATE 115200

#include <Console.h>
#include <Process.h>
#include <Bridge.h>
#include <SPI.h>
#include <RH_RF95.h>
#include <string.h>

RH_RF95 rf95;

int led = A2;
int blue = 3;
float frequency = 868.0;

int power = 0;
String data = "";
String device = "";
String sensor = "";

void setup() {
  pinMode(led, OUTPUT);
  pinMode(blue, OUTPUT); 
  Bridge.begin(BAUDRATE);
  Console.begin();
  //while (!Console) ; // Wait for console port to be available
  Console.println("Start Sketch");
  if (!rf95.init())
    Console.println("init failed");
  // Setup ISM frequency
  rf95.setFrequency(frequency);
  // Setup Power,dBm
  rf95.setTxPower(23);
  // Defaults BW Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
  Console.print("Listening on frequency: ");
  Console.println(frequency);
}

void loop() {
  if (rf95.available()) {
    
    // Should be a message for us now   
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf95.recv(buf, &len)) {
      digitalWrite(led, HIGH);
      digitalWrite(blue, HIGH);
      power = rf95.lastRssi(),DEC;
      RH_RF95::printBuffer("request: ", buf, len);
      data = String((char*)buf);
      Console.print("got request: ");
      Console.println(data);
      //Console.println((char*)buf);
      Console.print("RSSI: ");
      Console.println(power);
      
      //split data test
      device = getValue(data,',',1);
      sensor = getValue(data,',',2); 
      //send data
      send_data();
  
      // Send a reply
      uint8_t data[] = "And hello back to you";
      rf95.send(data, sizeof(data));
      rf95.waitPacketSent();
      Console.println("Sent a reply");
      digitalWrite(led, LOW);
      digitalWrite(blue, LOW);
    } else {
      Serial.println("recv failed");
    }
  }
}

// This function call the linkmysql.lua
void send_data() {
  Process logdata;
  logdata.begin("lua");
  logdata.addParameter("/root/linkmysql.lua");
  logdata.addParameter(String(device));
  logdata.addParameter(String(sensor));
  logdata.addParameter(String(power));
  logdata.run();
  //read the output of the command
  while (logdata.available() > 0) {
    char c = logdata.read();
  }
  Console.println("send data done.");
}

String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;
  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }
  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

