#include <SoftwareSerial.h>

#define DEBUG true
#define IP "192.168.0.210"
#define PORT 80

SoftwareSerial Serial1(2,3);

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);
  Serial.println("Starting...");
  sendData("AT+RST\r\n",2000,DEBUG);
  sendData("AT+CWMODE=1\r\n",1000,DEBUG);
  sendData("AT+CWJAP=\"EESYSTEMS\",\"0894587672\"\r\n",1000,DEBUG);
  delay(5000);
  sendData("AT+CIFSR\r\n",1000,DEBUG);
  sendData("AT+CIPMUX=0\r\n",1000,DEBUG);
  //sendData("AT+CIPSERVER=1,80\r\n",1000,DEBUG); // turn on server on port 80
}

void loop() {
  // put your main code here, to run repeatedly:
  int cnt1 = random(100,200);
  int cnt2 = random(300,400);
  //connect
  //sendData("AT+CIPSTART=\"TCP\",\"192.168.0.210\",21",1000,DEBUG);
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += IP;
  cmd += "\",";
  cmd += PORT;
  cmd += "\r\n";
  sendData(cmd,1000,DEBUG);
  //sendData("AT+CIPSTART=\"TCP\",\"192.168.0.210\",21",1000,DEBUG);
  //Serial.println(cmd);
  delay(10000);
  
  //construct data
  cmd = "GET /ems/ar_vodomer.php?in=1&inv=12";
  cmd += "&cnt1=";
  cmd += cnt1;
  cmd += "&cnt2=";
  cmd += cnt2;
  cmd += " HTTP/1.0\r\n";
  cmd += " Host: 192.168.0.210\r\n\r\n";
  //count data
  String final = "AT+CIPSEND=";
  final += cmd.length();
  final += "\r\n";
  sendData(final,1000,DEBUG);
  delay(100);
  //send data
  sendData(cmd,1000,DEBUG);
  delay(5000);
  //close connection
  sendData("AT+CIPCLOSE\r\n",1000,DEBUG);
  delay(5000);
}

String sendData(String command, const int timeout, boolean debug) {
    String response = "";
    Serial1.print(command); // send the read character to the esp8266
    long int time = millis();
    while( (time+timeout) > millis()) {
      while(Serial1.available()) {
        // The esp has data so display its output to the serial window 
        char c = Serial1.read(); // read the next character.
        response+=c;
      }  
    }
    if(debug) {
      Serial.println(response);
    }
    return response;
}
