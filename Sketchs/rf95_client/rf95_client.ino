#include <SPI.h>
#include <RH_RF95.h>
#include <Wire.h>
#include <Adafruit_MCP9808.h>

#define LED 13
#define RFM95_CS 10
#define RFM95_RST 9
#define RFM95_INT 2
#define node_id "A"

#define RF95_FREQ 868.0

RH_RF95 rf95(RFM95_CS, RFM95_INT);

Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();

void setup() {
  pinMode(LED,OUTPUT);
  digitalWrite(LED,LOW);
  pinMode(RFM95_RST,OUTPUT);
  digitalWrite(RFM95_RST,HIGH);

  //while (!Serial);
  Serial.begin(9600);
  delay(100);

  Serial.println("Arduino LoRa TX Test!");

  digitalWrite(RFM95_RST,LOW);
  delay(10);
  digitalWrite(RFM95_RST,HIGH);
  delay(10);

  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  Serial.println("LoRa radio init OK!");

  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); 
  Serial.println(RF95_FREQ);
  rf95.setTxPower(23,false);

  if (!tempsensor.begin()) {
    Serial.println("Couldn't find MCP9808!");
    while (1);
  }
}

int16_t packetnum = 0;

void loop() {
  tempsensor.shutdown_wake(0);   // Don't remove this line! required before reading temp
  float c = tempsensor.readTempC();
  Serial.print("Temp: "); Serial.println(c);
  delay(250);
  tempsensor.shutdown_wake(1); // shutdown MSP9808 - power consumption ~0.1 mikro Ampere
  
  Serial.println("Sending to rf95_server");

  String radiopacket = "";
  radiopacket += String(packetnum++);
  radiopacket += ",";
  radiopacket += node_id;
  radiopacket += ",";
  radiopacket += String(c);

  Serial.print("Sending "); 
  Serial.println(radiopacket); 
  delay(10);
  rf95.send((uint8_t*)radiopacket.c_str(), radiopacket.length()+1);

  Serial.println("Waiting for packet to complete..."); 
  delay(10);
  rf95.waitPacketSent();
  
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  Serial.println("Waiting for reply..."); 
  delay(10);
  
  if (rf95.waitAvailableTimeout(1000)) {
    if (rf95.recv(buf, &len)) {
      digitalWrite(LED,HIGH);
      Serial.print("Got reply: ");
      Serial.println((char*)buf);
      Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(),DEC);
      delay(500);
      digitalWrite(LED,LOW);
    } else {
      Serial.println("Receive failed");
      digitalWrite(LED,LOW);
    }
  } else {
    Serial.println("No reply, is there a listener around?");
      digitalWrite(LED,LOW);
  }

  delay(15000); 
}
