
#include <Bridge.h>
#include <YunServer.h>
#include <YunClient.h> 

// Listen on default port 5555, the webserver on the Yun
// will forward there all the HTTP requests for us.
YunServer server;

String startString;

void setup() {
  Bridge.begin();
  server.listenOnLocalhost();
  server.begin();
}

void loop() {

  // Get clients coming from server
  YunClient client = server.accept();

  // There is a new client?  
  if(client) {  
    //readStringUntil() "consumes" the string.
    //this means that if command was led/20, now
    //it's just 20.
    String command = client.readStringUntil('/');        
    if (command == "led") {      
      //convert the value from string to int.
      int value = client.parseInt();
      analogWrite(A2, value);
    }    
    client.stop();
  }
}






