
// Possible commands are listed here:
//
// "digital/13"     -> digitalRead(13)
// "digital/13/1"   -> digitalWrite(13, HIGH)
// "analog/2/123"   -> analogWrite(2, 123)
// "analog/2"       -> analogRead(2)
// "mode/13/input"  -> pinMode(13, INPUT)
// "mode/13/output" -> pinMode(13, OUTPUT)

#include <Bridge.h>
#include <Console.h>
#include <YunServer.h>
#include <YunClient.h>
#include <Process.h>

// Listen on default port 5555, the webserver on the Yun
// will forward there all the HTTP requests for us.
YunServer server;

const unsigned long postingInterval = 10000;  //delay between updates to Carriots
unsigned long lastRequest = 0;      // when you last made a request
String dataString = "";

void setup() {
  Serial.begin(9600);
  //Bridge startup
  pinMode(12,OUTPUT);
  pinMode(13,OUTPUT);
  digitalWrite(13, LOW);
  Bridge.begin();
  //Listen for incoming connection only from localhost
  server.listenOnLocalhost();
  //server.noListenOnLocalhost();
  server.begin();
  //init
  lastRequest = millis();
}

void loop() {
  // Get clients coming from server
  YunClient client = server.accept();

  // There is a new client?
  if (client) {
    // Process request
    process(client);

    // Close connection and free resources.
    client.stop();
  }

  delay(50); // Poll every 50ms

  long now = millis();

  // if the sending interval has passed since your
  // last connection, then connect again and send data:
  if (now - lastRequest >= postingInterval) {
    sendData();
    lastRequest = now;
  }
}

void process(YunClient client) {
  // read the command
  String command = client.readStringUntil('/');
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

void sendData() {
  //get time from linux
  String hour = "";
  String mins = "";
  String secs = "";
  String day = "";
  String month = "";
  String year = "";
  String dayn = "1";
  //hour
  Process h;
  h.runShellCommand("date +%H");
  while(h.running());  
  while (h.available()) {
    hour = h.readString();
  } 
  hour.trim();
  delay(50); // Poll every 50ms
  
  //minutes
  Process m;
  m.runShellCommand("date +%M");
  while(m.running());  
  while (m.available()) {
    mins = m.readString();
  } 
  mins.trim();
  delay(50); // Poll every 50ms
  
  //secunds
  Process s;
  s.runShellCommand("date +%S");
  while(s.running());  
  while (s.available()) {
    secs = s.readString();
  } 
  secs.trim();
  delay(50); // Poll every 50ms
  
  //day
  Process d;
  d.runShellCommand("date +%d");
  while(d.running());  
  while (d.available()) {
    day = d.readString();
  } 
  day.trim();
  delay(50); // Poll every 50ms
  
  //month
  Process mo;
  mo.runShellCommand("date +%m");
  while(mo.running());  
  while (mo.available()) {
    month = mo.readString();
  } 
  month.trim();
  delay(50); // Poll every 50ms
  
  //year
  Process y;
  y.runShellCommand("date +%y");
  while(y.running());  
  while (y.available()) {
    year = y.readString();
  } 
  year.trim();
  delay(50); // Poll every 50ms
  
  //day on the week
  /*
  Process p;
  p.runShellCommand("date +%a");
  while(p.running());  
  while (p.available()) {
    dayn = p.readString();
  } 
  dayn.trim();
  delay(50); // Poll every 50ms
  */
  //Generate simulation data
  int stepen = random(0,6);
  float tempW = random(10,120);
  float tempS = random(0,200);
  int burner = random(0,255);
  int setpoint = random(10,40);
  int stat = random(0,4);
  float tempin = random(10,40);
  float tempout = random(-20,40);
  int safeP = 120;
  int pompa = random(0,1);
  int alarm = random(0,1);

  //generate string
  dataString = "in=1";
  dataString += "&sec=" + secs + "&min=" + mins + "&hour=" + hour + "&day=" + day + "&month=" + month + "&year=" + year + "&dayn=" + dayn;
  dataString += "&step=" + String(stepen) + "&tempW=" + String(tempW) + "&tempS=" + String(tempS) + "&burner=" + String(burner) + "&setpoint=" + String(setpoint) + "&status=" + String(stat);
  dataString += "&tempin=" + String(tempin) + "&tempout=" + String(tempout) + "&safeP=" + String(safeP) + "&pump=" + String(pompa) + "&alarm=" + String(alarm);

  //echo string for debug
  Serial.println(dataString);
  
  //Send the HTTP POST request
  Process carriots;
  Serial.println("Sending data... ");
  carriots.begin("curl");
  carriots.addParameter("--data");
  carriots.addParameter(dataString);
  carriots.addParameter("http://127.0.0.1/sd/insert.php");
  carriots.run();
  Serial.println("done!");

  //server response za debug
  while (carriots.available() > 0) {
    char c = carriots.read();
    Serial.write(c);
  }
}

