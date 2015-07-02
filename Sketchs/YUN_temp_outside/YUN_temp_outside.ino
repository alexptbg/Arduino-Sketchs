#include <Bridge.h>
#include <Console.h>
#include <HttpClient.h>
#include <Process.h>
#include <YunClient.h>
#include <YunServer.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_MCP9808.h>
#include <DHT.h>

IPAddress server(213,226,1,254);

YunClient client;

#define DHTPIN 4 
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();

String MAC = "90:A2:DA:F3:12:53";//7//yun
String ID = "AR_0007_2015_1.0"; //ID
String IP = "";

float temp;
float hum;
float mcp;

//leds
int red = 5;
int green = 6;

String parametri ="";

void setup(){
  Bridge.begin();
  Serial.begin(9600);
  pinMode(red,OUTPUT);
  pinMode(green,OUTPUT);
  digitalWrite(red,HIGH);
  digitalWrite(green,LOW);
  delay(2000);
  digitalWrite(red,LOW);
  //MCP
  if(!tempsensor.begin()) {
    Serial.println("Couldn't find MCP9808!");
    while(1);
  }
  //DHT11
  dht.begin();
}

void loop(){
  digitalWrite(red, LOW);
  digitalWrite(green, LOW);
  //dht
  hum = (float) dht.readHumidity();
  temp = (float) dht.readTemperature();
  delay(250);
  //mcp
  mcp = (float) tempsensor.readTempC();
  delay(250);
  //bridge ip address
  Process p;
  p.runShellCommand("ifconfig wlan0 | grep inet | cut -c21-35");
  while(p.running());  
  while (p.available()) {
    IP = p.readString();
  } 
  IP.trim();
  delay(250);
  //bridge mac address
  Process m;
  m.runShellCommand("ifconfig wlan0 | grep HWaddr | cut -c39-55");
  while(m.running());  
  while (m.available()) {
    MAC = m.readString();
  }
  MAC.trim();
  delay(250);
  //bridge time
  //date | grep EEST | cut c12-20
  //GET to server
  if (client.connect(server, 80)) {
    Serial.println("connected");
    delay(2000);
    parametri="?out=1&hum=" + String(hum) + "&temp1=" + String(temp) + "&temp2=" + String(mcp) + "&id=" + String(ID) + "&ip=" + String(IP) + "&mac=" + String(MAC);
    client.print("GET /kas/arduino_out_temp.php");
    client.print(parametri);
    client.println(" HTTP/1.1");
    client.println("Host: 213.226.1.254");
    client.print("Content-length:");
    client.println(parametri.length());
    Serial.println(parametri);
    client.println("Connection: Close");
    client.println("Content-Type: application/x-www-form-urlencoded;");
    client.println();
    client.println(parametri);
    digitalWrite(green,HIGH);
    delay(1000);
    digitalWrite(green,LOW);
  } else {
    Serial.println("connection failed");
    digitalWrite(red,HIGH);
    delay(1000);
  }
  if(client.connected()){
    client.stop();
  }
  delay(6000);
}
