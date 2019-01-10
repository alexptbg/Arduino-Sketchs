#include <SPI.h>
#include <Ethernet.h>
#include <Adafruit_NeoPixel.h>
#include <DHT.h>
#include <Timer.h>

#define L1 2
/*
#define RELAY1 7
*/
#define DHTPIN 3
#define DHTTYPE DHT11

Adafruit_NeoPixel strip = Adafruit_NeoPixel(8,L1,NEO_GRB + NEO_KHZ800);

DHT dht(DHTPIN,DHTTYPE);

byte mac[] = { 0x90, 0xA2, 0xDA, 0x0F, 0x9F, 0x08 };

IPAddress ip(192,168,2,89);
IPAddress gateway(192,168,2,1);
IPAddress subnet(255,255,254,0);

char myserver[] = "eesystems.net";

EthernetServer server(80);
EthernetClient cx;

Timer t;

boolean updated = false;

char inString[32];
int stringPos = 0;
boolean startRead = false;

// Color definitions
uint32_t magenta = strip.Color(255,0,255);
uint32_t white = strip.Color(255,255,255);
uint32_t yellow = strip.Color(255,255,0);
uint32_t red = strip.Color(255,0,0);
uint32_t green = strip.Color(0,255,0);
uint32_t orange = strip.Color(255,130,0);
uint32_t purple = strip.Color(130,0,255);
uint32_t d_blue = strip.Color(0,0,255);
uint32_t l_blue = strip.Color(0,255,255);

void setup() {
  //pinMode(RELAY1, OUTPUT);
  strip.begin();
  strip.show();
  strip.setBrightness(25);
  dht.begin();
  Ethernet.begin(mac,ip); 
  server.begin();
  Serial.begin(19200);
  Serial.print("Server internal IP: ");
  Serial.println(Ethernet.localIP());
  int tickEvent = t.every(10000, updateIP);
}

void loop() {
  t.update();
  EthernetClient client = server.available();
  if (updated) {
    strip.setPixelColor(1,green);
  } else {
    strip.setPixelColor(1,red);
  }
  /*
   digitalWrite(RELAY1,LOW);
   delay(2000);
   digitalWrite(RELAY1,HIGH);
   delay(2000);
   */
  /*
  if (LED_status) {
    strip.setPixelColor(4,green);
  } else {
    strip.setPixelColor(4,red);
  }
  */
  strip.setPixelColor(0,green);
  strip.setPixelColor(2,d_blue);
  strip.show();
  delay(400);
  strip.setPixelColor(2,0);
  strip.setPixelColor(6,0);
  strip.setPixelColor(7,0);
  strip.show();
  delay(400); 
  float h = dht.readHumidity();
  //float t = dht.readTemperature();
  float f = dht.readTemperature(true);
  float t = (f - 32) * 5 / 9;
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    strip.setPixelColor(7,red);
    strip.show();
    return;
  } else {
    strip.setPixelColor(7,magenta);
    strip.show();
  }
  //float hi = dht.computeHeatIndex(f,h);
  //float index = (hi - 32) * 5 / 9 - 1;
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(" *C ");
  
  if (client) {
    Serial.println("Connection:");
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.print(c);
        if (c == '\n' && currentLineIsBlank) {
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
	  client.println("Refresh: 5");
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.println("<head>");
          client.println("<style>");
          client.println("body { color:#FF0000; }");
          client.println("table { border: 1px solid #000000; width:300px; }");
          client.println("table td { border: 1px solid #000000; }");
          client.println("p { color:#000000; }");
          client.println("span { color:#FF0000; }");
          client.println("</style>");
          client.println("</head>");
          client.println("<body>");
          //start output
          client.println("<table>");
          //tr1
          client.println("<tr>");
          client.println("<td>");
          client.print("Temperature: ");
          client.println("</td>");
          client.println("<td>");
          client.print("<span>");
          client.print(t);
          client.print("</span>");
          client.println("</td>");
          client.println("</tr>");
          //tr2
          client.println("<tr>");
          client.println("<td>");
          client.print("Humidity:");
          client.println("</td>");
          client.println("<td>");
          client.print("<span>");
          client.print(h);
          client.print("</span>");
          client.println("</td>");
          client.println("</tr>");
          //end table
          client.println("</table>");
          client.println("");
          //end output
          client.println("</body>");
          client.println("</html>");
          //end client
          break;
        }
        if (c == '\n') {
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }
    }
    delay(10);
    client.stop();
    Serial.println("Client disconnected!");
  }
}

void updateIP() {
  stringPos = 0;
  //updated = false;
  memset( &inString, 0, 32 );
  Serial.println("Connecting to server...");
  if (cx.connect(myserver, 80)>0) {
    
    Serial.println("Making HTTP request...");
    cx.println("GET /dev/dev.php?device=ETH_6969 HTTP/1.0");
    cx.println("User-Agent: Arduino 1.0");
    cx.println("HOST: eesystems.net");
    cx.println("Connection: keep-open");
    cx.println();
    //cx.setTimeout(5000);
    strip.setPixelColor(6,orange);
    
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



      /*
      String newString = inString;
      //String stringTime = newString.substring(0,19);
      String sDate = strtok(inString," ");
      String sTime = strtok(NULL," ");
      String sStatus = strtok(NULL," ");
      Serial.println(newString);
      Serial.println(sDate);
      Serial.println(sTime);
      Serial.println(sStatus);
      /*
      if (sStatus == "updated") {
        updated = true;
        Serial.println("Yes");
      } else if (sStatus == "inserted") {
        updated = true;
        Serial.println("Yes");
      } else {
        updated = false;
        Serial.println("No");
      }
      */
