#define DEBUG true
#define IP "192.168.0.210";

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200); // your esp's baud rate might be different
  sendData("AT+RST\r\n",2000,DEBUG); // reset module
  Serial.println("Ready");
  sendData("AT+CWMODE=1\r\n",1000,DEBUG); // configure as access point
  sendData("AT+CWJAP=\"EESYSTEMS\",\"0894587672\"\r\n",1000,DEBUG); // turn on server on port 80
  sendData("AT+CIFSR\r\n",1000,DEBUG); // get ip address
  sendData("AT+CIPMUX=1\r\n",1000,DEBUG); // configure for multiple connections
  //sendData("AT+CIPSERVER=1,80\r\n",1000,DEBUG); // turn on server on port 80
}

void loop() {
  if(Serial1.available()) {
    Serial.println("Available!"); 
  }
  delay(2000);

  int cnt1 = random(100,200);
  int cnt2 = random(300,400);
  
  String cmd = "AT+CIPSTART=\"TCP\",\""; //make this command: AT+CPISTART="TCP","192.168.88.35",80
  cmd += IP;
  cmd += "\",4013";
  cmd += "\r\n";
  //esp8266.println(cmd);
  sendData("AT+CPISTART=\"TCP\",\"192.168.0.210\",4013\r\n",100,DEBUG); 
  Serial.println(cmd);
  //wait a little while for 'Linked'
  delay(300);
  /*
  /*
  //This is our HTTP GET Request change to the page and server you want to load.
  cmd = "GET /ems/ar_vodomer.php?in=1&inv=12";
  cmd += "&cnt1=";
  cmd += cnt1;
  cmd += "&cnt2=";
  cmd += cnt2;
  cmd += " HTTP/1.0\r\n";
  cmd += "Host: 192.168.0.210\r\n\r\n";
  cmd += "\r\n";
  //The ESP8266 needs to know the size of the GET request
  //esp8266.print("AT+CIPSEND=");
  //esp8266.println(cmd.length());
  String final = "AT+CIPSEND=";
  final += cmd.length();
  final += "\r\n";
  sendData(final,10,DEBUG);
  Serial.println(final);
  //Serial.print("AT+CIPSEND=");
  //Serial.println(cmd.length());
 sendData(cmd,100,DEBUG);
  //Look for the > prompt from the esp8266
  //esp8266.println(cmd);
  //Serial.println(cmd);
  if(esp8266.find(">")) {
    Serial.println("FOUND!");
    //Send our http GET request
    sendData(cmd,1000,DEBUG);
    
    //esp8266.println(cmd);
  } else {
    Serial.println("NOT FOUND!");
    //Something didn't work...
    //esp8266.println("AT+CIPCLOSE");
  }
 
  //Check the returned header & web page. Looking for a keyword. I used YES12321
  if (esp8266.find("YES")) {
    //If the string was found we know the page is up and we turn on the LED status
    //light to show the server is ONLINE
    Serial.println("SENT!");
  } else {
    //If the string was not found then we can assume the server is offline therefore
    //we should turn of the light.
    Serial.println("ERROR");
  }
  //Wait a second for everything to settle.
  delay(1000);
  //Be great people and close our connection.
  esp8266.println("AT+CIPCLOSE");
  //Introduce a delay timer before we finish and go back to the begining.
  delay(5000);

  
  
  
  
  
  /*
  
  
  String cmd = "AT+CIPSTART=\"TCP\",\"";  //make this command: AT+CPISTART="TCP","146.227.57.195",80
  cmd += IP;
  cmd += "\",80";
  sendData(cmd,1000,DEBUG);
  Serial.println(cmd);  //send command to device

  delay(2000);  //wait a little while for 'Linked' response - this makes a difference


  cmd = GET;  //construct http GET request
  cmd += "&cnt1=";
  cmd += cnt1;
  cmd += "cnt2=";
  cmd += cnt2;
  cmd += " HTTP/1.0\r\n";
  cmd += "Host: eesystems.net\r\n\r\n";        //test file on my web
  sendData(cmd,1000,DEBUG);
  sendData("AT+CIPSEND=",1000,DEBUG);
  //sendData(cmd.length(),1000,DEBUG);
  Serial.print("AT+CIPSEND=");                //www.cse.dmu.ac.uk/~sexton/test.txt
  Serial.println(cmd.length());  //esp8266 needs to know message length of incoming message - .length provides this


   // Serial.println("found > prompt - issuing GET request");  //a debug message
    sendData(cmd,1000,DEBUG);
    Serial.println(cmd);  //this is our http GET request



  
  /*
  if(esp8266.available()) // check if the esp is sending a message 
  {
    /*
    while(esp8266.available())
    {
      // The esp has data so display its output to the serial window 
      char c = esp8266.read(); // read the next character.
      Serial.write(c);
    } */
    /*
    if(esp8266.find("+IPD,"))
    {
     delay(1000);
 
     int connectionId = esp8266.read()-48; // subtract 48 because the read() function returns 
                                           // the ASCII decimal value and 0 (the first decimal number) starts at 48
     
     String webpage = "<h1>Hello</h1><h2>World!</h2><button>LED1</button>";
 
     String cipSend = "AT+CIPSEND=";
     cipSend += connectionId;
     cipSend += ",";
     cipSend +=webpage.length();
     cipSend +="\r\n";
     
     sendData(cipSend,1000,DEBUG);
     sendData(webpage,1000,DEBUG);
     
     webpage="<button>LED2</button>";
     
     cipSend = "AT+CIPSEND=";
     cipSend += connectionId;
     cipSend += ",";
     cipSend +=webpage.length();
     cipSend +="\r\n";
     
     sendData(cipSend,1000,DEBUG);
     sendData(webpage,1000,DEBUG);
 
     String closeCommand = "AT+CIPCLOSE="; 
     closeCommand+=connectionId; // append connection id
     closeCommand+="\r\n";
     
     sendData(closeCommand,3000,DEBUG);
    }
  }*/
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
  if (Serial1.find("YES")) {
    //If the string was found we know the page is up and we turn on the LED status
    //light to show the server is ONLINE
    Serial.println("SENT!");
  } 
    if(debug) {
      Serial.println(response);
    }
    return response;
}
/*
String sendData(String command, const int timeout, boolean debug) {
    String response = "";
    esp8266.print(command); // send the read character to the esp8266
    long int time = millis();
    while( (time+timeout) > millis()) {
      while(esp8266.available()) {
        // The esp has data so display its output to the serial window 
        char c = esp8266.read(); // read the next character.
        response+=c;
      }  
    }
  if (esp8266.find("YES")) {
    //If the string was found we know the page is up and we turn on the LED status
    //light to show the server is ONLINE
    Serial.println("SENT!");
  } 
    if(debug) {
      Serial.println(response);
    }
    return response;
}*/
