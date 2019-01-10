#include <SD.h>

int stepToLoad = 1;
int endOfLineNumber = 0;
int stepCount = 0;
int stringIndex = 0;
File configFile;
char inputString [1280];
char inputChar;


void setup() {
  stepToLoad = 1; 
  if (!SD.begin(4)) {
  return;
  Serial.println("error opening file");
  }
  
 if (SD.exists("text.txt")) {  
 configFile = SD.open("text.txt");
 getStepCount();
 getStepData();
 configFile.close();
}

  
}


void getStepData(){
  
  if (configFile) {
  while (configFile.available()) {  
   inputChar = configFile.read(); // Gets one byte from serial buffer
  if (inputChar != ','){ // define breaking char here 
  
    inputString[stringIndex] = inputChar; // Store it
    stringIndex++; // Increment where to write next
    
  } else {
    endOfLineNumber++;
    stringIndex++;
    if (endOfLineNumber == stepToLoad){  //this is the correct line to load
      
     
      break; //break out of the "while" loop
    } else { //not the correct line to load, reset and continue
      
    stringIndex = 0;  
    }    
  }

  }
  
  
}
}



void getStepCount(){
if (configFile) {
  while (configFile.available()) {  
   inputChar = configFile.read(); // Gets one byte from serial buffer
  if (inputChar == '^'){ // define breaking char here 
  stepCount++;
    } 
  }

}  
}

void loop()
{
	// nothing happens after setup
}
