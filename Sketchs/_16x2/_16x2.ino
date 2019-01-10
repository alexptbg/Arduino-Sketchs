#include <Wire.h>
#include <rgb_lcd.h>

rgb_lcd lcd;

const int colorR = 233;
const int colorG = 0;
const int colorB = 128;

void setup() {

    lcd.begin(16,2);
    lcd.setRGB(colorR,colorG,colorB);
    lcd.print("hello, world!");
    delay(1000);
}

void loop() {
    // set the cursor to column 0, line 1
    // (note: line 1 is the second row, since counting begins with 0):
    lcd.setCursor(0, 1);
    lcd.print(millis()/1000);
    delay(100);
}

