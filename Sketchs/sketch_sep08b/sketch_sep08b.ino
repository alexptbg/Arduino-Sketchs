//The sample code for driving one way motor encoder
const byte encoder0pinA = 50;//A pin -> the interrupt pin 0
const byte encoder0pinB = 51;//B pin -> the digital pin 4
byte encoder0PinALast;
int duration;//the number of the pulses
boolean Direction;//the rotation direction
  
  
void setup()
{ 
  Serial.begin(9600);//Initialize the serial port
  EncoderInit();//Initialize the module
}
  
void loop()
{
  Serial.print("Pulse:");
  Serial.println(duration);
  duration = 10;
  delay(100);
}
  
void EncoderInit()
{
  Direction = true;//default -> Forward 
  pinMode(encoder0pinB,INPUT); 
  attachInterrupt(51, wheelSpeed, CHANGE);
}
  
void wheelSpeed()
{
  int Lstate = digitalRead(encoder0pinA);
  if((encoder0PinALast == LOW) && Lstate==HIGH)
  {
    int val = digitalRead(encoder0pinB);
    if(val == LOW && Direction)
    {
      Direction = false; //Reverse
    }
    else if(val == HIGH && !Direction)
    {
      Direction = true;  //Forward
    }
  }
  encoder0PinALast = Lstate;
  
  if(!Direction)  duration++;
  else  duration--;
}