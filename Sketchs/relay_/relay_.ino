#define RELAY1 2
#define RELAY2 3

#define D1 A0
#define D2 A1
#define D3 A2

#define POT A3

int offset = -80;
int pot_val = 0;
int pot_end = 0;

void setup() {    
  Serial.begin(9600); 
  pinMode(RELAY1,OUTPUT);
  pinMode(RELAY2,OUTPUT);
}

void loop() {
  int val1 = analogRead(D1);
  int val2 = analogRead(D2);
  int val3 = analogRead(D3);
  
  pot_val = analogRead(POT);
  
  Serial.print("POT:");
  Serial.println(pot_val);
  
  pot_end = map(pot_val,0,1023,0,30);
  
  int val11 = map(val1,550,1023,0,100);
  int val22 = map(val2,550,1023,0,100);
  int val33 = map(val3,550,1023,0,100);  
  
  int limit = offset + pot_end;
  
  Serial.print("D1:");
  Serial.print(val1);
  Serial.print(" - ");
  Serial.println(val11);
  
  Serial.print("D2:");
  Serial.print(val2);
  Serial.print(" - ");
  Serial.println(val22);
  
  Serial.print("D3:");
  Serial.print(val3);
  Serial.print(" - ");
  Serial.println(val33);
  
  Serial.print("LIMIT:");
  Serial.println(limit);
  
  if (val11 > limit || val22 > limit || val33 > limit) {
    digitalWrite(RELAY1,1);
    digitalWrite(RELAY2,1);
  } else {
    digitalWrite(RELAY1,0);
    digitalWrite(RELAY2,0);
  }
  delay(2000);
}

