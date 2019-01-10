#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>

Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);
float pinbg = 1016;
float offset = 62;

void setup(void) {
  Serial.begin(9600);
  Serial.println("Pressure Sensor Test"); 
  Serial.println("");

  if(!bmp.begin()) {
    Serial.print("Ooops, no BMP085 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
}

void loop(void) {
  sensors_event_t event;
  bmp.getEvent(&event);
 
  if (event.pressure) {
    Serial.print("Pressure:    ");
    Serial.print(event.pressure+offset);
    Serial.println(" hPa");

    float temperature;
    bmp.getTemperature(&temperature);
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" C");

    float seaLevelPressure = pinbg;
    Serial.print("Altitude:    "); 
    Serial.print(bmp.pressureToAltitude(seaLevelPressure,event.pressure)); 
    Serial.println(" m");
    Serial.println("");
  } else {
    Serial.println("Sensor error");
  }
  delay(1000);
}
