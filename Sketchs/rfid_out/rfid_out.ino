#include <SoftwareSerial.h>
 
SoftwareSerial RFID = SoftwareSerial(2,3);
 
char character;
String our_id;

void setup()  
{
  
  Serial.begin(9600);
  RFID.begin(9600);
}
 
void loop(){
 
  while(RFID.available()>0)
  {
      character = RFID.read();
      our_id += character;
  }
 
  if (our_id.length() > 10) {
      our_id = our_id.substring(3,11);
      Serial.println(our_id);
      our_id = "";
  }
  delay(1000);
}
unsigned int hexToDec(String hexString) {
  unsigned int decValue = 0;
  int nextInt;
  for (int i = 0; i < hexString.length(); i++) {
    nextInt = int(hexString.charAt(i));
    if (nextInt >= 48 && nextInt <= 57) nextInt = map(nextInt, 48, 57, 0, 9);
    if (nextInt >= 65 && nextInt <= 70) nextInt = map(nextInt, 65, 70, 10, 15);
    if (nextInt >= 97 && nextInt <= 102) nextInt = map(nextInt, 97, 102, 10, 15);
    nextInt = constrain(nextInt, 0, 15);
    decValue = (decValue * 16) + nextInt;
  }
  return decValue;
}
