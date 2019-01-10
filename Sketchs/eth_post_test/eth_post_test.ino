#include <DHT.h>
#include <Ethernet.h>
#include <SPI.h>

byte mac[] = { 0x90, 0xA2, 0xDA, 0x0F, 0x9F, 0x08 }; // RESERVED MAC ADDRESS

EthernetClient client;
char server[] = "eesystems.net"; // IP Adres (or name) of server to dump data to

int  interval = 5000; // Wait between dumps


#define DHTPIN 2 // SENSOR PIN
#define DHTTYPE DHT11 // SENSOR TYPE - THE ADAFRUIT LIBRARY OFFERS SUPPORT FOR MORE MODELS
DHT dht(DHTPIN, DHTTYPE);

long previousMillis = 0;
unsigned long currentMillis = 0;
//long interval = 250000; // READING INTERVAL

int t = 0;	// TEMPERATURE VAR
int h = 0;	// HUMIDITY VAR
String data;

void setup() { 
	Serial.begin(9600);

	if (Ethernet.begin(mac) == 0) {
		Serial.println("Failed to configure Ethernet using DHCP"); 
	}

	dht.begin(); 
	delay(1000); // GIVE THE SENSOR SOME TIME TO START
	data = "";
}



void loop() {
  	h = (int) dht.readHumidity(); 
	t = (int) dht.readTemperature(); 
  if (client.connect(server, 80)) {
    Serial.println("-> Connected");
    // Make a HTTP request:
    client.print( "GET /kas/out_temp_update.php");
    client.print("?insert=");
    client.print("now");
    client.print("&temp=");
    client.print(t);
    client.print("&humidity=");
    client.print(h);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(server);
    client.println("Connection: close");
    client.println();
    client.stop();
  } else {
    Serial.println("--> connection failed/n");
  }
  delay(interval);
}
