#include <SPI.h>         
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <Wire.h>
#include <RTClib.h>
#include <DHT.h>
#include <Timer.h>
//new
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>

//RTC
/*
RTC_DS1307 RTC;
unsigned int localPort = 8888;
IPAddress timeServer(206,246,122,250);//NTP Server IP (time.nist.gov)
const int NTP_PACKET_SIZE= 48; 
byte packetBuffer[ NTP_PACKET_SIZE];
EthernetUDP Udp;
unsigned long epoch; //Unix Epoch time (NTP or RTC depending on state)
*/
//MAC
byte mac[] = { 0x90, 0xA2, 0xDA, 0x0F, 0x9F, 0x08 };    // MAC address
//Server
IPAddress ip(192,168,2,89);//IP address
EthernetServer server(80);

boolean currentLineIsBlank = false;
String readString = "";
//client
EthernetClient clientx;
char cserver[] = "eesystems.net"; // IP Adres (or name) of server to dump data to
//DHT
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);

void setup() {
  //initialize the ethernet device
  Ethernet.begin(mac,ip);
  server.begin();

  Serial.begin(9600); //Start the serial interface
  Wire.begin();  //Start Wire
  /*
  RTC.begin();  //Start the RTC

  delay(1000);
  //check to see if the RTC is already configured
  if(!RTC.isrunning()) {
    Serial.println("RTC Not Configured");
    Serial.println("Starting Ethernet");  //configure Ethernet
    if(Ethernet.begin(mac) == 0) {
      Serial.println("Fatal Error: Unable to obtain DHCP address");
      for(;;)  //do nothing forever!
        ;
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
  */
  Serial.print("Server internal IP: ");
  Serial.println(Ethernet.localIP());
  //client

  //DHT
  Serial.println("DHT/BMP test!");
  dht.begin();
  //BMP180
  if(!bmp.begin()) {
    Serial.print("Ooops, no BMP085 detected!");
    while(1);
  }
  /* Display some basic information on this sensor */
  displaySensorDetails();
}

void loop() {
  //time
  //repeatedly show the current date and time, taken from the RTC
  /*
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
  */
  //DHT
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  float hi = dht.computeHeatIndex(f, h);
  float fl = (f - 32) * 5 / 9;
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" % ");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print("Heat index: ");
  Serial.print(fl);
  Serial.println(" *C");
  //BMP180
  sensors_event_t event;
  bmp.getEvent(&event);
  if (event.pressure) {
    /* Display atmospheric pressue in hPa */
    Serial.print("Pressure:    ");
    Serial.print(event.pressure);
    Serial.println(" hPa");
    /* First we get the current temperature from the BMP085 */
    float temperature;
    bmp.getTemperature(&temperature);
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" C");
    /* Then convert the atmospheric pressure, and SLP to altitude         */
    /* Update this next line with the current SLP for better results      */
    float seaLevelPressure = SENSORS_PRESSURE_SEALEVELHPA;
    Serial.print("Altitude:    "); 
    Serial.print(bmp.pressureToAltitude(seaLevelPressure,event.pressure)); 
    Serial.println(" m");
    Serial.println("");
  } else {
    Serial.println("Sensor error");
  }
  //delay(1000);
  //Server
  EthernetClient client = server.available();
  if (client) {
    Serial.println("++++++++++++++++++++++++++++");
    Serial.println("new client");

    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        if (readString.length() < 100) {
          readString += c;
        }
        if (c == '\n' && currentLineIsBlank) {
          Serial.println(readString); 
          //print to serial monitor for debuging
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          client.println("Refresh: 5");
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.println("<head>");
          client.println("</head>");
          client.println("<body>");
          client.println("");
          /*
          //rtc
          client.print(now.year(), DEC);
          client.print("-");
          if (now.month() < 10) {
              client.print('0');
          }
          client.print(now.month(), DEC);
          client.print("-");
          if (now.day() < 10) {
              client.print('0');
          }
          client.print(now.day(), DEC);
          client.print(" ");
          if (now.hour() < 10) {
              client.print('0');
          }
          client.print(now.hour(), DEC);
          client.print(':');
          if (now.minute() < 10) {
              client.print('0');
          }
          client.print(now.minute(), DEC);
          client.print(':');
          if (now.second() < 10) {
              client.print('0');
          }
          client.print(now.second(), DEC);
          */
          //DHT
          client.print(" - Humidity: ");
          client.print(h);
          client.print(" % ");
          client.print("Temperature: ");
          client.print(t);
          client.print(" *C ");
          client.print("Heat index: ");
          client.print(fl);
          client.println(" *C");


          client.println("");
          client.println("</body>");
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;

        }
        else if (c != '\r') {
          //you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(10);
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
    Serial.println("+++++++++++++++++++++++++++++++");
  }
  //client
  if (clientx.connect(cserver, 80)) {
    Serial.println("-> Connected");
    // Make a HTTP request:
    clientx.print("GET /kas/out_temp_update.php");
    clientx.print("?insert=");
    clientx.print("now");
    clientx.print("&c_date=");
    //clientx.print(now.unixtime());
    
    clientx.print("&temp=");
    clientx.print(t);
    clientx.print("&humidity=");
    clientx.print(h);
    clientx.println(" HTTP/1.1");
    clientx.print("Host: ");
    clientx.println(cserver);
    clientx.println("Connection: close");
    clientx.println();
    clientx.stop();
    /*
    Serial.print(now.unixtime());
    */
    Serial.println("DATA SENT!");
  } else {
    Serial.println("--> connection failed/n");
  }
}
/*
unsigned long getNTP() {
  sendNTPpacket(timeServer); // send an NTP packet to a time server
  delay(1000);  
  if (Udp.parsePacket()) {  
    Udp.read(packetBuffer,NTP_PACKET_SIZE);  // read the packet into the buffer
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
  }
}

void showUTC(unsigned long epoch) {
  if (((epoch  % 86400L) / 3600) < 10) {
    Serial.print('0');
  }
  Serial.print((epoch  % 86400L) / 3600); // print the hour (86400 equals secs per day)
  Serial.print(':');  
  if (((epoch % 3600) / 60) < 10) {
    Serial.print('0');
  }
  Serial.print((epoch  % 3600) / 60); // print the minute (3600 equals secs per minute)
  Serial.print(':'); 
  if ((epoch % 60) < 10) {
    Serial.print('0');
  }
  Serial.println(epoch %60); // print the second
}

void sendNTPpacket(IPAddress &address) {
  memset(packetBuffer, 0, NTP_PACKET_SIZE); 
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  packetBuffer[12]  = 49; 
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;       
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer,NTP_PACKET_SIZE);
  Udp.endPacket(); 
}
*/
void displaySensorDetails(void) {
  sensor_t sensor;
  bmp.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" hPa");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" hPa");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" hPa");  
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}
