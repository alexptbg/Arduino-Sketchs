#include <SPI.h>
#include <Ethernet.h>

byte mac[] = { 0x90, 0xA2, 0xDA, 0x0F, 0xCA, 0xD9 };
//IPAddress ip(192,168,2,241);

char server[] = "ka-ex.net";    
String dataLocation = "/ip1.php HTTP/1.1";

EthernetClient client;
String currentLine = "";            // string for incoming serial data
String currRates = "";
boolean readingRates = false;       // is reading?
const int requestInterval = 1000000; // milliseconds delay between requests

boolean requested;                  // whether you've made a request since connecting
long lastAttemptTime = 0;           // last time you connected to the server, in milliseconds

void setup() {
  Serial.begin(9600);
  //Ethernet.begin(mac,ip);
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
  }
  Serial.print("Server IP: ");
  Serial.println(Ethernet.localIP());
  connectToServer();
}


void loop() { 

  if (client.connected()) {
    if (client.available()) {
      // read incoming bytes:
      char inChar = client.read();

      // add incoming byte to end of line:
      currentLine += inChar;

      // if you get a newline, clear the line:
      if (inChar == '\n') {
        currentLine = "";
      }

      if (currentLine.endsWith("<ip>")) {
        readingRates = true;
      } 
      else if (readingRates) {
        if (!currentLine.endsWith("</ip>")) { //'>' is our ending character
          currRates += inChar;
        } 
        else {
          readingRates = false;

          String justRates = currRates.substring(0, currRates.length()-4);
          Serial.println(justRates);

          // Split justRates
          int firstSpaceIndex = justRates.indexOf(" ");
          int secondSpaceIndex = justRates.indexOf(" ", firstSpaceIndex+1);
          String firstVal = justRates.substring(0, firstSpaceIndex);
          String secondVal = justRates.substring(firstSpaceIndex+1, secondSpaceIndex);
          String thirdVal = justRates.substring(secondSpaceIndex);

          currRates = "";
          client.stop();
          Serial.println("Disconnected");
        }
      }
    }
  } 
  else if (millis() - lastAttemptTime > requestInterval) {
    connectToServer();
  }
}


void connectToServer() {
  Serial.println("connecting to server...");
  if (client.connect(server, 80)) {
    Serial.println("making HTTP request...");
    client.println("GET " + dataLocation);
    client.println("Host: ka-ex.net");
    client.println();
  }
  lastAttemptTime = millis();
}

float strToFloat(String str) {
  char carray[str.length() + 1];           // determine size of array
  str.toCharArray(carray, sizeof(carray)); // put str into an array
  return atof(carray);
}
