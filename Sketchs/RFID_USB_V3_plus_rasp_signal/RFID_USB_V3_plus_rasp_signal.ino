#include "TimerOne.h"
//PIRINTEX RFID BY KARADJOV AND ALEX

#define RX 2
#define TX 3

int val;
int bitTimeH = 0;
int bitTimeL = 0;
int curState = 0;

#define maxBuf 400 
#define debug  0

#define Sound 5
#define inPin A1

byte raw[maxBuf];
byte manch[maxBuf/2];
byte buffer[maxBuf];
byte dataRFID[12];

int index = 0;
int startIndex = 0;
int stopIndex = 0;

uint64_t numberID = 0;
uint64_t lastID = 0;

static unsigned long TimeOut = 0;
uint64_t TimeOutCard = 0;

boolean bSendToSerial = false;
boolean bStopTimer = false;
boolean errFlag = false;
boolean bReadCard = false;

String  strRFID = "";
String  strToBLT = "";
String  strFromBLT = "";

const uint32_t  PAUSE = 800;

#define blLED 3
#define grnLED 4
#define sign 2

void setup() {
  Serial.begin(9600);
  pinMode(inPin,INPUT_PULLUP);
  pinMode(grnLED,OUTPUT);
  pinMode(blLED,OUTPUT);
  pinMode(sign,OUTPUT);  
  digitalWrite(grnLED,HIGH);
  digitalWrite(blLED,LOW);
  digitalWrite(sign,LOW);
  Serial.println("RFID");
  TimeOut = millis();
  TimeOutCard = 0;
  tone(Sound,3100,200);
  delay(250);
  tone(Sound,3500,200);
  delay(250);
  tone(Sound,3900,200);
  delay(250);
  tone(Sound,4300,200);
  delay(250);
  tone(Sound,3900,200);
  delay(250);
  tone(Sound,3500,200);
  delay(250);
  tone(Sound,3100,200);
  delay(250);
  Timer1.initialize(8);
  Timer1.pwm(9,512);
  Timer1.attachInterrupt(callback);
}

void callback() {
  val = digitalRead(inPin);
  if(val) { 
    if (val != curState) {
      curState = val;
      if (index > 0 && index < maxBuf) {
        raw[index] = bitTimeL;
        index++;
        bitTimeL = 1;
        bitTimeH = 1;
      }
    } else {
      bitTimeH++;
    }
  } else {
    if (val != curState) {
      curState = val;
      if (index < maxBuf) {
        raw[index] = bitTimeH;
        index++;
        bitTimeH = 1;
        bitTimeL = 1;
      }
    } else {
      bitTimeL++;
    }
  }
}

void loop() {
  if (index >= maxBuf) {
    /*  
    if (debug) {
      bool blow = false;
      for (int i = 0; i < maxBuf; i++) {
        Serial.print(raw[i]);
        if (!blow) {
          Serial.print("-");
          blow = true;
        } else {
          Serial.print("//");
          blow = false;
        }
      }
      Serial.println("///raw data");
      Serial.println();
    }
    */
    ConvertBuffer();
    /*
    if (debug) {
      for (int i = 0; i <= stopIndex; i++) {
        Serial.print(manch[i]);
        Serial.print("/");
      }
      Serial.println();
      Serial.println("Stop index - " + String(stopIndex));
      Serial.println();
    }
    */
    if (ManchsterDecoder()) {
    /*
      if (debug){
        for (int i = 0; i < 55; i++){
          Serial.print((int)manch[i]);
          Serial.print("/");
        }
        Serial.println();
        Serial.println();
      }
      */
      ConvertToHex();
      if (errFlag == false) {
        bReadCard = true;
        TimeOutCard = 0;
        if (numberID != lastID) {
          Timer1.stop();
          bStopTimer = true;
          TimeOut = millis();
          lastID = numberID;
          //Serial.print("NUMBER ID - ");
          //Serial.println((unsigned long)numberID);
          //strToBLT += '>';
          strToBLT += (unsigned long)numberID;
          //strToBLT += '<';
          bSendToSerial = true;
          digitalWrite(blLED,HIGH);
          digitalWrite(sign,HIGH);
          tone(Sound,3900,60);
          delay(60);
        }
      }
    }
    bitTimeH = 0;
    bitTimeL = 0;
    curState = 0;
    index = 0;
  }

  if (bSendToSerial) {
    bSendToSerial = false;
    Serial.println(strToBLT);
    strToBLT = "";
  }

  if (bStopTimer) {
    if ((millis() - TimeOut) > PAUSE) {
      bStopTimer = false;
      Timer1.initialize(8);
      Timer1.pwm(9,512);
      digitalWrite(blLED,LOW);
      digitalWrite(sign,LOW);
    }
  }

  if (bReadCard && !bStopTimer) {
    if (TimeOutCard < 200) {
      TimeOutCard++;
    } else {
      lastID = 0;
      bReadCard = false;
    }
  }
  vCheckSerial();
}

void ConvertBuffer(void) {
  uint16_t count = 0;
  for (int i = 0; i < maxBuf; i += 2) {
    if ((raw[i] >= 15 && raw[i] <= 32) && (raw[i + 1] >= 15 && raw[i + 1] <= 32)) {
      manch[count] = 1;
    }
    else if ((raw[i] >= 38 && raw[i] <= 56) && (raw[i + 1] >= 15 && raw[i + 1] <= 32)) {
      manch[count] = 2;
    }
    else if ((raw[i] >= 15 && raw[i] <= 32) && (raw[i + 1] >= 38 && raw[i + 1] <= 56)) {
      manch[count] = 3;
    }
    else if ((raw[i] >= 38 && raw[i] <= 56) && (raw[i + 1] >= 38 && raw[i + 1] <= 56)) {
      manch[count] = 4;
    } else {
      manch[count] = 101; // error code
    }
    count++;
  }
  int j = 0;
  for (int i = 0; i < count; i++) {
    if (manch[i] != 101) {
      manch[j] = manch[i];
      j = j + 1;
    } else {
      j = 0;
    }
  }
  stopIndex = j;
}

boolean ManchsterDecoder(void) {
  int j = 0;
  bool bStart = 0;
  bool bHalf = 0;
  for (int i = 0; i < stopIndex; i++) {
    if ((manch[i] == 3 || manch[i] == 4) && manch[i + 1] == 1 && manch[i + 2] == 1 && manch[i + 3] == 1 && manch[i + 4] == 1 && manch[i + 5] == 1 && manch[i + 6] == 1 && manch[i + 7] == 1 && manch[i + 8] == 1 && bStart == 0)
    {
      bStart = 1;
      bHalf = 0;
      i += 9;
      j = 0;
    }
    if (bStart) {
      if (manch[i] == 1) {
        if (bHalf) {
          buffer[j] = 0;
        } else {
          buffer[j] = 1;
        }
        if (j<maxBuf) {
          j++;
        }
      } else if (manch[i] == 2) {
        if (bHalf) {
          buffer[j] = 1;
          bHalf = 0;
        } else {
          buffer[j] = 1;
          j++;
          buffer[j] = 0;
          bHalf = 1;
        }
        if (j<maxBuf) {
          j++;
        }
      } else if (manch[i] == 3) {
        if (bHalf) {
          buffer[j] = 0;
          bHalf = 0;
        } else {
          buffer[j] = 0;
          j++;
          buffer[j] = 1;
          bHalf = 1;
        }
        if (j<maxBuf) {
          j++;
        }
      }
      else if (manch[i] == 4) {
        if (bHalf) {
          buffer[j] = 0;
          j++;
          buffer[j] = 1;
        } else {
          buffer[j] = 1;
          j++;
          buffer[j] = 0;
        }
        if (j<maxBuf) {
          j++;
        }
      }
    }
  }
  if (j > 55 && buffer[0] == 1) {
    byte count = 0;
    for (int i = 1; i < j; i++) {
      if (count < 55) {
        manch[count] = buffer[i];
        count++;
      } else {
        break;
      }
    }
    return true;
  } else {
    return false;
  }
}

void ConvertToHex(void) {
  for (int i = 0; i < 12; i++) {
    dataRFID[i] = 0;
  }
  numberID = 0;
  int j = 0;
  int k = 0;
  bool parityCheck = 0;
  errFlag = false;
  for (int i = 0; i < 55; i++) {
    int v = manch[i];
    if (j == 4) {
      k++;
      if (parityCheck != v) {
        if (k < 11) {
          errFlag = true;
        }
      }
      parityCheck = 0;
    } else {
      if (v == 1) {
        dataRFID[k] = dataRFID[k] << 1;
        dataRFID[k] += 1;
      } else {
        dataRFID[k] = dataRFID[k] << 1;
      }
      parityCheck = parityCheck ^ v;
    }
    if (j == 4) {
      j = 0;
    } else {
      j++;
    }
  }
  if (errFlag == 0) {
    uint64_t val = 0;
    for (int i = 0; i < 8; i++) {
      val = dataRFID[i + 2];
      int j = 7 - i;
      while (j > 0) {
        val = val << 4;
        j--;
      }
      numberID += val;
    }
  }
}

void vCheckSerial() {
  char inChar;
  if (Serial.available() > 0) {
    inChar = Serial.read();
    if (inChar == '!') {
      Timer1.stop();
      bStopTimer = true;
      TimeOut = millis();
      strToBLT = "RFID";
      bSendToSerial = true;
    }
  }
}
