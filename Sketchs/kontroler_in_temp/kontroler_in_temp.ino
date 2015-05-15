#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h>

#include <Adafruit_MCP9808.h>
#include <DHT.h>

byte mac[] = { 0x90, 0xA2, 0xDA, 0x0F, 0xC5, 0xD3 };

//String mac_addr = "90:A2:DA:0F:C5:AA";//1
//String mac_addr = "90:A2:DA:0F:C4:B9";//2
//String mac_addr = "90:A2:DA:0F:C5:A2";//3
//String mac_addr = "90:A2:DA:0F:C5:D3";//4
//String mac_addr = "90:A2:DA:0F:C5:A5";//5
String mac_addr = "90:A2:DA:0F:CA:D9";  //6

String ID = "AR_0006_2015_1.0"; //ID

EthernetServer server(80);

EthernetClient clientx;

boolean LineIsBlank = false;
String readString = "";

char cserver[] = "eesystems.net";

Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();

#define DHTPIN 2 
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

float hoffset = 10;
//leds
int red = 3;
int green = 4;

//SETUP
void setup() {
  Wire.begin();// Activate I2C
  Ethernet.begin(mac);
  server.begin();
  Serial.begin(9600); //Start the serial interface
  Serial.print("Server IP: ");
  Serial.println(Ethernet.localIP());
  //leds
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  //MCP
  if(!tempsensor.begin()) {
    Serial.println("Couldn't find MCP9808!");
    while(1);
  }
  //DHT11
  dht.begin();
}

//LOOP
void loop() {
  //leds
  digitalWrite(red, LOW);
  digitalWrite(green, LOW);
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
  delay(250);
  //MCP
  float c = tempsensor.readTempC();
  float d_p = dht.dewPoint(c,h);
  delay(250);
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
          cli.println("Refresh: 10");
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
          cli.print("</p><p>MAC ADDR: ");
          cli.print(mac_addr);
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
          //MCP
          cli.println("<h4>MCP DATA</h4>");
          cli.print("<p>Temperature: ");
          float c = tempsensor.readTempC();
          cli.print(c);
          cli.print(" C</p><p>Dew Point: ");
          cli.print(d_p);
          cli.print(" C</p>");
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
    delay(250);
    cli.stop();
    //Serial.println("Client Disconnected!");
  }
  //Send to server
  if (clientx.connect(cserver, 80)) {
    Serial.println("-> Connected");
    // Make a HTTP request:
    clientx.print("GET /kas/arduino_in_temp.php");
    //start vars
    clientx.print("?in=");
    clientx.print("1");
    clientx.print("&hum=");
    clientx.print(h);
    clientx.print("&temp1=");
    clientx.print(t);
    clientx.print("&temp2=");
    float c = tempsensor.readTempC();
    clientx.print(c);
    clientx.print("&dp=");
    clientx.print(d_p);
    clientx.print("&id=");
    clientx.print(ID);
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
    digitalWrite(green, HIGH);
    delay(1000);
    digitalWrite(green, LOW);
  } else {
    Serial.println("-> Connection failed!");
    digitalWrite(red, HIGH);
    delay(1000);
    digitalWrite(red, LOW);
  }
  delay(11000);
}
