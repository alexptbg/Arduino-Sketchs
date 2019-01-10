#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h>

#include <Adafruit_TMP007.h>
#include <Adafruit_MCP9808.h>
#include <DHT.h>

byte mac[] = { 0x90, 0xA2, 0xDA, 0x0F, 0x64, 0x5D };

EthernetServer server(80);

Adafruit_TMP007 tmp007(0x40);
Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();

#define DHTPIN 2 
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

//BMP180
#define ADDRESS_SENSOR 0x77
int16_t  ac1, ac2, ac3, b1, b2, mb, mc, md;
uint16_t ac4, ac5, ac6;
float T;
float P;
float pinbg = 1017;
float poffset = 63;
uint8_t osd;
// Hardware pressure sampling accuracy modes:
// Ultra Low Power       OSS = 0
// Standard              OSS = 1
// High                  OSS = 2
// Ultra High Resolution OSS = 3
const uint8_t oss = 3;

//SETUP
void setup() {
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
  Serial.println("Starting!");
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  // start the Ethernet connection and the server:
  Ethernet.begin(mac);
  server.begin();
  Serial.print("Server is at: ");
  Serial.println(Ethernet.localIP());
  
  Serial.println("Adafruit TMP007 example");

  // you can also use tmp007.begin(TMP007_CFG_1SAMPLE) or 2SAMPLE/4SAMPLE/8SAMPLE to have
  // lower precision, higher rate sampling. default is TMP007_CFG_16SAMPLE which takes
  // 4 seconds per reading (16 samples)
  if (!tmp007.begin()) {
    Serial.println("No sensor found");
    while (1);
  }
  Serial.println("MCP9808 demo");
  
  // Make sure the sensor is found, you can also pass in a different i2c
  // address with tempsensor.begin(0x19) for example
  if (!tempsensor.begin()) {
    Serial.println("Couldn't find MCP9808!");
    while (1);
  }
  
  Serial.println("DHTxx test!");
  dht.begin();
  
  //BMP180
  if(oss == 0 ) osd =  5;      // Ultra Low Power       OSD =  5ms
  else if(oss == 1 ) osd =  8; // Standard              OSD =  8ms
  else if(oss == 2 ) osd = 14; // High                  OSD = 14ms
  else if(oss == 3 ) osd = 26; // Ultra High Resolution OSD = 26ms
  else { while(1){} }
  init_SENSOR();
  delay(100);

}

void loop() {
  Serial.println("===================================================");
  float objt = tmp007.readObjTempC();
  Serial.print("Object Temperature: "); 
  Serial.print(objt); Serial.println("*C");
  float diet = tmp007.readDieTempC();
  Serial.print("Die Temperature: "); 
  Serial.print(diet); 
  Serial.println("*C");
  delay(4000); // 4 seconds per reading for 16 samples per reading
  
  // Read and print out the temperature, then convert to *F
  float c = tempsensor.readTempC();
  //float f = c * 9.0 / 5.0 + 32;
  Serial.print("Temp: "); 
  Serial.print(c); 
  Serial.println("*C"); 
  delay(250);
  
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.println(" %");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(" *C");
  delay(250);
  
  //BMP180
  int32_t b5;
  b5 = temperature();                       // Read and calculate temperature (T) 
  float temp = T;
  P = pressure(b5);                         // Read and calculate pressure (P) 
  float pre = P+poffset;
  float alt = (pow(pinbg / P, 0.190223f) - 1.0f) * 44330.08f;                       // Use static SI standard temperature (15°C)
  float d_p = dht.dewPoint(temp,h);
  Serial.print("<p>Pressure: ");
  Serial.print(pre);
  Serial.println(" hPa");
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.println("*C");
  Serial.print("DewPoint: ");
  Serial.print(d_p);
  Serial.println("*C");
  Serial.print("Altitude: ");
  Serial.print(alt); 
  Serial.println(" m");
  delay(250);
  

  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
      client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          // output the value of each analog input pin
          for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
            int sensorReading = analogRead(analogChannel);
            client.print("analog input ");
            client.print(analogChannel);
            client.print(" is ");
            client.print(sensorReading);
            client.println("<br />");      
          }
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
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
