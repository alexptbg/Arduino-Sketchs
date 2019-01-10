/*
 This MODIFIED sketch connects to a website (http://www.rtupdate.wunderground.com)
 Using an Arduino Wiznet Ethernet shield To upload your weather conditions to wunderground.com,
 We make a standard HTTP GET request with the ID, PASSWORD and weather conditions as GET parameters.
 You must first create an account with wunderground for this to werk!
 The anemometer/windvane are from ebay looke for a cheap one and upgrade it or make your own
 code for the Anemometer from: Authors: M.A. de Pablo & C. de Pablo S. on there home-made version af an anomometer
 Various notes throughout attempt to explain why i did what i did to make it werk.
 This sketch also incorporates an NTP RTC update code to make sure that the time is always correct and
 resets the RTC Soft or Hard to UTC ZULU to comp for time drift, (Wunderground wants "UTC ZULU HOUR" for RTU).
 Results for this can be monitored @ http://www.wunderground.com/weatherstation/WXDailyHistory.asp?ID=KFLFORTM67
 
 Binary sketch size: 29,800 bytes (of a 32,256 byte maximum) as of 6/27/2014
 
 misc links,
 * http://en.wikipedia.org/wiki/Dew_point Dew point Calc
 * http://wiki.wunderground.com/index.php/PWS_-_Upload_Protocol Information on the Upload Protocol for wunderground.
 
 
 Circuit:
 * Arduino UNO adafruit http://www.adafruit.com/products/50
 * Ethernet shield attached to pins 10, 11, 12, 13 http://www.adafruit.com/products/201
 * DHT to pin A0 adafruit http://www.adafruit.com/products/393
 * Wind direction to pin A1 this is a DAVIS instrument which incorperates the anemometer (or make a home made one using a 360 var resistor)
 * Anemometer to Digtal 2 (interrupt 0)this is a DAVIS instrument (reed switch) 
 * and it looks like this http://www.skygeek.com/davis-instrument-7890-anemometer-mast-mount.html?utm_source=googlebase&utm_medium=shoppingengine&utm_content=davis-instrument-7890-anemometer-mast-mount&utm_campaign=froogle&gclid=CLLugIr107wCFbB9Ogodi14AiA
 * Rain tip to Digital 3 (interrupt 1) (hall effect sensor) http://www.adafruit.com/products/158
 * BMP to I2C adafruit https://www.adafruit.com/products/391
 * RTC to I2C adafruit https://www.adafruit.com/products/264
 **
 Originaly created 18 Dec 2009
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe, based on work by Adrian McEwen
 **
 **
 Modified  To upload weather conditions to wunderground.com
 1,Jan 2014
 By the drunkin programer
 *
 */


#include "SPI.h"
#include "Ethernet.h"
#include "EthernetUdp.h"	      // New from IDE 1.0
#include "Wire.h"
#include "DHT.h"
#include <Adafruit_BMP085.h>
//#include "TSL2561.h"
#include "Adafruit_SI1145.h"
#include "RTClib.h"
 
#define DHTTYPE DHT11
int buttonState;             // the current reading from the input pin
int lastButtonState = HIGH;   // the previous reading from the input pin
// Below is for bebouncing the rainBucket's hall effect switch. 
// I'm tring this out because from time to time, I get false hits due to something unknown at this time.
// The following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 10;    // the debounce time; increase if the output flickers

//IPAddress SERVER(38,102,137,157);  // numeric IP for wunderground.com
char SERVER[] = "rtupdate.wunderground.com";           // Realtime update server
//char SERVER [] = "weatherstation.wunderground.com";  //standard server
char WEBPAGE [] = "GET /weatherstation/updateweatherstation.php?";
char ID [] = "KFLFORTM67";
char PASSWORD [] = "xxxxxxxxxx";
// byte timeServer[] = {192, 43, 244, 18}; // time.nist.gov NTP server
byte timeServer0[] = {
  132, 163, 4, 101};    // ntp1.nl.net NTP server  
byte timeServer1[] = {
  132, 163, 4, 102}; // time-b.timefreq.bldrdoc.gov NTP server
byte timeServer2[] = {
  132, 163, 4, 103};
const int NTP_PACKET_SIZE= 48;     // NTP time stamp is in the first 48 bytes of the message
byte pb[NTP_PACKET_SIZE];          // buffer to hold incoming and outgoing packets 
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };//Hard config for ethernet card mac addy
EthernetClient client;
unsigned int localPort = 8888;
const unsigned long rainTime = 100;
const unsigned long period = 2500;// Measurement period for the anemometer(miliseconds)
const unsigned long IDLE_TIMEOUT_MS = 1000; // Time to wait for the WunderGround server to respond
const int radio = 60;             // Radius from center of Anemometer to center of one of the Anemometer cups
const int DHTPIN = A0;            // DHT 22  (AM2302)
// Pins
const int vane = A1;              // Wind direction
const int anemometer = 2;         // D2 Wind speed
const int rainTip = 3;            // pin D3 Rain bucket
//const int led = 6 ;             // Visual Notification for whatever

unsigned int counter = 0;         // pulse count for wind sensor 
unsigned int RPM = 0;             // Revolutions per minute for wind sensor
unsigned int winddir = 0.0;       // Wind direction for anemometer
unsigned int rainin = 0;          // inches of rain  
unsigned int dailyrainin = 0;     // dailyRain
unsigned int rainCount = 0;       // count down from ? for rainin Reset

float windspeedmph = 0/ 0.445;    // Wind speed (m/s) for wind sensor
float windgustmph = 0;            // Max wind speed for wind sensor
const float pi = 3.14159265;      //RPM Calcs

//TSL2561 tsl(TSL2561_ADDR_FLOAT); // Light Sensor
Adafruit_SI1145 uv = Adafruit_SI1145();// UV Sensor
EthernetUDP Udp;                  // UDP for the time server
DHT dht(DHTPIN, DHTTYPE);         // DHT 22  (AM2302)
Adafruit_BMP085 bmp;              // BMP Sensor
//Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);
//RTC_DS1307 rtc; // Hardware RTC time
RTC_Millis rtc; // Software RTC time


//---DEWPOINT CALC!-------------------------------
double dewPoint(double tempf, double humidity)
{
  double A0= 373.15/(273.15 + tempf);
  double SUM = -7.90298 * (A0-1);
  SUM += 5.02808 * log10(A0);
  SUM += -1.3816e-7 * (pow(10, (11.344*(1-1/A0)))-1) ;
  SUM += 8.1328e-3 * (pow(10,(-3.49149*(A0-1)))-1) ;
  SUM += log10(1013.246);
  double VP = pow(10, SUM-3) * humidity;
  double T = log(VP/0.61078);   
  return (241.88 * T) / (17.558-T);
}
//---DEWPOINT CALC!-------------------------------



void setup(void)  
{  

  Serial.begin(19200);
  //rtc.begin(); // uncomment to use Hardware RTC!
  // Uncomment to below to reset the Hardware RTC!
  // rtc.adjust(DateTime(__DATE__, __TIME__));// leave uncommented if using software 
  // Comment out below to use the Hardware RTC
  rtc.begin(DateTime(__DATE__, __TIME__));
  DateTime now = rtc.now();
  bmp.begin();
  dht.begin();
  uv.begin();
  //tsl.begin();
  //tsl.setGain(TSL2561_GAIN_16X);      // set 16x gain (for dim situations)
  //tsl.setTiming(TSL2561_INTEGRATIONTIME_402MS);  // longest integration time (dim light)
  //pinMode(led, OUTPUT);
  //digitalWrite(led, HIGH);
  pinMode(anemometer, INPUT); //Anemometer input(spins like mad when the wind blows).
  digitalWrite(anemometer, HIGH); // set this high so when it detects a change it records it via INT(0)
  pinMode(rainTip, INPUT);// rain tip input
  digitalWrite(rainTip, HIGH);     // set this high so when it detects a change it records it via INT(1)
  Serial.println(F("Wunderground"));// Tells me whats Loaded on the arduino Chip if I change it out
  Serial.print(F("Compile Date:  "));
  Serial.print(now.month());
  Serial.print('/');
  Serial.print(now.day());
  Serial.print('/');
  Serial.println(now.year());
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet");
    //Ethernet.begin(mac, ip);
    // Serial.println(ip);
  }
  Serial.print("Local I.P: ");
  Serial.println(Ethernet.localIP());
  Serial.println("Ready!");

  ntp();//Set the RTC on first boot, it will fail the first time but Complets the second time
}
// ______________________________End of the Setup Function_______________________________
void loop(void)
{
  DateTime now = rtc.now();

  //uint32_t lum = tsl.getFullLuminosity();
  //uint16_t ir, full;
  //ir = lum >> 16;
  //full = lum & 0xFFFF;
  
  // max wind speed holder(wunderground)
  if (windspeedmph > windgustmph) {
    windgustmph = windspeedmph;
  }
  // Clear the Daily rain amount and check the time
  if (now.hour()==24){
    dailyrainin = 0;
    ntp();
  }
  if (rainCount >= 900){
    /*
     Im using A 900 count for the 4 second loop time 
     wunderground goes by 60 minute intervals
     4 seconds x 900 = 3600 seconds / 60 = 60 minutes
     */
    rainCount = 0;
    rainin = 0;
    windgustmph = 0;

  }
  //Start everything up!
  rainCount++;
  windvelocity();
  RPMcalc();
  WindSpeed();
  Heading();
  Rain();
  //float ldr = (uv.readVisible());
  //if (ldr <=351){
    //tsl.setGain(TSL2561_GAIN_16X);      // set 16x gain (for dim situations)
    //tsl.setTiming(TSL2561_INTEGRATIONTIME_402MS);  // longest integration time (dim light)

 // }
 // else if(ldr >=350){
    // You can change the gain on the fly, to adapt to brighter/dimmer light situations
    //tsl.setGain(TSL2561_GAIN_0X);         // set no gain (for bright situtations)
    //tsl.setTiming(TSL2561_INTEGRATIONTIME_13MS);  // shortest integration time (bright light)
  //}
  float UVindex = uv.readUV();
  UVindex /= 100.0;
  //float Wm2 = (tsl.calculateLux(full, ir)* 0.0079);
  //Wm2*=100;
  float tempf = dht.readTemperature(); 
  float humidity = dht.readHumidity(); 
  float baromin = bmp.readPressure()* 0.0002953;// Calc for converting Pa to inHg (wunderground)
  float dewptf = (dewPoint(dht.readTemperature(), dht.readHumidity())); //Dew point calc(wunderground)

  // Send the Data
  if (client.connect(SERVER, 80)) { 
    //Serial.println("Sending DATA ");
    client.print(WEBPAGE); 
    client.print("ID=");
    client.print(ID);
    client.print("&PASSWORD=");
    client.print(PASSWORD);
    client.print("&dateutc=");
    client.print(now.year());
    client.print("-");
    client.print(now.month());
    client.print("-");
    client.print(now.day());
    client.print("+");
    client.print(now.hour()); 
    client.print("%3A");
    client.print(now.minute());
    client.print("%3A");
    client.print(now.second()+5);
    client.print("&winddir=");
    client.print(winddir);
    client.print("&windspeedmph=");
    client.print(windspeedmph);
    client.print("&windgustmph=");
    client.print(windgustmph);
    client.print("&windgustmph_10m=");
    client.print(windgustmph);//[mph past 10 minutes wind gust mph ]
    client.print("&tempf=");
    client.print(tempf);
    client.print("&rainin=");
    client.print(rainin / 2.0 / 100.0);
    client.print("&dailyrainin=");
    client.print(dailyrainin / 2.0 / 100.0); 
    client.print("&baromin=");
    client.print(baromin);
    client.print("&dewptf=");
    client.print(dewptf);
    client.print("&humidity=");
    client.print(humidity);
    client.print("&UV=");
    client.print(UVindex);//Si1145 UV/IR/Visible Light Sensor
    //client.print("&solarradiation="); //TSL2561 luminosity sensor
    //client.print(Wm2);
    //client.print("&action=updateraw");//Standard update
    client.print("&softwaretype=Arduino%20UNO%20version1&action=updateraw&realtime=1&rtfreq=2.5");//Rapid Fire
    client.println();
  } 
  else {
    Serial.println(F("Connection failed"));  
    delay(2000);  
    return;
  }
  //Serial.println("Server Response!");
  unsigned long lastRead = millis();
  while (client.connected() && (millis() - lastRead < IDLE_TIMEOUT_MS)) {
    if (client.available()) {
      char c = client.read();
      Serial.print(c);
    }
  }
  client.stop();  // Stop/disconnect the ethernet Client
  PrintSerial();
}// end loop


///////////////////////////////////////////
//
// NTP RTC Time SYNC
// 
void ntp(){

  Udp.begin(localPort);// Start the UDP on the selected port
  // Serial.println();
  delay(100);
  int packetSize = Udp.parsePacket(); //<<<<<<<<<<<<NEW LINE HERE 
  // send an NTP packet to a time server
  sendNTPpacket(timeServer1);
  // wait to see if a reply is available
  delay(1000);
  if ( Udp.available() ) {
    // read the packet into the buffer
    Udp.read(pb, packetSize);      // New from IDE 1.0 <<<<<<<<<<<<<MODIFIED LINE

    unsigned long t1, t2, t3, t4;
    t1 = t2 = t3 = t4 = 0;
    for (int i=0; i< 4; i++)
    {
      t1 = t1 << 8 | pb[16+i];      
      t2 = t2 << 8 | pb[24+i];      
      t3 = t3 << 8 | pb[32+i];      
      t4 = t4 << 8 | pb[40+i];
    }

    float f1,f2,f3,f4;
    f1 = ((long)pb[20] * 256 + pb[21]) / 65536.0;      
    f2 = ((long)pb[28] * 256 + pb[29]) / 65536.0;      
    f3 = ((long)pb[36] * 256 + pb[37]) / 65536.0;      
    f4 = ((long)pb[44] * 256 + pb[45]) / 65536.0;

    const unsigned long seventyYears = 2208988800UL;
    t1 -= seventyYears;
    t2 -= seventyYears;
    t3 -= seventyYears;
    t4 -= seventyYears;

    PrintDateTime(DateTime(t4)); 
    // Serial.println(f4,4);
    Serial.println();
    // Adjust timezone and DST... in my case substract 4 hours for Florida Time
    // or work in UTC?

    //t4 -= (4 * 3600L);     // Notice the L for long calculations!!
    t4 -= (2/3600L);     // Divide by 2 for DST
    t4 += 1;               // adjust the delay(1000) at begin of loop!
    if (f4 > 0.4) t4++;    // adjust fractional part, see above
    Serial.println("UpDating CLOCK ");
    //Serial.print("RTC before: ");
    PrintDateTime(rtc.now());// Show me the time
    rtc.adjust(DateTime(t4));
    Serial.println("Updated Time : ");
    PrintDateTime(rtc.now());
    Serial.println();
    Serial.println("done ...");
  }
  else
  {
    //Serial.println("ARRRRRRRRRRRRrrr !!!");// wait 10 more loop's.
    Serial.println("No UDP available ...");
    delay(1000);//wait a second or more before trying again 
    ntp(); //Try to get the time from the NTP SERVER
  }

}
void PrintDateTime(DateTime t)
{
  char datestr[24];
  sprintf(datestr, "%04d-%02d-%02d  %02d:%02d:%02d  ", t.year(), t.month(), t.day(), t.hour(), t.minute(), t.second());
  Serial.println(datestr);  
}


// send an NTP request to the time server at the given address 
unsigned long sendNTPpacket(byte *address)
{
  // set all bytes in the buffer to 0
  memset(pb, 0, NTP_PACKET_SIZE); 
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  pb[0] = 0b11100011;   // LI, Version, Mode
  pb[1] = 0;     // Stratum, or type of clock
  pb[2] = 6;     // Polling Interval
  pb[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  pb[12]  = 49; 
  pb[13]  = 0x4E;
  pb[14]  = 49;
  pb[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp: 
#if ARDUINO >= 100
  // IDE 1.0 compatible:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(pb,NTP_PACKET_SIZE);
  Udp.endPacket(); 
#else
  Udp.sendPacket( pb,NTP_PACKET_SIZE,  address, 123); //NTP requests are to port 123
#endif	  

}

//Wind Speed Trigger/timer
void windvelocity(){
  windspeedmph = 0;
  counter = 0;
  attachInterrupt(0, addcount, RISING);//anemometer
  unsigned long millis();                     
  long startTime = millis();
  while(millis() < startTime + period) {
  }
  detachInterrupt(0);
}
//RPM calc needed for wind speed calc
void RPMcalc(){
  RPM=((counter*4)*60)/(period/1000);  // Calculate revolutions per minute (RPM)
}
void WindSpeed(){
  windspeedmph = ((2 * pi * radio * RPM)/60) / 1000;
}
void addcount(){
  counter++;
}
//Rain Trigger
void Rain(){

  int reading = digitalRead(rainTip);

  // check to see if you just pressed the button 
  // (i.e. the input went from LOW to HIGH),  and you've waited 
  // long enough since the last press to ignore any noise:  

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  } 
  
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
       attachInterrupt(1,addrain,CHANGE);
      }
  }

  lastButtonState = reading;

}
// Rain calc
void addrain(){

  (dailyrainin)++;
  (rainin)++;
  //digitalWrite(led, HIGH);
}

// Voltage to Compas Degree conversion
void Heading(){
  float windDir =(analogRead(vane)* 5.00 / 1023.0);    // Wind direction                
  if ((windDir > 4.94)||(windDir < 0.01))
    (winddir = 0.0);// North
  if ((windDir >= 0.02)&&(windDir < 0.30))
    (winddir = 22.5);//NNE
  if ((windDir >= 0.31)&&(windDir < 0.72))
    (winddir = 45.0);//NE
  if ((windDir >= 0.72)&&(windDir < 0.99))
    (winddir= 67.5);//ENE
  if ((windDir >= 1.00)&&(windDir < 1.25))
    (winddir = 90.0);//E
  if ((windDir >= 1.26)&&(windDir < 1.52))
    (winddir= 112.5);//ESE
  if ((windDir >= 1.53)&&(windDir < 1.91))
    (winddir= 135.0);//SE
  if ((windDir >= 1.92)&&(windDir < 2.40))
    (winddir = 157.5);//SSE
  if ((windDir >= 2.41)&&(windDir < 2.73))
    (winddir = 180.0);//S
  if ((windDir >= 2.74)&&(windDir < 2.96))
    (winddir = 202.5);//SSW
  if ((windDir >= 2.97)&&(windDir < 3.37))
    (winddir = 225.0);//SW
  if ((windDir >= 3.38)&&(windDir < 3.55))
    (winddir = 247.5);//WSW
  if ((windDir >= 3.56)&&(windDir < 3.85))
    (winddir = 270.0);//W
  if ((windDir >= 4.13)&&(windDir < 4.06))
    (winddir = 292.5);//WNW
  if ((windDir >= 4.07)&&(windDir < 4.32))
    (winddir = 315.0);//NW
  if ((windDir >= 4.33)&&(windDir < 4.93))
    (winddir = 337.5);//NNW
}
// For debugging
void PrintSerial(){
  DateTime now = rtc.now();
  float UVindex = uv.readUV();
  UVindex /= 100.0;
  float tempf = dht.readTemperature(); 
  float humidity = dht.readHumidity(); 
  float baromin = bmp.readPressure()* 0.0002953;// Calc for converting Pa to inHg (wunderground)
  float dewptf = (dewPoint(dht.readTemperature(), dht.readHumidity())); //Dew point calc(wunderground)

  // for Debug, or you can sit up all night watching it.
  Serial.println();
  Serial.print("wind dir:");
  Serial.print("\t");
  Serial.print(winddir);
  Serial.println(" deg's");
  Serial.print("wind:");
  Serial.print("\t\t");
  Serial.println(windspeedmph);
  Serial.print("wind gust mph:");
  Serial.print("\t");
  Serial.println(windgustmph);
  Serial.print("temp:");
  Serial.print("\t\t");
  Serial.print(tempf);
  Serial.println(" *F");
  Serial.print("baro:");
  Serial.print("\t\t");
  Serial.print(baromin);
  Serial.println(" inHg");
  Serial.print("dew point:");
  Serial.print("\t");
  Serial.println(dewptf);
  Serial.print("humi:");
  Serial.print("\t\t");
  Serial.println(humidity);
  Serial.print("rain:");
  Serial.print("\t\t");
  Serial.println(rainin / 2.0 / 100.0);
  Serial.print("daily:");
  Serial.print("\t\t");
  Serial.println(dailyrainin / 2.0 / 100.0);
  Serial.print("U.V."); 
  Serial.print("\t\t"); 
  Serial.println(UVindex);
  Serial.print("LOCAL Time");
  Serial.print("\t");
  Serial.print(now.hour());
  Serial.print(':');
  Serial.print(now.minute());
  Serial.print(':');
  Serial.println(now.second()+5);
}
///////////////////////////////////////////
//
// End of program
//

