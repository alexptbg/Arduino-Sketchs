#include "IRremote.h"

IRsend irsend;

void setup()
{
  Serial.begin(9600);
}

int khz=38; //NB Change this default value as neccessary to the correct modulation frequency
// ON and 2O C° with 1 FAN heat
//unsigned heat[] = {12952,3450,1700,450,450,400,1300,450,400,450,400,450,400,450,450,400,450,400,450,450,400,450,400,450,450,400,450,400,450,450,1250,450,400,450,450,400,450,450,400,450,400,450,400,450,450,400,1300,450,1250,450,1300,450,400,450,400,450,1250,450,450,400,450,450,400,450,400,450,400,450,450,400,450,450,400,450,400,450,400,450,400,450,450,400,450,450,400,450,400,450,400,450,450,400,450,450,400,450,400,450,400,450};
unsigned heat[] = {12952, 3450, 1700, 450, 450, 400, 1300, 450, 400, 450, 400, 450, 400, 450, 450, 400, 450, 400, 450, 450, 400, 450, 400, 450, 450, 400, 450, 400, 450, 450, 1250, 450, 400, 450, 450, 400, 450, 450, 400, 450, 400, 450, 400, 450, 450, 400, 1300, 450, 1250, 450, 1300, 450, 400, 450, 400, 450, 1250, 450, 450, 400, 450, 450, 400, 450, 400, 450, 400, 450, 450, 400, 450, 450, 400, 450, 400, 450, 400, 450, 400, 450, 450, 400, 450, 450, 400, 450, 400, 450, 400, 450, 450, 400, 450, 450, 400, 450, 400, 450, 400, 450};
// OFF the A/C
unsigned OFF[] = {5900,500,350,400,450,400,1550,200,450,400,450,400,1500,200,1500,250,400,450,1450,200,450,400,450,450,1450,250,1450,250,450,400,1500,200,1500,250,400,450,400,450,1450,250,450,400,450,400,1500,200,450,400,450,400,450,400,450,450,450,400,450,450,400,450,400,450,400,450,450,400,450,400,450,400,450,450,400,450,400,400,450,450,450,400,450,400,450,450,400,400,450,450,400,450,450,400,450,400,450,450,400,450,1500,200};
// ON and 23° with 2 FAN cold
unsigned cold[] = {3050,3000,3000,4400,550,1600,600,550,550,1650,550,550,550,550,550,1650,550,500,600,1600,550,550,550,1650,550,1650,550,1650,550,550,550,550,550,500,600,1600,550,550,550,550,550,1650,550,550,550,550,550,550,550,550,550,550,550,550,550,500,600,1650,500,550,550,600,500,1700,500,550,550,550,550,550,550,550,550,550,550,550,550,1650,550,1650,550,550,550,500,550,1650,550,550,550,550,550,550,550,550,550,550,550,550,550,550,550,550,550,550,550,1650,550,550,550,550,550,550,550,550,550,500,550,550,550,550,550,550,550,550,550,550,550,550,550,550,550,550,550,550,550,550,550,550,550,550,550,550,550,1650,550,1600,600,550,550};
// ON and 20 C° with 1 FAN heat and SWING
unsigned heat_with_swing[] = {3000,3000,3000,4400,550,1650,550,500,600,1600,600,500,600,500,600,1600,550,550,550,1650,550,500,600,1600,600,550,550,550,550,550,550,1600,600,500,600,500,550,550,550,550,550,550,550,550,550,550,550,550,550,550,550,550,550,550,550,550,550,1650,550,550,550,1600,550,550,550,600,500,550,550,550,550,550,550,550,550,550,550,1650,550,1650,550,550,550,550,550,1600,550,1650,550,550,550,550,550,550,550,550,550,550,550,550,550,550,550,1650,550,1650,550,550,550,550,550,550,550,550,500,550,550,550,550,550,550,550,550,550,550,550,550,550,550,550,550,550,550,550,550,550,550,1650,550,1650,550,1650,500,1650,550,550,550,1700,500};
// ON and 23° with 2 FAN cold and SWING
unsigned cold_with_sqwing[] = {2950,3050,2950,4400,550,1600,600,550,550,1650,550,550,550,550,550,1600,600,500,600,1600,550,500,600,1650,550,1650,550,1600,600,500,600,1600,600,500,600,500,550,550,550,550,550,550,550,550,550,550,550,550,550,550,550,500,600,550,550,500,600,1650,550,550,550,1600,600,500,600,1600,550,1600,600,500,600,500,600,550,550,500,600,1650,550,1600,600,500,600,500,550,1650,550,550,550,550,550,550,550,550,550,550,550,550,550,550,550,550,550,550,550,1650,550,500,600,500,600,500,600,500,600,500,550,550,550,550,550,550,550,550,550,550,550,550,550,550,550,550,550,1650,550,1650,550,1600,600,1600,550,550,600,500,550,550,550,550,550};

void loop() {

irsend.sendRaw(heat, sizeof(heat)/sizeof(int), khz);
delay(1000);
irsend.sendRaw(OFF, sizeof(OFF)/sizeof(int),khz);
delay(1000);

}