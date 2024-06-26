#include <Adafruit_Fingerprint.h>
#include <EEPROM.h>
#include <RTClib.h>

#define startButton 15
#define CREDENTIALS_SIZE sizeof(UserCredentials)
#define EEPROM_START_ADDR 0

SoftwareSerial researchProject(8, 9);
RTC_DS3231 rtc;

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&researchProject);

uint8_t id = 1; // Initialize ID to 1

struct UserCredentials {
  std::string name;
  std::string guardiansName;
  std::string mobileNumber;
  int attendance;
};

UserCredentials credentials;

void setup() {
  pinMode(startButton, INPUT_PULLUP);
  Serial.begin(9600);
  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(100);
  Serial.println("\n\nAdafruit Fingerprint sensor enrollment");

  // set the data rate for the sensor serial port
  finger.begin(57600);
  
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  finger.getTemplateCount();
  Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
  Serial.println("Waiting for valid 
}

// Load credentials from EEPROM for a specific user ID
UserCredentials loadCredentials(uint8_t userId) {
  UserCredentials credentials;
  int addr = EEPROM_START_ADDR + (userId - 1) * CREDENTIALS_SIZE;
  EEPROM.get(addr, credentials);
  return credentials;
}

// Save credentials to EEPROM for a specific user ID
void saveCredentials(uint8_t userId, const UserCredentials &credentials) {
  int addr = EEPROM_START_ADDR + (userId - 1) * CREDENTIALS_SIZE;
  EEPROM.put(addr, credentials);
  EEPROM.commit(); // Don't forget to commit changes to EEPROM
}

void loop() {
  if (digitalRead(startButton) == LOW) {
    enrollFingerprint();
    delay(200);
  }

  finger_status = getFingerprintIDez();
  if (finger_status!=-1 and finger_status!=-2){
    Serial.print("Match");
  } else{
    if (finger_status==-2){
      for (int ii=0;ii<5;ii++){
        Serial.print("Not Match");
      }
    }
  }
  delay(50);
}

int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) {
    Serial.println(p);
    return -1;
  }

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) {
    Serial.println(p);
    return -1;
  }

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK) {
    return -2;
  }

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  Serial.print(" with confidence of "); Serial.println(finger.confidence);

  // Check if attendance needs to be reset
  if (finger.fingerID >= 1 && finger.fingerID <= 127) {
    UserCredentials credentials = loadCredentials(finger.fingerID);
    credentials.attendance++; // Increment attendance for this set of credentials
  if (credentials.attendance == 3) {
    credentials.attendance = 1; // Reset attendance to 1
}
    saveCredentials(finger.fingerID, credentials);


    firstOrSecond(credentials.attendance);
  }

  return finger.fingerID;
}

void getCredentials() {
  Serial.println("Please enter your name:");
  while (Serial.available() == 0);
  credentials.name = Serial.readStringUntil('\n');

  Serial.println("Please enter your guardian's name:");
  while (Serial.available() == 0);
  credentials.guardiansName = Serial.readStringUntil('\n');

  Serial.println("Please enter your phone number:");
  while (Serial.available() == 0);
  credentials.mobileNumber = Serial.readStringUntil('\n');
  
  // Save credentials in EEPROM associated with the fingerprint ID
  int addr = (id - 1) * sizeof(UserCredentials);
  EEPROM.put(addr, credentials);
  EEPROM.commit();
  
  Serial.println("Credentials saved.");
}

void firstOrSecond(int attendance) {
  if (attendance == 1) {
    properGreeting = "your daughter/son has just arrived at school at";
    sendMessage();
  } else if (attendance == 2) {
    properGreeting = "your daughter/son has just left school at";
    sendMessage();
  }
}


void sendMessage() {
  researchProject.println("AT+CMGF=1");
  delay(100);
  String command = "AT+CMGS=\"" + credentials.mobileNumber + "\"\r";
  researchProject.println(command);
  delay(100);

  int currentHour = rtc.now().hour();
  int currentMinute = rtc.now().minute();

  String ampm = (currentHour < 12) ? "AM" : "PM";
  if (currentHour > 12) {
    currentHour -= 12;
  }
  // Determine the appropriate greeting based on the current hour
  std::string greeting;

  if (currentHour >= 1 && currentHour < 12) {
    greeting = "Good morning";
  } else if (currentHour >= 12 && currentHour < 18) {
    greeting = "Good afternoon";
  } else {
    greeting = "Good evening";
  }
  // Construct the complete message
String message = String(greeting.c_str()) + " " + String(credentials.guardiansName.c_str()) + ", " + properGreeting + " " +
                 String(currentHour) + ":" + String(currentMinute) + " " + ampm;

  researchProject.println(message);
  delay(100);
}

void enrollFingerprint() {
  Serial.println("Ready to enroll a fingerprint!");
  Serial.print("Enrolling ID #");
  Serial.println(id);
  
  while (!getFingerprintEnroll());
}

uint8_t getFingerprintEnroll() {

  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  Serial.println("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }
  
  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  Serial.print("Creating model for #");  Serial.println(id);
  
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
  
  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
    id++;
    getCredentials();
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
}
