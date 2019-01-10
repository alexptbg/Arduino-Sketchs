#include <SPI.h>         
#include <Ethernet.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>
#include <Adafruit_SI1145.h>
#include <DHT.h>

//ID
String ID = "AR_0001_2014_1.0";
//sea level pressure in bulgaria 
float inbg = 1016;//november
//MAC
byte mac[] = { 0x90, 0xA2, 0xDA, 0x0F, 0x9F, 0x08 };    // MAC address
//Server
//IPAddress ip(192,168,2,89);//IP address
EthernetServer server(80);

boolean LineIsBlank = false;
String readString = "";
//client
EthernetClient clientx;
char cserver[] = "eesystems.net"; // IP Adres (or name) of server to dump data to
//DHT
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
//BMP
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);
//SI1145
Adafruit_SI1145 uv = Adafruit_SI1145();
//GA1A12S202
void setup() {
  //initialize the ethernet device
  Ethernet.begin(mac);
  server.begin();
  //Serial
  Serial.begin(9600); //Start the serial interface
  Serial.print("Server IP: ");
  Serial.println(Ethernet.localIP());
  //DHT
  dht.begin();
  //BMP180
  if(!bmp.begin()) {
    Serial.print("No BMP180!");
    while(1);
  }
  //SI1145
  if (!uv.begin()) {
    Serial.println("No SI1145!");
    while(1);
  }
}

void loop() {
  //DHT
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);
  if (isnan(h) || isnan(t) || isnan(f)) {
    return;
  }
  //float hi = dht.computeHeatIndex(f, h);
  //float fl = (f - 32) * 5 / 9;
  /*
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" % ");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print("Heat index: ");
  Serial.print(fl);
  Serial.println(" *C");
  */
  delay(1000);
  //BMP180
  sensors_event_t event;
  bmp.getEvent(&event);
  /*
  if (event.pressure) {
    //do nothind
  } else {
    Serial.println("Failed to read BMP sensor!");
  }
  */
  delay(1000);
  //SI1145
  /*
  Serial.print("uVisibility: "); 
  Serial.println(uv.readVisible());
  Serial.print("IR: "); 
  Serial.println(uv.readIR());
  */
  float UVindex = uv.readUV();
  UVindex /= 100.0;
  /*
  Serial.print("UV: ");  
  Serial.println(UVindex);
  */
  delay(1000);
  //Server
  EthernetClient cli = server.available();
  if (cli) {
    //Serial.println("New Client!");
    boolean LineIsBlank = true;
    while (cli.connected()) {
      if (cli.available()) {
        char c = cli.read();
        Serial.write(c);
        if (readString.length() < 100) {
          readString += c;
        }
        if (c == '\n' && LineIsBlank) {
          Serial.println(readString); 
          //print to serial monitor for debuging
          cli.println("HTTP/1.1 200 OK");
          cli.println("Content-Type: text/html");
          cli.println("Connection: close");
          cli.println("Refresh: 5");
          cli.println();
          cli.println("<!DOCTYPE HTML>");
          cli.println("<html>");
          cli.println("<head><title>ARDUINO SERVER ");
          cli.print(ID);
          cli.print("</title>");
          cli.println("</head>");
          cli.println("<body>");
          //START
          //ARDUINO DATA
          cli.println("<h4>ARDUINO DATA</h4>");
          cli.print("<p>IP Address: ");
          cli.print(Ethernet.localIP());
          cli.print("</p><p>Device ID: ");
          cli.print(ID);
          cli.print("</p>");
          //DHT
          cli.println("<h4>DHT11 DATA</h4>");
          cli.print("<p>Humidity: ");
          cli.print(h);
          cli.print(" %</p><p>Temperature: ");
          cli.print(t);
          /*
          cli.print(" C</p><p>Heat Index: ");
          cli.print(fl);
          */
          cli.print(" C</p>");
          //BMP
          if (event.pressure) {
            cli.println("<h4>BMP180 DATA</h4>");
            cli.print("<p>Pressure: ");
            cli.print(event.pressure);
            cli.print(" hPa</p><p>Temperature: ");
            float temperature;
            bmp.getTemperature(&temperature);
            cli.print(temperature);
            cli.print(" C</p><p>Heat Index: ");
            float Tf = temperature * 1.8 + 32.0;
            float h_i = dht.computeHeatIndex(Tf, h);
            float f_l = (h_i - 32) * 5 / 9;
            cli.print(f_l);
            cli.print(" C</p><p>Altitude: ");
            //float seaLevelPressure = SENSORS_PRESSURE_SEALEVELHPA;
            float seaLevelPressure = inbg;
            cli.print(bmp.pressureToAltitude(seaLevelPressure,event.pressure)); 
            cli.print(" m</p>");
          }
          //SI1145
          cli.println("<h4>SI1145 DATA</h4>");
          cli.print("<p>UV Radiation: ");
          cli.print(uv.readVisible());
          cli.print(" nm</p><p>UV Index: ");
          cli.print(UVindex);
          cli.print("</p>");
          //end
          cli.println("</body>");
          cli.print("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          LineIsBlank = true;
        }
        else if (c != '\r') {
          //you've gotten a character on the current line
          LineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1000);
    // close the connection:
    cli.stop();
    //Serial.println("Client Disconnected!");
  }
  //Send to server
  if (clientx.connect(cserver, 80)) {
    Serial.println("-> Connected");
    // Make a HTTP request:
    clientx.print("GET /kas/arduino.php");
    //start vars
    clientx.print("?insert=");
    clientx.print("now");
    clientx.print("&hum=");
    clientx.print(h);
    //clientx.print("&hi=");
    //clientx.print(fl);
    clientx.print("&pre=");
    clientx.print(event.pressure);
    clientx.print("&temp=");
    float temperature;
    bmp.getTemperature(&temperature);
    clientx.print(temperature);
    float Tf = temperature * 1.8 + 32.0;
    float h_i = dht.computeHeatIndex(Tf, h);
    float f_l = (h_i - 32) * 5 / 9;
    clientx.print("&hi=");
    clientx.print(f_l);
    clientx.print("&alt=");
    //float seaLevelPressure = SENSORS_PRESSURE_SEALEVELHPA;
    float seaLevelPressure = inbg;
    clientx.print(bmp.pressureToAltitude(seaLevelPressure,event.pressure));
    clientx.print("&uvra=");
    clientx.print(uv.readVisible());
    clientx.print("&uvi=");
    clientx.print(UVindex);
    clientx.print("&id=");
    clientx.print(ID);
    clientx.print("&ip=");
    clientx.print(Ethernet.localIP());
    //end vars
    clientx.println(" HTTP/1.1");
    clientx.print("Host: ");
    clientx.println(cserver);
    clientx.println("Connection: close");
    clientx.println();
    clientx.stop();
    Serial.println("SENT!");
  } else {
    Serial.println("-> Connection failed!");
  }
}

