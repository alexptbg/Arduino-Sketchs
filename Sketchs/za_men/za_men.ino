#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <Wire.h>
#include <rgb_lcd.h>
#include <Adafruit_MCP9808.h>
#include <RTClib.h>
#include <Grove_LED_Bar.h>

//ETH
byte mac[] = { 0x90, 0xA2, 0xDA, 0x0F, 0x9F, 0x08 };

//DEVICE
String mac_addr = "90:A2:DA:0F:9F:08";//4
String ID = "AR_0006_2015_1.0"; //ID

//RTC
RTC_DS1307 RTC;
unsigned int localPort = 8888; //Local port to listen for UDP Packets
IPAddress timeServer(128,138,141,172);//NTP Server IP (time.nist.gov)
const int NTP_PACKET_SIZE = 48;  //NTP Time stamp is in the firth 48 bytes of the message
byte packetBuffer[ NTP_PACKET_SIZE];  //Buffer to hold incomming and outgoing packets
EthernetUDP Udp;  //UDP Instance to let us send and recieve packets
unsigned long epoch; //Unix Epoch time (NTP or RTC depending on state)

//MCP
Adafruit_MCP9808 tempsensor1 = Adafruit_MCP9808();

//LCD
rgb_lcd lcd;
const int colorR = 0;
const int colorG = 255;
const int colorB = 255;

//BMP180
#define ADDRESS_SENSOR 0x77
int16_t  ac1, ac2, ac3, b1, b2, mb, mc, md;
uint16_t ac4, ac5, ac6;
float T;
uint8_t osd;
const uint8_t oss = 3;

//TEST
char server[] = "ka-ex.net";    
String dataLocation = "/ip1.php HTTP/1.1";

EthernetClient client;
String currentLine = "";            // string for incoming serial data
String currRates = "";
boolean readingRates = false;       // is reading?
const int requestInterval = 10000; // milliseconds delay between requests

boolean requested;                  // whether you've made a request since connecting
long lastAttemptTime = 0;           // last time you connected to the server, in milliseconds

//CLIENT
EthernetClient clientx;
EthernetClient clienti;
char cserver[] = "eesystems.net";

//LED
Grove_LED_Bar bar(9,8,0);

//CHAR
byte heart[8] = {
  0b00000,
  0b01010,
  0b11111,
  0b11111,
  0b11111,
  0b01110,
  0b00100,
  0b00000
};

byte A[8] = {
  0b01110,
  0b10001,
  0b10001,
  0b11111,
  0b10001,
  0b10001,
  0b10001,
  0b00000
};


//SETUP
void setup() {
  Wire.begin();// Activate I2C
  //display
  lcd.begin(16,2);
  lcd.createChar(0,A);
  lcd.setRGB(colorR,colorG,colorB);
  lcd.setCursor(0,0);
  lcd.print("KA-EX.NET");
  lcd.write((unsigned char)0);
  lcd.setCursor(0,1);
  lcd.print("SOARES  KARADJOV"); 
  //Ethernet
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    bar.setLed(1,1);
  }
  //LED
  bar.setLed(10,1);
  //SERIAL
  Serial.begin(9600); //Start the serial interface
  Serial.print("Server IP: ");
  Serial.println(Ethernet.localIP());
  //RTC
  RTC.begin();  //Start the RTC
  delay(500);
  //check to see if the RTC is already configured
  //if(!RTC.isrunning()) {
    //Serial.println("RTC Not Configured");
    /*
    if(Ethernet.begin(mac) == 0) {
      Serial.println("Fatal Error: Unable to obtain DHCP address");
      for(;;)  //do nothing forever!
        ;      //TODO Change this to retry a few times
    }
    */
    Udp.begin(localPort);
    //get the NTP timestamp
    epoch = getNTP();
    //set the RTC
    RTC.adjust(epoch);
    //display what we did!
    Serial.println("RTC Configured:");
    bar.setLed(9,1);
    //show Unix Epoch
    Serial.print("Unix Epoch: ");
    Serial.println(epoch);
    //show UTC
    //Serial.print("UTC: ");
    //showUTC(epoch);
    delay(100);
  //} else {
    //Serial.println("RTC Already Configured");
  //}
  //MCP
  if(!tempsensor1.begin()) {
    Serial.println("Couldn't find MCP9808!");
    bar.setLed(2,1);
    while(1);
  }
  //BMP
  osd = 26;
  init_SENSOR();
  delay(1400);
  lcd.clear();
  bar.setLed(9,0);
}

//LOOP
void loop() {
  //RTC
  DateTime now = RTC.now();

  int Y = now.year();
  int M = now.month();
  int D = now.day();

  Serial.print(Y);
  Serial.print('-');
  if (M < 10) { Serial.print("0"); }
  Serial.print(M);
  Serial.print('-');
  if (D < 10) { Serial.print("0"); }
  Serial.print(D);
  //Display time
  lcd.setCursor(0,0);
  lcd.print(Y-2000); 
  lcd.setCursor(2,0);
  lcd.print("-"); 
  if (M < 10) {
    lcd.setCursor(3,0);
    lcd.print("0");
    lcd.setCursor(4,0);
    lcd.print(M);
    lcd.setCursor(5,0);
    lcd.print("-");
  } else {
    lcd.setCursor(3,0);
    lcd.print(M);
    lcd.setCursor(4,0);
    lcd.print("-");
  }
  if (D < 10) {
    lcd.setCursor(6,0);
    lcd.print("0");
    lcd.setCursor(7,0);
    lcd.print(D);
  } else {
    lcd.setCursor(6,0);
    lcd.print(D);
  }
  lcd.setCursor(8,0);
  lcd.print(" ");
  Serial.print(" - ");
  showUTC(now.unixtime());
  bar.setLed(6,1);
  //MCP
  float c1 = tempsensor1.readTempC();
  bar.setLed(8,1);
  delay(100);
  bar.setLed(8,0);
  //Serial.print("Temperature 1: ");
  //Serial.print(c1,1);
  //Serial.println(" C");
  //display
  lcd.setCursor(11,0);
  lcd.print(c1,1); 
  lcd.setCursor(15,0);
  lcd.print((char)223);
  //BMP180
  int32_t b5;
  b5 = temperature();
  bar.setLed(7,1);
  delay(100);
  bar.setLed(7,0);
  //Serial.print("Temperature 2: ");
  //Serial.print(T,1);
  //Serial.println(" C");
  lcd.setCursor(11,1);
  lcd.print(T,1); 
  lcd.setCursor(15,1);
  lcd.print((char)223);
  bar.setLed(6,0);
  //TEST
  if (client.connected()) {
    //Serial.println("Connected!#1");
    bar.setLed(6,1);
    if (client.available()) {
      // read incoming bytes:
      char inChar = client.read();
      // add incoming byte to end of line:
      currentLine += inChar;
      Serial.println(currentLine);
      // if you get a newline, clear the line:
      if (inChar == '\n') {
        currentLine = "";
      }
      if (currentLine.endsWith("<ip>")) {
        readingRates = true;
        //Serial.println("Reading:");
      } else if (readingRates) {
        if (!currentLine.endsWith("</ip>")) { //'>' is our ending character
          currRates += inChar;
        } else {
          readingRates = false;
          String ip = currRates.substring(0, currRates.length()-4);
          Serial.println("SEND IP:");
          Serial.println(ip);
          //Split justRates
          //int firstSpaceIndex = justRates.indexOf(" ");
          //int secondSpaceIndex = justRates.indexOf(" ", firstSpaceIndex+1);
          //String firstVal = justRates.substring(0, firstSpaceIndex);
          //String secondVal = justRates.substring(firstSpaceIndex+1, secondSpaceIndex);
          //String thirdVal = justRates.substring(secondSpaceIndex);
          //Send to server
          if (clienti.connect(cserver, 80)) {
            Serial.println("-> Connected IP");
            //Make a HTTP request:
            clienti.print("GET /kas/arduino_up_ip.php");
            //start vars
            clienti.print("?in=");
            clienti.print("1");
            clienti.print("&ip=");
            clienti.print(Ethernet.localIP());
            clienti.print("&mac=");
            clienti.print(mac_addr);
            //end vars
            clienti.println(" HTTP/1.1");
            clienti.print("Host: ");
            clienti.println(cserver);
            clienti.println("Connection: close");
            clienti.println();
            clienti.stop();
            client.stop();
            Serial.println("-> SENT!");
            bar.setLed(5,1);
            bar.setLed(1,0);
          } else {
            Serial.println("-> Connection failed!");
            bar.setLed(5,0);
            bar.setLed(1,1);
          }
          delay(700);
          currRates = "";
          Serial.println("Disconnected");
        }
      }
    }
  } else {
    bar.setLed(6,0);
  }
  delay(700);
}

void init_SENSOR() {
  ac1 = read_2_bytes(0xAA);
  ac2 = read_2_bytes(0xAC);
  ac3 = read_2_bytes(0xAE);
  ac4 = read_2_bytes(0xB0);
  ac5 = read_2_bytes(0xB2);
  ac6 = read_2_bytes(0xB4);
   b1 = read_2_bytes(0xB6);
   b2 = read_2_bytes(0xB8);
   mb = read_2_bytes(0xBA);
   mc = read_2_bytes(0xBC);
   md = read_2_bytes(0xBE);
}

int32_t temperature() {
  int32_t x1, x2, b5, UT;
  Wire.beginTransmission(ADDRESS_SENSOR);
  Wire.write(0xf4);
  Wire.write(0x2e);
  Wire.endTransmission();
  delay(5);
  UT = read_2_bytes(0xf6);
  x1 = (UT - ac6) * ac5 >> 15;
  x2 = ((int32_t)mc << 11) / (x1 + md);
  b5 = x1 + x2;
  T  = (b5 + 8) >> 4;
  T = T / 10.0;
  return b5;  
}

uint8_t read_1_byte(uint8_t code) {
  uint8_t value;
  Wire.beginTransmission(ADDRESS_SENSOR);
  Wire.write(code);
  Wire.endTransmission();
  Wire.requestFrom(ADDRESS_SENSOR,1);
  if(Wire.available() >= 1) { value = Wire.read(); }
  return value;
}

uint16_t read_2_bytes(uint8_t code) {
  uint16_t value;
  Wire.beginTransmission(ADDRESS_SENSOR);
  Wire.write(code);
  Wire.endTransmission();
  Wire.requestFrom(ADDRESS_SENSOR,2);
  if(Wire.available() >= 2) { value = (Wire.read() << 8) | Wire.read(); }
  return value;
}

void showUTC(unsigned long epoch) {
  //hour
  if (((epoch  % 86400L) / 3600) < 10) {
    Serial.print('0');
    Serial.print((epoch  % 86400L) / 3600);
    lcd.setCursor(0,1);
    lcd.print("0");
    lcd.setCursor(1,1);
    lcd.print((epoch  % 86400L) / 3600);
  } else {
    Serial.print((epoch  % 86400L) / 3600);
    lcd.setCursor(0,1);
    lcd.print((epoch  % 86400L) / 3600);
  }
  Serial.print(':');  
  lcd.setCursor(2,1);
  lcd.print(":");
  //Minute
  if (((epoch % 3600) / 60) < 10) {
    Serial.print('0');
    Serial.print((epoch  % 3600) / 60);
    lcd.setCursor(3,1);
    lcd.print("0");
    lcd.setCursor(4,1);
    lcd.print((epoch  % 3600) / 60);
  } else {
    Serial.print((epoch  % 3600) / 60);
    lcd.setCursor(3,1);
    lcd.print((epoch  % 3600) / 60);
  }
  Serial.print(':'); 
  lcd.setCursor(5,1);
  lcd.print(":");
  //Secconds
  if ((epoch % 60) < 10) {
    Serial.print('0');
    Serial.println(epoch %60);
    lcd.setCursor(6,1);
    lcd.print("0");
    lcd.setCursor(7,1);
    lcd.print(epoch %60);
  } else {
    Serial.println(epoch %60);
    lcd.setCursor(6,1);
    lcd.print(epoch %60);
  }
  if ((((epoch  % 86400L) / 3600) == 0) && (((epoch % 3600) / 60) == 0) && ((epoch % 60) < 1)) {
    Udp.begin(localPort);
    epoch = getNTP();
    RTC.adjust(epoch);
    Serial.println("RTC Configured:");
    Serial.print("Unix Epoch: ");
    Serial.println(epoch);
    Serial.print("UTC: ");
    showUTC(epoch);
  }
  if ((((epoch % 3600) / 60) == 1) && ((epoch % 60) < 1)) {
    Serial.println("Connecting to server...");
    if (client.connect(server, 80)) {
      Serial.println("HTTP requesting...");
      client.println("GET " + dataLocation);
      client.println("Host: ka-ex.net");
      client.println();
    }
  }
  if (((epoch % 60) == 00) || ((epoch % 60) == 15) || ((epoch % 60) == 30) || ((epoch % 60) == 45)) {
    if (clientx.connect(cserver, 80)) {
      Serial.println("-> Connected TEMP");
      //Make a HTTP request:
      clientx.print("GET /kas/arduino_in_tempx.php");
      //start vars
      clientx.print("?in=");
      clientx.print("1");
      clientx.print("&ip=");
      clientx.print(Ethernet.localIP());
      clientx.print("&mac=");
      clientx.print(mac_addr);
      //end vars
      clientx.println(" HTTP/1.1");
      clientx.print("Host: ");
      clientx.println(cserver);
      clientx.println("Connection: close");
      clientx.println();
      clientx.stop();
      Serial.println("-> SENT!");
      bar.setLed(4,1);
      bar.setLed(1,0);
      delay(100);
      bar.setLed(4,0);
    } else {
      Serial.println("-> Connection failed!");
      bar.setLed(1,1);
    }
  }
}

unsigned long getNTP() {
  sendNTPpacket(timeServer);
  delay(999);
  if (Udp.parsePacket()) {  
    Udp.read(packetBuffer,NTP_PACKET_SIZE);
    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);  
    unsigned long secsSince1900 = highWord << 16 | lowWord;  
    Serial.print("Seconds since Jan 1 1900 = " );
    Serial.println(secsSince1900);               
    Serial.print("Unix time = ");
    const unsigned long seventyYears = 2208988800UL;     
    unsigned long epoch = secsSince1900 - seventyYears;  
    //offset +2 hours
    epoch +=7200;
    return epoch;
    bar.setLed(3,1);
    bar.setLed(1,0);
  } else {
    bar.setLed(3,0);
    bar.setLed(1,1);
  }
}

void sendNTPpacket(IPAddress& address) {
  memset(packetBuffer, 0, NTP_PACKET_SIZE); 
  packetBuffer[0] = 0b11100011;
  packetBuffer[1] = 0;
  packetBuffer[2] = 6;
  packetBuffer[3] = 0xEC;
  packetBuffer[12]  = 49; 
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;      
  Udp.beginPacket(address,123);
  Udp.write(packetBuffer,NTP_PACKET_SIZE);
  Udp.endPacket(); 
}

