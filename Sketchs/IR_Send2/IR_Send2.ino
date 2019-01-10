

#define LED 13
char rxChar = 0;

//=== function to print the command list:  ===========================
void printHelp(void){
  Serial.println("--- Command list: ---");
  Serial.println("? -> Print this HELP");  
  Serial.println("s ->     \"status\"");  
}
  
//---------------- setup ---------------------------------------------
void setup(){
  Serial.begin(9600);   // Open serial port (9600 bauds).
  pinMode(LED, OUTPUT); // Sets pin 13 as OUTPUT.
  Serial.flush();       // Clear receive buffer.
  printHelp();          // Print the command list.
}

//--------------- loop ----------------------------------------------- 
void loop(){
  if (Serial.available() >0){          // Check receive buffer.
    rxChar = Serial.read();            // Save character received. 
    Serial.flush();                    // Clear receive buffer.
  
  switch (rxChar) {
    
    case 'a':
    case 'A':                          // If received 'a' or 'A':
	if (digitalRead(LED) == LOW){        // If LED is Off:
          digitalWrite(LED,HIGH);      // Turn On the LED.
          Serial.println("LED turned On");
	}
        else Serial.println("LED already On!");
        break;

    case 'd':
    case 'D':                          // If received 'd' or 'D':
	if (digitalRead(LED) == HIGH){       // If LED is On:
          digitalWrite(LED,LOW);       // Turn Off the LED.
          Serial.println("LED turned Off");
	}
        else Serial.println("LED already Off!");
        break;
        
    case 's':
    case 'S':                          // If received  's' or 'S':
	if (digitalRead(LED) == HIGH)        // Read LED status.
          Serial.println("LED status: On");
        else Serial.println("LED status: Off");
        break;
        
    case '?':                          // If received a ?:
        printHelp();                   // print the command list.
        break;
        
    default:                           
      Serial.println((char)rxChar);
    }
  }
}
// End of the Sketch.
