#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h>

#include <Adafruit_MCP9808.h>

byte mac[] = { 0x90, 0xA2, 0xDA, 0x0F, 0x9F, 0x08 };

String mac_addr = "DE:AD:BE:EF:FE:ED";//4

String ID = "AR_0006_2015_1.0"; //ID

EthernetServer server(80);

EthernetClient clientx;

boolean LineIsBlank = false;
String readString = "";

char cserver[] = "eesystems.net";

Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();


#include <Adafruit_GFX.h>
#include <Adafruit_TFTLCD.h>

#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

#define LCD_RESET -1

#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

void setup(void) {
  Wire.begin();// Activate I2C
  Ethernet.begin(mac);
  server.begin();
  Serial.begin(9600); //Start the serial interface
  Serial.print("IP: ");
  Serial.println(Ethernet.localIP());
  //MCP
  if(!tempsensor.begin()) {
    Serial.println("Couldn't find MCP9808!");
    while(1);
  }
  //start display
  tft.reset();
  uint16_t identifier = tft.readID();
  tft.begin(identifier);
  tft.fillScreen(BLACK);
  tft.setRotation(1);
}

void loop(void) {
  //MCP
  float c = tempsensor.readTempC();
  //display
  tft.setCursor(0,0);
  tft.setTextColor(GREEN);
  tft.setTextSize(5);
  tft.print("INSIDE:");
  tft.setCursor(210,0);
  tft.print(c);
  delay(1000);
  //tft.fillScreen(BLACK);
  tft.fillRect(210,0,150,38,BLACK);
  
}
