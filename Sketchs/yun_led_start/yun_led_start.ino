#include <Bridge.h>
#include <YunServer.h>
#include <YunClient.h>
#include <Process.h>
#include <Wire.h>
#include <rgb_lcd.h>
#include <string.h>

YunServer server;

rgb_lcd lcd;

const int colorR = 0;
const int colorG = 255;
const int colorB = 255;

long previousLCDMillis = 0;
long lcdInterval = 3000;
//screen to show
int screen = 0;   
int screenMax = 4;
bool screenChanged = true;

//screen vars
#define A 0
#define B 1
#define C 2
#define D 3
#define E 4

int Point = 25;
int Step = 1;
int Status = 0;

int t = 0;

String webV = "";
String settings = "";
String inString = ""; 
String data = "";
String sep = ":";

String xval;
String zval;

void setup() {
  Serial.begin(9600);
  //Bridge startup
  pinMode(12,OUTPUT);
  Bridge.begin();
  //Server start
  server.listenOnLocalhost();
  server.begin();
  //start lcd
  lcd.begin(16,2);
  lcd.setRGB(colorR,colorG,colorB);
  //lcd.setCursor(0,0);
  //lcd.print("Starting....");
  showWelcome();
  delay(100);
  //get settings
  Process s;
  s.runShellCommand("curl http://localhost/sd/sql_get_settings.php");
  while(s.running());
  while (s.available()) {
    settings = s.readString();
  }
  settings.trim();
  Serial.println(settings);
  Point = getValue(settings,':',1).toInt();//point
  Step = getValue(settings,':',3).toInt();//step
  Status = getValue(settings,':',5).toInt();//status
  //start
  delay(1000);
  lcd.clear();
}

void loop() {
  unsigned long currentLCDMillis = millis();
  lcd.setCursor(0,0);
  lcd.print(getDateStamp());

  //Get clients coming from server
  YunClient client = server.accept();

  //There is a new client?
  if (client) {
    //Process request
    process(client);
    //Close connection and free resources.
    client.stop();
  } else {
    //get web value
    Process v;
    //p.runShellCommand("ifconfig wlan0 | grep inet | cut -c21-35");
    v.runShellCommand("curl http://eesystems.net/rand.php");
    while(v.running());  
    while (v.available()) {
      webV = v.readString();
    }
    webV.trim();
    xval = getValue(webV,':',0);
    zval = getValue(webV,':',1);
  }
  // MUST WE SWITCH SCREEN?
  if(currentLCDMillis - previousLCDMillis > lcdInterval) {
    previousLCDMillis = currentLCDMillis;
    screen++;
    if (screen > screenMax) screen = 0;  // all screens done? => start over
    screenChanged = true;
  }
  //debug Serial.println(screen);

  // DISPLAY CURRENT SCREEN
  if (screenChanged) {
    screenChanged = false; //reset for next iteration
    switch(screen) {
    case A:
      showStatus(Status);
      break;
    case B:
      showPoint(Point);
      break;
    case C:
      showWebV(xval,zval);
      break;
    case D:
      showSensor();
      break;
    case E:
      showStep(Step);
      break;
    default:
      //cannot happen -> showError() ?
      break;
    }
  }
  //Poll every 50ms
  delay(50);
}

void process(YunClient client) {
  //read the command
  String command = client.readStringUntil('/');

  //is "digital" command?
  if (command == "digital") {
    digitalCommand(client);
  }
  //is "analog" command?
  if (command == "analog") {
    analogCommand(client);
  }
  //is "mode" command?
  if (command == "mode") {
    modeCommand(client);
  }
  //test sets
  if (command == "set") {
    setWhat(client);
  }
}

void setWhat(YunClient client) {

  String what = client.readStringUntil('/');
  
  if (what == "point") {
    Serial.print("Setting Point: ");
    while (client.available() > 0) {
      int inChar = client.read();
      if (isDigit(inChar)) {
        inString += (char)inChar;
      }
    }
    Point = inString.toInt();
    Process p;
    p.begin("curl");
    String myUrl = "http://localhost/sd/sql_save_settings.php?point=";
    p.addParameter(myUrl+Point);
    p.run();
    Serial.println(inString);
    inString = "";
  } else if (what == "step") {
    Serial.print("Setting Step: ");
    while (client.available() > 0) {
      int inChar = client.read();
      if (isDigit(inChar)) {
        inString += (char)inChar;
      }
    }
    Step = inString.toInt();
    Process p;
    p.begin("curl");
    String myUrl = "http://localhost/sd/sql_save_settings.php?step=";
    p.addParameter(myUrl+Step);
    p.run();
    Serial.println(inString);
    inString = "";
  } else if (what == "status") {
    Serial.print("Setting Status: ");
    while (client.available() > 0) {
      int inChar = client.read();
      if (isDigit(inChar)) {
        inString += (char)inChar;
      }
    }
    Status = inString.toInt();
    Process p;
    p.begin("curl");
    String myUrl = "http://localhost/sd/sql_save_settings.php?status=";
    p.addParameter(myUrl+Status);
    p.run();
    Serial.println(inString);
    inString = "";
  } else if (what == "reset") {
    //reset-mcu
    Process s;
    s.runShellCommand("reset-mcu");
    while(s.running());
  } else if (what == "read") {
    data = "";
    data += analogRead(0)+sep+analogRead(1)+sep+analogRead(2)+sep+analogRead(3)+sep+analogRead(4)+sep+analogRead(5)+sep;
    data += Status+sep+Step+sep+Point+sep+xval+sep+zval+sep+digitalRead(12);
    client.print(data);
  }
}

void digitalCommand(YunClient client) {
  int pin, value;

  //Read pin number
  pin = client.parseInt();

  //If the next character is a '/' it means we have an URL with a value like: "/digital/12/1"
  if (client.read() == '/') {
    value = client.parseInt();
    digitalWrite(pin, value);
  } else {
    value = digitalRead(pin);
  }

  //Send feedback to client
  client.print(F("Pin D"));
  client.print(pin);
  client.print(F(" set to "));
  client.println(value);

  //Update datastore key with the current pin value
  String key = "D";
  key += pin;
  Bridge.put(key, String(value));
}

void analogCommand(YunClient client) {
  int pin, value;

  //Read pin number
  pin = client.parseInt();

  //If the next character is a '/' it means we have an URL with a value like: "/analog/5/120"
  if (client.read() == '/') {
    //Read value and execute command
    value = client.parseInt();
    analogWrite(pin, value);

    //Send feedback to client
    client.print(F("Pin A"));
    client.print(pin);
    client.print(F(" set to analog "));
    client.println(value);

    //Update datastore key with the current pin value
    String key = "A";
    key += pin;
    Bridge.put(key, String(value));
  } else {
    //Read analog pin
    value = analogRead(pin);

    //Send feedback to client
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

  //Read pin number
  pin = client.parseInt();

  //If the next character is not a '/' we have a malformed URL
  if (client.read() != '/') {
    client.println(F("error"));
    return;
  }

  String mode = client.readStringUntil('\r');

  if (mode == "input") {
    pinMode(pin, INPUT);
    //Send feedback to client
    client.print(F("Pin D"));
    client.print(pin);
    client.print(F(" configured as INPUT!"));
    return;
  }

  if (mode == "output") {
    pinMode(pin, OUTPUT);
    //Send feedback to client
    client.print(F("Pin D"));
    client.print(pin);
    client.print(F(" configured as OUTPUT!"));
    return;
  }
  client.print(F("error: invalid mode "));
  client.print(mode);
}

String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = { 0, -1  };
  int maxIndex = data.length()-1;
  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
      found++;
      strIndex[0] = strIndex[1]+1;
      strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }
  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void showWelcome() {
  lcd.setCursor(0,0);
  lcd.print("STARTING SYSTEM");
}

void showSensor() {
  //lcd.clear();
  int sensor = random(10,99);
  lcd.setCursor(0,1);
  lcd.print("                ");
  lcd.setCursor(0,1);
  lcd.print("TEMP:");
  lcd.setCursor(14,1);
  lcd.print(sensor);
}

void showStatus(int Status) {
  //lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("                ");
  lcd.setCursor(0,1);
  lcd.print("STATUS:");
  lcd.setCursor(15,1);
  lcd.print(Status); 
}

void showPoint(int Point) {
  //lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("                ");
  lcd.setCursor(0,1);
  lcd.print("SET POINT:");
  lcd.setCursor(14,1);
  lcd.print(Point); 
}

void showStep(int Step) {
  //lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("                ");
  lcd.setCursor(0,1);
  lcd.print("STEP:");
  lcd.setCursor(15,1);
  lcd.print(Step); 
}

void showWebV(String xval,String zval) {
  //lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("                ");
  //lcd.setCursor(0,0);
  //lcd.print("SENSOR WEB1:");
  //lcd.setCursor(14,0);
  //lcd.print(xval);
  lcd.setCursor(0,1);
  lcd.print("SENSOR WEB2:");
  lcd.setCursor(14,1);
  lcd.print(zval); 
}

String getDateStamp() {
  String result;
  Process time;
  time.begin("date");
  time.addParameter("+%Y-%m-%d %H:%M");  
  time.run();
  while(time.available()>0) {
    char c = time.read();
    if(c != '\n')
      result += c;
  }
  return result;
}

