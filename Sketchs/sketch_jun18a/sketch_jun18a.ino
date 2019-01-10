#include <SD.h>

File myFile;

struct parameters {
  int v1;
  int v2;
} settings;

void setup()
{
  Serial.begin(9600);
  Serial.print("Initializing SD card...");

   pinMode(4, OUTPUT);
   
  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
  
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open("settings.txt", FILE_WRITE);
  
  // if the file opened okay, write to it:
  if (myFile) {
    //Serial.print("Writing to test.txt...");
    //myFile.println("testing 1, 2, 3.");
	// close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening settings.txt");
  }
  
  // re-open the file for reading:
  myFile = SD.open("settings.txt");
  if (myFile) {
    Serial.println("settings.txt:");
    
    // read from the file until there's nothing else in it:
    while (myFile.available()) {
    	Serial.write(myFile.read());
        
    }
    // close the file:
    myFile.close();
  } else {
  	// if the file didn't open, print an error:
    Serial.println("error opening settings.txt");
  }
}

void loop()
{
	// nothing happens after setup
}
