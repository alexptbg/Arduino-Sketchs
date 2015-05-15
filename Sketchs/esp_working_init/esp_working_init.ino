#include <SoftwareSerial.h>

#define DEBUG true
#define IP "192.168.0.210"
#define PORT 80
#define INV 12

SoftwareSerial Serial1(2,3);

String strName[10] = {};
int Signal[10] = {};
byte Numbers = 0;
boolean	bSeparate = 0;	
String strProba = "";
String Password = "";
String currentLine = "";

void setup() {
  Serial.begin(115200);
  Serial1.begin(38400);
  Serial1.println("Starting...");
  delay(1000);
  sendData("AT+RST\r\n",2000,DEBUG);
  delay(1000);
  sendData("AT\r\n",2000,DEBUG);
  //sendData("AT+CWLAP\r\n",3000,DEBUG);
  strProba = scanNets(3000,DEBUG);
  sendData("AT+CWMODE=1\r\n",1000,DEBUG);
  /*strProba = strProba + 
             "+CWLAP:(0,\"\",0)" + char(10) + char(13) +	
	     "+CWLAP:(2,\"EESYSTEMS\",-42)" + char(10) + char(13) + 
	     "+CWLAP:(4,\"StrGlad\",-86)" + char(10) + char(13) +	
             "+CWLAP:(4,\"Web\",-87)" + char(10) + char(13) + 
	     "+CWLAP:(4,\"B1Mod\",-85)" + char(10) + char(13) +	
	     "+CWLAP:(3,\"BOSS\",-88)";
  */
  Serial1.println(strProba);
  delay(1000);
  if(!bSeparate) {
    bSeparate = 1;
    SeparateString(strProba);
    if(Numbers) {
      SortBySignal();
      //limit 1 network
      Numbers = 1;
    }
    if(Numbers) {
      Serial1.println(" ");
      for (byte i = 0; i <= Numbers; i++) {
        if ((strName[i] != "") && (strName[i] != "0")) {
          if (strName[i] == "EESYSTEMS") {
            Password = "0894587672"; 
          } else {
            Password = "pirint3x";
          }
	  //Serial.println(strName[i] + " .. >> .." + String(Signal[i]));
          String network = "AT+CWJAP=\"";
          network += strName[i];
          network += "\",\"";
          network += Password;
          network += "\"\r\n";
          //Serial.println("Network string");
          //Serial.println(network);
          Serial1.println("Try to connect...");
          conNet(network,2000,DEBUG);
        }
      }
    }
  }
  //sendData("AT+CWJAP=\"EESYSTEMS\",\"0894587672\"\r\n",1000,DEBUG);
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
  cmd = "GET /ems/ar_vodomer.php";
  cmd += "?in=1";
  cmd += "&inv=";
  cmd += INV;
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
  delay(10000);
}

String sendData(String command, const int timeout, boolean debug) {
  String response = "";
  Serial.print(command);
  long int time = millis();
  char c = 0;
  while((time+timeout) > millis()) {
    while(Serial.available()) {
      c = Serial.read(); // read the next character.
      response += c;
      currentLine = response;
      if (c == '\n') {
        currentLine = "";
      }
      if (currentLine.endsWith("no change")) {
        Serial1.println("Nothing has changed.");
      }
      if (currentLine.endsWith("busy now ...")) {
        Serial1.println("BUSY!");
        delay(1000);
        Serial1.println("RESETING!");
        delay(2000);
        software_Reset();
      }      
    }
  }
  if(debug) {
    Serial1.println(response);
  }
  return response;
}

String conNet(String command, const int timeout, boolean debug) {
  String response = "";
  Serial.print(command);
  long int time = millis();
  char c = 0;
  while((time+timeout) > millis()) {
    while(Serial.available()) {
      c = Serial.read(); // read the next character.
      response += c;
      currentLine = response;
      if (c == '\n') {
        currentLine = "";
      }
      /*
      if (currentLine.endsWith("OK")) {
        Serial.println("Connected");
      }
      */     
    }
  }
  if(debug) {
    Serial1.println(response);
  }
  return response;
}

String scanNets(const int timeout, boolean debug) {
  String nets = "";
  String cmd = "AT+CWLAP\r\n";
  Serial.print(cmd);
  long int time = millis();
  char c = 0;
  while((time+timeout) > millis()) {
    while(Serial.available()) {
      c = Serial.read(); // read the next character.
      nets+=c;
    }
  }
  return(nets);
}

void SortBySignal() {
  int max;
  String name;
  for(int i = 0; i < Numbers; i++) {
    for(int j = i + 1; j <= Numbers; j++) {
      if(Signal[i] > Signal[j]) {
	max = Signal[i];
	name = strName[i];
	Signal[i] = Signal[j];
	strName[i] = strName[j];
	Signal[j] = max;
	strName[j] = name;
      }
    }
  }
}

void SeparateString(String Separate) {
  boolean bDetectLine = 0;
  boolean bDetectName = 0;
  int level = 0;
  byte bt = 0;
  for(int i = 0; i < Separate.length(); i ++) {
    if( Separate[i] == '+' && Separate[i + 1] == 'C' && Separate[i + 2] == 'W' && Separate[i + 6] == ':') {
      bDetectLine = 1;
      level = 0;
    }
    if(bDetectLine) {
      if(Separate[i] == char(10)) {
	if(level != 0) {
	  Numbers += 1;
	}
	bDetectLine = 0;
      } else {
	if(bDetectName) {
	  if(Separate[i] == '"') {
	    bDetectName = 0;
	  } else {
	    strName[Numbers] = strName[Numbers] + Separate[i];
	  }
	} else {
	  if(Separate[i] == '"') {
	    bDetectName = 1;
	    strName[Numbers] = "";
	  }
	}			
	if(Separate[i] == ',' && Separate[i + 1] == '-') {
	  for(byte j = 0; j < 3; j ++) {
	    if(Separate[i + 2 + j] >= 48 && Separate[i + 2 + j] <= 57) {
	      level = level * 10;
	      bt = Separate[i + 2 + j];
	      bt = bt - 48;
	      level = level + bt;
	    } else {
	      break;
	    }
	  }
	  Signal[Numbers] = level;
	}
      }
    }
  }
}

void software_Reset() {
  asm volatile ("  jmp 0");  
}
