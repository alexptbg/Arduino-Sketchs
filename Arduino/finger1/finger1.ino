#include <Adafruit_Fingerprint.h>

#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
SoftwareSerial mySerial(2,3);
#else
#define mySerial Serial1
#endif

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

#define SOUND 5

int incomingByte = 0;
volatile int finger_status = -1;
bool matchFound = false;
uint8_t id;
static bool isEnroll = false;
static bool reload = false;
String comRead;
int getFingerprintIDez();

void setup() {
  tone(SOUND, 4600, 100);
  delay(100);
  tone(SOUND, 4200, 100);
  delay(100);
  tone(SOUND, 3800, 100);
  delay(100);
  tone(SOUND, 3400, 100);
  delay(100);
  tone(SOUND, 3000, 100);
  delay(100);
  Serial.begin(38400);
  while (!Serial);
  delay(100);
  Serial.println("Alex finger detect test");
  finger.begin(57600);
  delay(10);
  if (finger.verifyPassword()) {
    Serial.println("Found the fingerprint sensor!");
    tone(SOUND, 4600, 200);
    delay(200);
    tone(SOUND, 4200, 200);
    delay(200);
    tone(SOUND, 4600, 200);
    delay(200);
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }
  Serial.println(F("Fingerprint sensor parameters"));
  finger.getParameters();
  Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
  Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
  Serial.print(F("Capacity: ")); Serial.println(finger.capacity);
  Serial.print(F("Security level: ")); Serial.println(finger.security_level);
  Serial.print(F("Device address: ")); Serial.println(finger.device_addr, HEX);
  Serial.print(F("Packet len: ")); Serial.println(finger.packet_len);
  Serial.print(F("Baud rate: ")); Serial.println(finger.baud_rate);
  finger.getTemplateCount();
  if (finger.templateCount == 0) {
    Serial.print("Fingerprint sensor doesn't contain any fingerprint data.");
  } else {
    Serial.println("Waiting for valid finger...");
    Serial.print("Sensor contains "); 
    Serial.print(finger.templateCount);
    Serial.println(" templates");
  }
  //cleat sensor database use carefull
  //finger.emptyDatabase();
  //Serial.println("Now database is empty :)");
}

void loop() {
  //getFingerprintID();
  //check if serial data incomes
  if (Serial.available() > 0) {
    //read the incoming byte:
    incomingByte = Serial.read();
    //say what you got:
    //Serial.print("I received: ");
    //Serial.println(incomingByte,DEC);
    //char97="a" //add a fringer print
    //char99="c" //count fringer prints
    //char100="d" //delete a finger print
    //char114="r" //reset the controller
    if (incomingByte==97) {
      Serial.println("OK1\r\n");
      atest();
    } else if (incomingByte==99) {
      Serial.print("Finger print sensor templates: ");
      finger.getTemplateCount();
      Serial.println(finger.templateCount);
    } else if (incomingByte==114) {
      Serial.println("Reset?!?");
      reset();
    } else if (incomingByte==100) {
      Serial.println("OK3\r\n");
      adel();
    }
  } else {
    if(isEnroll==false) {
      //getFingerprintID();
      matchFound = false;
      reload = false;
      finger_status = getFingerprintIDezAlex();
      if (!matchFound) {
        if (finger_status != -1 && finger_status != -2) {
          Serial.println("Weapon Activated");
          matchFound = true; // Set the match found flag
          tone(SOUND, 4600, 100);
          delay(100);
        } else {
          if (finger_status == -2) {
            Serial.println("Access Denied");
            matchFound = false;
            tone(SOUND, 3000, 100);
            delay(120);
            tone(SOUND, 3000, 100);
            delay(100);
          }
        }
      }
      delay(50); // don't need to run this at full speed.
    }
  }
}

uint8_t getFingerprintID() {
  //read
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      //Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  //OK success!
  p = finger.image2Tz();
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
  //OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }
  //found a match!
  Serial.print("Found ID #"); 
  Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); 
  Serial.println(finger.confidence);
  return finger.fingerID;
}

uint8_t readnumber(void) {
  uint8_t num = 0;
  while (num == 0) {
    while (! Serial.available());
    num = Serial.parseInt();
  }
  return num;
}

int getFingerprintIDezAlex() {
  uint8_t p = finger.getImage();
  if (p != 2) {
    Serial.println(p);
  }
  if (p != FINGERPRINT_OK) return -1;
  p = finger.image2Tz();
  if (p != 2) {
    Serial.println(p);
  }
  if (p != FINGERPRINT_OK) return -1;
  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK) return -2;
  //found a match!
  Serial.print("Found ID #");
  Serial.print(finger.fingerID);
  Serial.print(" with confidence of ");
  Serial.println(finger.confidence);
  return finger.fingerID;
}

void atest() {
  isEnroll = true;
  Serial.println("Ready to enroll a fingerprint!");
  Serial.println("Please type in the ID # (from 1 to 255) you want to save this finger as...");
  id = readnumber();
  if (id == 0) {// ID #0 not allowed, try again!
   reload = true;
   reloadX();
   //return;
  }
  //while (! getFingerprintEnroll() ); if increased id repeat
  if(reload == true) {
    reloadX();
  } else {
    reload = false;
    isEnroll = true;
    getFingerprintEnroll();
  }
}

void reloadX() {
  Serial.println("Error, reloading loop.");
  incomingByte = 0;
  isEnroll = false;
  delay(500);
}

uint8_t getFingerprintEnroll() {
  if(isEnroll==true && reload==false) {
    Serial.print("Enrolling ID #");
    Serial.println(id);
    int p = -1;
    Serial.print("Waiting for valid finger to enroll as #"); 
    Serial.println(id);
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
    Serial.print("ID "); 
    Serial.println(id);
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
    Serial.print("Creating model for #");
    Serial.println(id);
    p = finger.createModel();
    if (p == FINGERPRINT_OK) {
      Serial.println("Prints matched!");
    } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
      Serial.println("Communication error");
      return p;
    } else if (p == FINGERPRINT_ENROLLMISMATCH) {
      Serial.println("Fingerprints did not match");
      reloadX();
      return p;
    } else {
      Serial.println("Unknown error");
      return p;
    }   
    Serial.print("ID "); 
    Serial.println(id);
    p = finger.storeModel(id);
    if (p == FINGERPRINT_OK) {
      Serial.println("Stored!");
      tone(SOUND, 4600, 100);
      delay(100);
      tone(SOUND, 4600, 100);
      delay(300);
      //id++;
      incomingByte = 0;
      return isEnroll = false;
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
  } else {
    reloadX();
  }
}

void printHex(int num, int precision) {
  char tmp[16];
  char format[128];
  sprintf(format, "%%.%dX", precision);
  sprintf(tmp, format, num);
  Serial.print(tmp);
}

//delete test
void adel() {
  isEnroll = false;
  Serial.println("Please type in the ID # (from 1 to 255) you want to delete...");
  uint8_t id = readnumber();
  if (id == 0) {// ID #0 not allowed, try again!
     return;
  }
  Serial.print("Deleting ID #");
  Serial.println(id);
  deleteFingerprint(id);
}

uint8_t deleteFingerprint(uint8_t id) {
  uint8_t p = -1;
  p = finger.deleteModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Deleted!");
    tone(SOUND, 3000, 100);
    delay(100);
    tone(SOUND, 3000, 100);
    delay(100);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not delete in that location");
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
  } else {
    Serial.print("Unknown error: 0x"); 
    Serial.println(p, HEX);
  }
  return p;
}

void reset() {
  tone(SOUND, 3000, 100);
  delay(100);
  tone(SOUND, 3000, 100);
  delay(100);
  tone(SOUND, 3000, 100);
  delay(100); 
  asm volatile ("jmp 0"); 
}

/* EOF */
