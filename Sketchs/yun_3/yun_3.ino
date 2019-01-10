
// Possible commands are listed here:
//
// "digital/13"     -> digitalRead(13)
// "digital/13/1"   -> digitalWrite(13, HIGH)
// "analog/2/123"   -> analogWrite(2, 123)
// "analog/2"       -> analogRead(2)
// "mode/13/input"  -> pinMode(13, INPUT)
// "mode/13/output" -> pinMode(13, OUTPUT)
// "inputs/read"    -> Analog pins read

#include <Bridge.h>
#include <YunServer.h>
#include <YunClient.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <rgb_lcd.h>

YunServer server;
SoftwareSerial DEBUG(4,5);

//LCD
rgb_lcd lcd;
const int colorR = 0;
const int colorG = 255;
const int colorB = 255;

int i = 0;

void setup() {
  DEBUG.begin(38400);
  DEBUG.println("Starting...");
  // Bridge startup
  Bridge.begin();
  //digital pins
  pinMode(6,OUTPUT);
  pinMode(7,OUTPUT);
  pinMode(8,OUTPUT);
  pinMode(9,OUTPUT);
  pinMode(10,OUTPUT);
  pinMode(11,OUTPUT);
  pinMode(12,OUTPUT);

  digitalWrite(6,LOW);
  digitalWrite(7,HIGH);
  digitalWrite(8,LOW);
  digitalWrite(9,HIGH);
  digitalWrite(10,LOW);
  digitalWrite(11,HIGH);
  digitalWrite(12,LOW);

  // Listen for incoming connection only from localhost
  // (no one from the external network could connect)
  server.listenOnLocalhost();
  server.begin();
  //DISPLAY
  Wire.begin();// Activate I2C
  //display
  lcd.begin(16,2);
  lcd.setRGB(colorR,colorG,colorB);
  lcd.setCursor(0,0);
  lcd.print("KA-EX.NET");
  lcd.setCursor(0,1);
  lcd.print("SOARES  KARADJOV"); 
  delay(1000);
  lcd.clear();
}

void loop() {
  i++;
  String looping = "Looping... ";
  looping += String(i);
  DEBUG.println(looping);
  
  int pot = analogRead(A1);
  lcd.setCursor(0,0);
  lcd.print("A1:"); 
  lcd.print(pot);
  
  // Get clients coming from server
  YunClient client = server.accept();

  // There is a new client?
  if (client) {
    // Process request
    process(client);

    client.stop();
  }
  delay(50); // Poll every 50ms
}

void process(YunClient client) {
  // read the command
  String command = client.readStringUntil('/');

  client.println("Status: 200");
  client.println("Access-Control-Allow-Origin: *");   
  client.println("Access-Control-Allow-Methods: GET");
  client.println("Content-Type: text/html");
  client.println("Connection: close");
  client.println();

  // is "digital" command?
  if (command == "digital") {
    digitalCommand(client);
  }

  // is "analog" command?
  if (command == "analog") {
    analogCommand(client);
  }

  // is "mode" command?
  if (command == "mode") {
    modeCommand(client);
  }
  
  //inputs test
  if (command == "analogsRead") {
    analogsRead(client);
  }
  if (command == "digitalsRead") {
    digitalsRead(client);
  }
}

void digitalCommand(YunClient client) {
  int pin, value;

  // Read pin number
  pin = client.parseInt();

  // If the next character is a '/' it means we have an URL
  // with a value like: "/digital/13/1"
  if (client.read() == '/') {
    value = client.parseInt();
    digitalWrite(pin, value);
  } 
  else {
    value = digitalRead(pin);
  }

  // Send feedback to client
  client.print(F("Pin D"));
  client.print(pin);
  client.print(F(" set to "));
  client.println(value);

  // Update datastore key with the current pin value
  String key = "D";
  key += pin;
  Bridge.put(key, String(value));
}

void analogCommand(YunClient client) {
  int pin, value;

  // Read pin number
  pin = client.parseInt();

  // If the next character is a '/' it means we have an URL
  // with a value like: "/analog/5/120"
  if (client.read() == '/') {
    // Read value and execute command
    value = client.parseInt();
    analogWrite(pin, value);

    // Send feedback to client
    client.print(F("Pin D"));
    client.print(pin);
    client.print(F(" set to analog "));
    client.println(value);

    // Update datastore key with the current pin value
    String key = "D";
    key += pin;
    Bridge.put(key, String(value));
  }
  else {
    // Read analog pin
    value = analogRead(pin);

    // Send feedback to client
    client.print(F("Pin A"));
    client.print(pin);
    client.print(F(" reads analog "));
    client.println(value);

    // Update datastore key with the current pin value
    String key = "A";
    key += pin;
    Bridge.put(key, String(value));
  }
}

void modeCommand(YunClient client) {
  int pin;

  // Read pin number
  pin = client.parseInt();

  // If the next character is not a '/' we have a malformed URL
  if (client.read() != '/') {
    client.println(F("error"));
    return;
  }

  String mode = client.readStringUntil('\r');

  if (mode == "input") {
    pinMode(pin, INPUT);
    // Send feedback to client
    client.print(F("Pin D"));
    client.print(pin);
    client.print(F(" configured as INPUT!"));
    return;
  }

  if (mode == "output") {
    pinMode(pin, OUTPUT);
    // Send feedback to client
    client.print(F("Pin D"));
    client.print(pin);
    client.print(F(" configured as OUTPUT!"));
    return;
  }

  client.print(F("error: invalid mode "));
  client.print(mode);
}

void analogsRead(YunClient client) {
    String command = client.readString();
    command.trim();
    if (command == "read") {
      //init JSON data
      /*
      client.println("Status:200");
      client.println("content-type:application/json");
      client.println();
      */
      client.println("{");
      /*
      loop through the analog pins and for each pin,
      make a JSON line, like:
      "A0" : 450
      */
      for(int i = 0; i <= 5; i++) {        
        client.print("\"A");
        client.print(i);
        client.print("\":");        
        if (i == 5) {
          client.println(analogRead(i));
        } 
        else {
          client.print(analogRead(i));
          client.println(",");
        }
      }
      //close
      client.println("}");     
    }
}

void digitalsRead(YunClient client) {
    String command = client.readString();
    command.trim();
    if (command == "read") {
      //init JSON data
      /*
      client.println("Status:200");
      client.println("content-type:application/json");
      client.println();
      */
      client.println("{");
      /*
      loop through the analog pins and for each pin,
      make a JSON line, like:
      "D0" : 0
      */
      for(int i = 6; i <= 12; i++) {        
        client.print("\"D");
        client.print(i);
        client.print("\":");        
        if (i == 12) {
          client.println(digitalRead(i));
        } 
        else {
          client.print(digitalRead(i));
          client.println(",");
        }
      }
      //close
      client.println("}");     
    }
}
