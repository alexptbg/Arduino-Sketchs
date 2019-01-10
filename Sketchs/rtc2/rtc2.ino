#include <SPI.h>         
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <Wire.h>
#include <RTClib.h>
#include <DHT.h>
#include <Timer.h>

//RTC Encapsulation
RTC_DS1307 RTC;

byte mac[] = { 0x90, 0xA2, 0xDA, 0x0F, 0x9F, 0x08 };//MAC Address for Ethernet Shield

unsigned int localPort = 8888; //Local port to listen for UDP Packets

IPAddress timeServer(206,246,122,250);//NTP Server IP (time.nist.gov)

const int NTP_PACKET_SIZE= 48;  //NTP Time stamp is in the firth 48 bytes of the message

byte packetBuffer[ NTP_PACKET_SIZE];  //Buffer to hold incomming and outgoing packets

EthernetUDP Udp;  //UDP Instance to let us send and recieve packets

unsigned long epoch; //Unix Epoch time (NTP or RTC depending on state)

IPAddress ip(192,168,2,89);
IPAddress gateway(192,168,2,1);
IPAddress subnet(255,255,254,0);

char myserver[] = "eesystems.net";

EthernetServer server(80);
EthernetClient cx;

#define DHTPIN1 2
#define DHTPIN2 3 

#define DHTTYPE DHT11

DHT dht1(DHTPIN1, DHTTYPE);
DHT dht2(DHTPIN2, DHTTYPE);

boolean updated = false;

Timer t;

char inString[32];
int stringPos = 0;
boolean startRead = false;

void setup() {
  
  Serial.begin(9600); //Start the serial interface
  
  Wire.begin();  //Start Wire
  
  RTC.begin();  //Start the RTC

  delay(1000);
  //check to see if the RTC is already configured
  if( !RTC.isrunning() ) {
    Serial.println("RTC Not Configured");
    Serial.println("Starting Ethernet");  //configure Ethernet
    if(Ethernet.begin(mac) == 0) {
      Serial.println("Fatal Error: Unable to obtain DHCP address");
      for(;;)  //do nothing forever!
        ;      //TODO Change this to retry a few times
    }
    
    Serial.println("Ethernet Configured");
    Udp.begin(localPort);

    //get the NTP timestamp
    epoch = getNTP();
    
    //set the RTC
    RTC.adjust(epoch);
    
    //display what we did!
    Serial.println("RTC Configured:");
    
    //show Unix Epoch
    Serial.print("Unix Epoch: ");
    Serial.println(epoch);
    
    //show UTC
    Serial.print("UTC: ");
    showUTC(epoch);
  } else {
    Serial.println("RTC Already Configured");
  }
  Serial.println("DHTxx test!");
  dht1.begin();
  dht2.begin();

  Ethernet.begin(mac,ip); 
  server.begin();
  Serial.print("Server internal IP: ");
  Serial.println(Ethernet.localIP());
  
  int tickEvent = t.every(10000, update);
}

void loop() {
  t.update();
  //repeatedly show the current date and time, taken from the RTC
  DateTime now = RTC.now();

  Serial.print(now.unixtime());
  Serial.print(" - ");
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" - ");

  showUTC(now.unixtime());
  
  delay(1000);
  
  float h1 = dht1.readHumidity();
  float t1 = dht1.readTemperature();
  float f1 = dht1.readTemperature(true);
  
  if (isnan(h1) || isnan(t1) || isnan(f1)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  float hi1 = dht1.computeHeatIndex(f1,h1);
  float fl1 = ((f1 - 32) * 5 / 9); 
  
  Serial.print("Humidity: ");
  Serial.print(h1);
  Serial.print(" - ");
  Serial.print("Temperature: ");
  Serial.print(t1);
  Serial.print(" *C ");
  Serial.print("Heat index: ");
  Serial.print(fl1);
  Serial.println(" *C");
  //2
  float h2 = dht2.readHumidity();
  float t2 = dht2.readTemperature();
  float f2 = dht2.readTemperature(true);
  
  if (isnan(h2) || isnan(t2) || isnan(f2)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  float hi2 = dht2.computeHeatIndex(f2,h2);
  float fl2 = ((f2 - 32) * 5 / 9); 
  
  Serial.print("Humidity: ");
  Serial.print(h2);
  Serial.print(" - ");
  Serial.print("Temperature: ");
  Serial.print(t2);
  Serial.print(" *C ");
  Serial.print("Heat index: ");
  Serial.print(fl2);
  Serial.println(" *C");
}

void showUTC(unsigned long epoch) {
  if ( ((epoch  % 86400L) / 3600) < 10 ) {
    // In the first 10 minutes of each hour, we'll want a leading '0'
    Serial.print('0');
  }
  Serial.print((epoch  % 86400L) / 3600); // print the hour (86400 equals secs per day)
  Serial.print(':');  
  if ( ((epoch % 3600) / 60) < 10 ) {
    // In the first 10 minutes of each hour, we'll want a leading '0'
    Serial.print('0');
  }
  Serial.print((epoch  % 3600) / 60); // print the minute (3600 equals secs per minute)
  Serial.print(':'); 
  if ( (epoch % 60) < 10 ) {
    // In the first 10 seconds of each minute, we'll want a leading '0'
    Serial.print('0');
  }
  Serial.println(epoch %60); // print the second
}

unsigned long getNTP() {
  sendNTPpacket(timeServer); // send an NTP packet to a time server

  // wait to see if a reply is available
  delay(1000);  
  if ( Udp.parsePacket() ) {  
    // We've received a packet, read the data from it
    Udp.read(packetBuffer,NTP_PACKET_SIZE);  // read the packet into the buffer

    //the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, esxtract the two words:

    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);  
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;  
    Serial.print("Seconds since Jan 1 1900 = " );
    Serial.println(secsSince1900);               

    // now convert NTP time into everyday time:
    Serial.print("Unix time = ");
    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const unsigned long seventyYears = 2208988800UL;
    // subtract seventy years:
    unsigned long epoch = secsSince1900 - seventyYears;
    //offset +2 hours
    epoch +=7200;
    return epoch;
  }
}

// send an NTP request to the time server at the given address 
void sendNTPpacket(IPAddress& address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE); 
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49; 
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:         
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer,NTP_PACKET_SIZE);
  Udp.endPacket(); 
}

void update() {
  stringPos = 0;
  //updated = false;
  memset( &inString, 0, 32 );
  Serial.println("Connecting to server...");
  if (cx.connect(myserver, 80)>0) {
    
    Serial.println("Making HTTP request...");
    cx.println("GET /kas/out_temp_update.php?insert=now&temp=24.56&humidity=45.67 HTTP/1.0");
    cx.println("User-Agent: Arduino 1.0");
    cx.println("HOST: eesystems.net");
    cx.println("Connection: keep-open");
    cx.println();
    //cx.setTimeout(5000);
    
    if(cx.connected()) {
      Serial.println("ARDUINO: HTTP message received");
      /*
      if(cx.find("200")){ 
         Serial.println("yes");
      } else {
         Serial.println("no");
      }*/
      while(cx.connected()) {
        char inChar = cx.read();
        if (inChar == '<') {
          startRead = true;
        } else if(startRead){
          if(inChar != '>'){ 
            inString[stringPos] = inChar;
            stringPos++;
          } else {
            startRead = false;
            Serial.println("Disconnecting.");
            cx.stop();
            cx.flush();
            Serial.println(inString);
          }
        }
        //break;
      }
    }
    Serial.println(inString);
    delay(10);
    cx.stop();
    cx.flush();
  } else {
    Serial.println("Connection failed!");
    updated = false;
  }
}
