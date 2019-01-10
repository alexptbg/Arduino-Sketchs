#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h>
#include <Adafruit_SI1145.h>
#include <DHT.h>
//CONFIG
String ID = "AR_0001_2014_1.0"; //ID
byte mac[] = { 0x90, 0xA2, 0xDA, 0x0F, 0x9F, 0x08 }; // MAC address
EthernetServer server(80);

EthernetClient clientx;

boolean LineIsBlank = false;
String readString = "";

char cserver[] = "eesystems.net";

//BMP180
#define ADDRESS_SENSOR 0x77
int16_t  ac1, ac2, ac3, b1, b2, mb, mc, md;
uint16_t ac4, ac5, ac6;
float T;
float P;
float pinbg = 1018;
float poffset = 62;
uint8_t osd;
// Hardware pressure sampling accuracy modes:
// Ultra Low Power       OSS = 0
// Standard              OSS = 1
// High                  OSS = 2
// Ultra High Resolution OSS = 3
const uint8_t oss = 3;

//SI1145
Adafruit_SI1145 uv = Adafruit_SI1145();

//DHT11
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN,DHTTYPE);
float hoffset = 1;

//SETUP
void setup() {
  Wire.begin();                             // Activate I2C
  Ethernet.begin(mac);
  server.begin();
  Serial.begin(9600); //Start the serial interface
  Serial.print("Server IP: ");
  Serial.println(Ethernet.localIP());
  //BMP180
  if(oss == 0 ) osd =  5;      // Ultra Low Power       OSD =  5ms
  else if(oss == 1 ) osd =  8; // Standard              OSD =  8ms
  else if(oss == 2 ) osd = 14; // High                  OSD = 14ms
  else if(oss == 3 ) osd = 26; // Ultra High Resolution OSD = 26ms
  else { while(1){} }
  init_SENSOR();
  delay(100);
  //SI1145
  if (!uv.begin()) {
    Serial.println("Nqma Si1145");
    while (1);
  }
  //DHT11
  dht.begin();
}

//LOOP
void loop() {
  //DHT
  float h = dht.readHumidity()+hoffset;
  if (h > 99) { h = 100; }
  else if (h < 20) { h = 20; } 
  float t = dht.readTemperature();
  float dp = dht.dewPoint(t,h);
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  delay(1000);
  
  //BMP180
  int32_t b5;
  b5 = temperature();                       // Read and calculate temperature (T) 
  float temp = T;
  P = pressure(b5);                         // Read and calculate pressure (P) 
  float pre = P+poffset;
  float alt = (pow(pinbg / P, 0.190223f) - 1.0f) * 44330.08f;                       // Use static SI standard temperature (15°C)
  float d_p = dht.dewPoint(temp,h);
  delay(1000); 
  
  //SI1145
  float uvr = uv.readVisible();
  float UVindex = uv.readUV();
  float uvi = UVindex /= 100.0;  
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
          cli.println("<head><title>ARDUINO SERVER</title>");
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
          cli.print(" C</p><p>Dew Point: ");
          cli.print(dp);
          cli.print(" C</p>");
          //BMP
          cli.println("<h4>BMP180 DATA</h4>");
          cli.print("<p>Pressure: ");
          cli.print(pre);
          cli.print(" hPa</p><p>Temperature: ");
          cli.print(temp);
          cli.print(" C</p><p>Dew Point: ");
          cli.print(d_p);
          cli.print(" C</p><p>Altitude: ");
          cli.print(alt); 
          cli.print(" m</p>");
          //SI1145
          cli.println("<h4>SI1145 DATA</h4>");
          cli.print("<p>UV Radiation: ");
          cli.print(uvr);
          cli.print(" nm</p><p>UV Index: ");
          cli.print(uvi);
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
    delay(1000);
    cli.stop();
    //Serial.println("Client Disconnected!");
  }
  //Send to server
  if (clientx.connect(cserver, 80)) {
    Serial.println("-> Connected");
    // Make a HTTP request:
    clientx.print("GET /kas/arduino.php");
    //start vars
    clientx.print("?in=");
    clientx.print("1");
    clientx.print("&hum=");
    clientx.print(h);
    clientx.print("&pre=");
    clientx.print(pre);
    clientx.print("&temp=");
    clientx.print(temp);
    clientx.print("&dp=");
    clientx.print(d_p);
    clientx.print("&alt=");
    clientx.print(alt);
    clientx.print("&uvr=");
    clientx.print(uvr);
    clientx.print("&uvi=");
    clientx.print(uvi);
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
    delay(1000);
  } else {
    Serial.println("-> Connection failed!");
  }
  delay(1000);
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

float pressure(int32_t b5) {
  int32_t x1, x2, x3, b3, b6, p, UP;
  uint32_t b4, b7; 
  UP = read_pressure();                          // Read raw pressure
  b6 = b5 - 4000;
  x1 = (b2 * (b6 * b6 >> 12)) >> 11; 
  x2 = ac2 * b6 >> 11;
  x3 = x1 + x2;
  int32_t b3_tmp = ac1;                          // Improved b3 calculation which also uses less program code.
  b3_tmp = (b3_tmp * 4 + x3) << oss;
  b3 = (b3_tmp + 2) >> 2;
  x1 = ac3 * b6 >> 13;
  x2 = (b1 * (b6 * b6 >> 12)) >> 16;
  x3 = ((x1 + x2) + 2) >> 2;
  b4 = (ac4 * (uint32_t)(x3 + 32768)) >> 15;
  b7 = ((uint32_t)UP - b3) * (50000 >> oss);
  if(b7 < 0x80000000) { p = (b7 << 1) / b4; } 
  else { p = (b7 / b4) << 1; } // or p = b7 < 0x80000000 ? (b7 * 2) / b4 : (b7 / b4) * 2;
  x1 = (p >> 8) * (p >> 8);
  x1 = (x1 * 3038) >> 16;
  x2 = (-7357 * p) >> 16;
  return (p + ((x1 + x2 + 3791) >> 4)) / 100.0f; // Return pressure in mbar
}

int32_t temperature() {
  int32_t x1, x2, b5, UT;
  Wire.beginTransmission(ADDRESS_SENSOR); // Start transmission to device 
  Wire.write(0xf4);                       // Sends register address
  Wire.write(0x2e);                       // Write data
  Wire.endTransmission();                 // End transmission
  delay(5);                               // Datasheet suggests 4.5 ms
  UT = read_2_bytes(0xf6);                // Read uncompensated TEMPERATURE value
  // Calculate true temperature
  x1 = (UT - ac6) * ac5 >> 15;
  x2 = ((int32_t)mc << 11) / (x1 + md);
  b5 = x1 + x2;
  T  = (b5 + 8) >> 4;
  T = T / 10.0;                           // Temperature in celsius 
  return b5;  
}

int32_t read_pressure() {
  int32_t value; 
  Wire.beginTransmission(ADDRESS_SENSOR);   // Start transmission to device 
  Wire.write(0xf4);                         // Sends register address to read from
  Wire.write(0x34 + (oss << 6));            // Write data
  Wire.endTransmission();                   // SEd transmission
  delay(osd);                               // Oversampling setting delay
  Wire.beginTransmission(ADDRESS_SENSOR);
  Wire.write(0xf6);                         // Register to read
  Wire.endTransmission();
  Wire.requestFrom(ADDRESS_SENSOR, 3);      // Request three bytes
  if(Wire.available() >= 3) {
    value = (((int32_t)Wire.read() << 16) | ((int32_t)Wire.read() << 8) | ((int32_t)Wire.read())) >> (8 - oss);
  }
  return value;                             // Return value
}

uint8_t read_1_byte(uint8_t code) {
  uint8_t value;
  Wire.beginTransmission(ADDRESS_SENSOR);         // Start transmission to device 
  Wire.write(code);                               // Sends register address to read from
  Wire.endTransmission();                         // End transmission
  Wire.requestFrom(ADDRESS_SENSOR, 1);            // Request data for 1 byte to be read
  if(Wire.available() >= 1) {
    value = Wire.read();                          // Get 1 byte of data
  }
  return value;                                   // Return value
}

uint16_t read_2_bytes(uint8_t code) {
  uint16_t value;
  Wire.beginTransmission(ADDRESS_SENSOR);         // Start transmission to device 
  Wire.write(code);                               // Sends register address to read from
  Wire.endTransmission();                         // End transmission
  Wire.requestFrom(ADDRESS_SENSOR, 2);            // Request 2 bytes from device
  if(Wire.available() >= 2) {
    value = (Wire.read() << 8) | Wire.read();     // Get 2 bytes of data
  }
  return value;                                   // Return value
}
