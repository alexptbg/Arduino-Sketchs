#include <Wire.h>
//#include <Adafruit_Sensor.h>
//#include <Adafruit_TSL2561_U.h>

//Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);
//BMP
#define ADDRESS_SENSOR 0x77                 // Sensor address

int16_t  ac1, ac2, ac3, b1, b2, mb, mc, md; // Store sensor PROM values from BMP180
uint16_t ac4, ac5, ac6;                     // Store sensor PROM values from BMP180
float T, P;                                 // Set global variables for temperature and pressure 

uint8_t osd;                                // Oversampling delay 

// Hardware pressure sampling accuracy modes:
// Ultra Low Power       OSS = 0
// Standard              OSS = 1
// High                  OSS = 2
// Ultra High Resolution OSS = 3
const uint8_t oss = 3;  
/*
void configureSensor(void) {
  // tsl.setGain(TSL2561_GAIN_1X);      /* No gain ... use in bright light to avoid sensor saturation */
  // tsl.setGain(TSL2561_GAIN_16X);     /* 16x gain ... use in low light to boost sensitivity */
  //tsl.enableAutoRange(true);            /* Auto-gain ... switches automatically between 1x and 16x */
  
  //tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);      /* fast but low resolution */
  //tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);  /* medium resolution and speed   */
  //tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);  /* 16-bit data but slowest conversions */
//}

void setup(void) {
  Serial.begin(9600);
  Serial.println("Light Sensor Test"); Serial.println("");
  
  /* Initialise the sensor *//*
  if(!tsl.begin())
  {
    /* There was a problem detecting the ADXL345 ... check your connections */
    //Serial.print("Ooops, no TSL2561 detected ... Check your wiring or I2C ADDR!");
    //while(1);
  //}
  
  /* Setup the sensor gain and integration time */
  //configureSensor();
       if(oss == 0 ) osd =  5; // Ultra Low Power       OSD =  5ms
  else if(oss == 1 ) osd =  8; // Standard              OSD =  8ms
  else if(oss == 2 ) osd = 14; // High                  OSD = 14ms
  else if(oss == 3 ) osd = 26; // Ultra High Resolution OSD = 26ms
  else { Serial.print("Incorrect OSS value defined!"); while(1){} }

  init_SENSOR();                            // Initialize baro sensor variables
}

void loop(void) {  
  /* Get a new sensor event */ 
  /*
  sensors_event_t event;
  tsl.getEvent(&event);
 
  /* Display the results (light is measured in lux) */
  /*if (event.light)
  {
    Serial.print(event.light); Serial.println(" lux");
  }
  else
  {
    /* If event.light = 0 lux the sensor is probably saturated
       and no reliable data could be generated! */
    //Serial.println("Sensor overload");
  //}
  delay(250);
  //BMP
  int32_t b5;
 
  b5 = temperature();                       // Read and calculate temperature (T) 

  Serial.print("Temperature: ");
  Serial.print(T, 2);
  Serial.print(" C, ");
  Serial.print(1.8 * T + 32.0, 2);
  Serial.println(" F");

  P = pressure(b5);                         // Read and calculate pressure (P) 
  Serial.println(b5);
  Serial.print("Pressure: ");
  Serial.print(P, 2);
  Serial.print(" mbar, ");
  Serial.print(P * 0.0295299830714, 2);
  Serial.println(" inHg");
  if(P < 300 || P > 1100) Serial.println("Pressure sensor reading out of range!!!!"); // Pressure range according to the data sheet

  float alt = 44330.0 * (1.0 - pow(P / 1016, 0.1903));
  
  Serial.print("Altitude: "); Serial.print(alt); Serial.print(" m, "); Serial.print(alt * 3.28084); Serial.println(" ft");
  Serial.println("-----------------------------------------");

  delay(500);                               // Delay between each readout

}

void init_SENSOR()
{
  ac1 = read_2_bytes(0xAA);
  ac2 = read_2_bytes(0xAC);
  ac3 = read_2_bytes(0xAE);
  ac4 = read_2_bytes(0xB0);
  ac5 = read_2_bytes(0xB2);
  ac6 = read_2_bytes(0xB4);
   b1 = read_2_bytes(0xB6);
   b2 = read_2_bytes(0xB8);
   mb = read_2_bytes(0xBA);
   mc = read_2_bytes(0xBC);
   md = read_2_bytes(0xBE);
  Serial.println("");
  Serial.println("Sensor calibration data:");
  Serial.print(F("ac1 = ")); Serial.print(ac1); Serial.println(F(";"));
  Serial.print(F("ac2 = ")); Serial.print(ac2); Serial.println(F(";"));
  Serial.print(F("ac3 = ")); Serial.print(ac3); Serial.println(F(";"));
  Serial.print(F("ac4 = ")); Serial.print(ac4); Serial.println(F(";"));
  Serial.print(F("ac5 = ")); Serial.print(ac5); Serial.println(F(";"));
  Serial.print(F("ac6 = ")); Serial.print(ac6); Serial.println(F(";"));
  Serial.print(F(" b1 = ")); Serial.print(b1);  Serial.println(F(";"));
  Serial.print(F(" b2 = ")); Serial.print(b2);  Serial.println(F(";"));
  Serial.print(F(" mb = ")); Serial.print(mb);  Serial.println(F(";"));
  Serial.print(F(" mc = ")); Serial.print(mc);  Serial.println(F(";"));
  Serial.print(F(" md = ")); Serial.print(md);  Serial.println(F(";"));
  Serial.println("----------------");
}
/**********************************************
  Calcualte pressure readings
 **********************************************/
float pressure(int32_t b5)
{
  int32_t x1, x2, x3, b3, b6, p, UP;
  uint32_t b4, b7; 

  UP = read_pressure();                          // Read raw pressure
  Serial.print(F("UP = ")); Serial.print(UP); Serial.println(F(";"));

  b6 = b5 - 4000;
  x1 = (b2 * (b6 * b6 >> 12)) >> 11; 
  x2 = ac2 * b6 >> 11;
  x3 = x1 + x2;
  int32_t b3_tmp = ac1;                          // Improved b3 calculation which also uses less program code.
  b3_tmp = (b3_tmp * 4 + x3) << oss;
  b3     = (b3_tmp + 2) >> 2;
  x1 = ac3 * b6 >> 13;
  x2 = (b1 * (b6 * b6 >> 12)) >> 16;
  x3 = ((x1 + x2) + 2) >> 2;
  b4 = (ac4 * (uint32_t)(x3 + 32768)) >> 15;
  b7 = ((uint32_t)UP - b3) * (50000 >> oss);
  if(b7 < 0x80000000) { p = (b7 << 1) / b4; } else { p = (b7 / b4) << 1; } // or p = b7 < 0x80000000 ? (b7 * 2) / b4 : (b7 / b4) * 2;
  x1 = (p >> 8) * (p >> 8);
  x1 = (x1 * 3038) >> 16;
  x2 = (-7357 * p) >> 16;
  return ((p + ((x1 + x2 + 3791) >> 4)) / 100.0f)+60; // Return pressure in mbar
}

/**********************************************
  Read uncompensated temperature
 **********************************************/
int32_t temperature()
{
  int32_t x1, x2, b5, UT;

  Wire.beginTransmission(ADDRESS_SENSOR); // Start transmission to device 
  Wire.write(0xf4);                       // Sends register address
  Wire.write(0x2e);                       // Write data
  Wire.endTransmission();                 // End transmission
  delay(5);                               // Datasheet suggests 4.5 ms
  UT = read_2_bytes(0xf6);                // Read uncompensated TEMPERATURE value
  Serial.print(F("UT = ")); Serial.print(UT); Serial.println(F(";"));

  // Calculate true temperature
  x1 = (UT - ac6) * ac5 >> 15;
  x2 = ((int32_t)mc << 11) / (x1 + md);
  b5 = x1 + x2;
  T  = (b5 + 8) >> 4;
  T = T / 10.0;                           // Temperature in celsius 
  return b5;  
}

/**********************************************
  Read uncompensated pressure value
 **********************************************/
int32_t read_pressure()
{
  int32_t value; 
  Wire.beginTransmission(ADDRESS_SENSOR);   // Start transmission to device 
  Wire.write(0xf4);                         // Sends register address to read from
  Wire.write(0x34 + (oss << 6));            // Write data
  Wire.endTransmission();                   // SEd transmission
  delay(osd);                               // Oversampling setting delay
  Wire.beginTransmission(ADDRESS_SENSOR);
  Wire.write(0xf6);                         // Register to read
  Wire.endTransmission();
  Wire.requestFrom(ADDRESS_SENSOR, 3);      // Request three bytes
  if(Wire.available() >= 3)
  {
    value = (((int32_t)Wire.read() << 16) | ((int32_t)Wire.read() << 8) | ((int32_t)Wire.read())) >> (8 - oss);
  }
  return value;                             // Return value
}

/**********************************************
  Read 1 byte from the BMP sensor
 **********************************************/
uint8_t read_1_byte(uint8_t code)
{
  uint8_t value;
  Wire.beginTransmission(ADDRESS_SENSOR);         // Start transmission to device 
  Wire.write(code);                               // Sends register address to read from
  Wire.endTransmission();                         // End transmission
  Wire.requestFrom(ADDRESS_SENSOR, 1);            // Request data for 1 byte to be read
  if(Wire.available() >= 1)
  {
    value = Wire.read();                          // Get 1 byte of data
  }
  return value;                                   // Return value
}

/**********************************************
  Read 2 bytes from the BMP sensor
 **********************************************/
uint16_t read_2_bytes(uint8_t code)
{
  uint16_t value;
  Wire.beginTransmission(ADDRESS_SENSOR);         // Start transmission to device 
  Wire.write(code);                               // Sends register address to read from
  Wire.endTransmission();                         // End transmission
  Wire.requestFrom(ADDRESS_SENSOR, 2);            // Request 2 bytes from device
  if(Wire.available() >= 2)
  {
    value = (Wire.read() << 8) | Wire.read();     // Get 2 bytes of data
  }
  return value;                                   // Return value
}
