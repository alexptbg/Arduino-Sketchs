
    #include <SoftwareSerial.h>
    //Serial Relay - Arduino will patch a
    //serial link between the computer and the GPRS Shield
    //at 19200 bps 8-N-1
    //Computer is connected to Hardware UART
    //GPRS Shield is connected to the Software UART

    SoftwareSerial mySerial(2, 3); //RX, TX

    //ligar pino 1 (placa GSM) no pino 3 ARDUINO
    int powerkey =  5;
    int statuspin = 6;
    int pinState = 0;
     
    void setup()
    {
      
      pinMode(powerkey, OUTPUT);
     pinMode(statuspin, INPUT);
     mySerial.begin(115200);               // the GPRS baud rate  
     Serial.begin(115200);                 // the GPRS baud rate
    }
     
    void loop()
    {
      
      pinState = digitalRead(statuspin);
      if(pinState==LOW){
      digitalWrite(powerkey, HIGH);   // set the LED on
      delay(2000);
      digitalWrite(powerkey, LOW);
     }
        if(Serial.available())
        {
           mySerial.write(Serial.read());
         }  
        else  if(mySerial.available())
        {
           Serial.write(mySerial.read());
         }  
    mySerial.println("AT");
    delay(500);
    mySerial.println("AT+IPR=115200");
    delay(300);
    mySerial.println("AT&W0");
    delay(300);
    }
