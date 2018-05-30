long data[8];
long laundry = 0;
long progr = 0;
long step = 0;
String rfid = "14398715 25509504 7071703 1234567";

static unsigned long TimeOut = 0;

#define Sound 5
#define inPin A1
#define blLED 3
#define grnLED 2
#define exLED 4

void setup() {
	Serial.begin(38400);
	pinMode(inPin, INPUT_PULLUP);      // sets the digital pin 7 as input to sense receiver input signal
	pinMode(grnLED, OUTPUT);
	pinMode(blLED, OUTPUT);
  pinMode(exLED, OUTPUT);
	digitalWrite(grnLED, LOW);
	digitalWrite(blLED, LOW);
  digitalWrite(exLED, LOW);
	TimeOut = millis();
	tone(Sound, 4000, 500);
	delay(500);
	tone(Sound, 3500, 300);
	delay(300);
	tone(Sound, 3000, 300);
	delay(300);
}

void loop() {
	if (millis() - TimeOut >= 15000) {
		TimeOut = millis();
		laundry = random(1,4);
		progr = random(1,10);
		step = random(1,8);
		for (int i = 0; i < 8; i++) {
			data[i] = random(0,255);
		}
		//Serial.println(randDEC);
		//print a random number from 10 to 19
		//randKG = random(10, 0x0fffff);
		Serial.print("w:"); Serial.print(laundry); Serial.print(",p:"); Serial.print(progr); Serial.print(",s:"); Serial.print(step);
		for (int i = 0; i < 8; i++) {
			Serial.print(" ");
			Serial.print(data[i]);
		}
		Serial.print(",rfid:"); Serial.print(rfid);
		Serial.print("\r\n");
		digitalWrite(grnLED, HIGH);
		digitalWrite(blLED, HIGH);
		delay(100);
		digitalWrite(grnLED, LOW);
		digitalWrite(blLED, LOW);
	}
}


