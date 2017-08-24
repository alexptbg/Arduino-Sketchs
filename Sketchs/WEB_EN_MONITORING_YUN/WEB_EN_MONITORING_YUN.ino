#include <TimerOne.h>
#include <SoftwareSerial.h>
#include <Bridge.h>
#include <Console.h>
#include <HttpClient.h>
#include <Process.h>
#include <YunClient.h>

IPAddress server(192,168,0,210);

YunClient client;

#define rrx 9                     //define what pin rx is going to be.
#define ttx 7                     //define what pin Tx is going to be.

SoftwareSerial RS485(rrx, ttx);

uint8_t const    SCAN_START_ADR = 1;
uint8_t const   SCAN_END_ADR = 6;

uint8_t ActivDevAdr[SCAN_END_ADR] = { 0, };
String  ActivDevType[SCAN_END_ADR] = { "", };
uint8_t NumbersSubDev[SCAN_END_ADR] = { 0, };

uint8_t CountSubDev = 0;
uint8_t CountUnoDev = 0;
uint8_t CountStek = 0;
uint8_t CountPause = 0;
uint8_t TotalDev = 0;

boolean bInitUnoDev = 0;
boolean bResponce = 0;
boolean bUnoResponce = 0;
boolean bSendInfo = 0;
boolean bNumbersSD = 0;
boolean bPause = 0;
boolean bScanDedices = 0;

uint16_t  CountResponce = 0;
uint16_t  CountRestart = 0;

String    strFromRS485 = "";
String    strFromUno = "";

void setup()
{
  Bridge.begin();
  Serial.begin(9600);
  delay(1000);
  Serial.println("...YUN");
  delay(1000);
  RS485.begin(4800);
  delay(1000);
  Timer1.initialize(100000); // set a timer of length 100000 microseconds (or 0.1 sec - or 10Hz => the led will blink 5 times, 5 cycles of on-and-off, per second)
  Timer1.attachInterrupt(Transfer); // attach the service routine here
}

void loop()
{
  CheckRS485Port();

  if (bUnoResponce)
  {
    bUnoResponce = 0;
    strFromUno = "";
    boolean start = 0;
    for (uint8_t i = 0; i < strFromRS485.length(); i++)
    {
      if (strFromRS485[i] == '>')
        start = 1;
      if (start)
        strFromUno += strFromRS485[i];
      if (strFromRS485[i] == '<')
        break;
    }
    CheckUnoResponce(strFromUno);
  }

  if (bSendInfo)
  {
    bPause = 1;
    bSendInfo = 0;
    InfoToSerial();
  }

  if (CountRestart >= 200)
  {
    for (uint8_t i = 0; i < SCAN_END_ADR; i++)
    {
      ActivDevAdr[i] = 0;
      NumbersSubDev[i] = 0;
      ActivDevType[i] = "";
    }
    CountStek = 0;
    TotalDev = 0;
    CountRestart = 0;
    bInitUnoDev = true;
    bResponce = 0;
    bNumbersSD = 0;
    bScanDedices = 0;
  }
}

void Transfer(void)
{
  CountRestart++;
  if (!bPause)
  {
    if (bInitUnoDev && !bResponce && !bNumbersSD && !bScanDedices)
    {
      if (CountUnoDev == 0)
        CountUnoDev = SCAN_START_ADR;
      if (CountUnoDev > SCAN_END_ADR)
      {
        bInitUnoDev = 0;
        CountUnoDev = 0;
        bSendInfo = 1;
        if (TotalDev != 0)
        {
          bScanDedices = 1;
          CountUnoDev = 0;
          CountSubDev = 0;
          bPause = 1;
        }
        return;
      }
      String sendRS485 = ">INIT" + IdToString(CountUnoDev) + "<";
      CountUnoDev++;
      bResponce = 1;
      CountResponce = 0;
      RS485.println(sendRS485);
      CountRestart = 0;
    }

    if (bScanDedices && !bResponce)
    {
      /* SEND Query To Device */
      /*****************************************************************************************************************************************/
        RS485.println(">" + IdToString(ActivDevAdr[CountUnoDev]) + "," + IdToString(CountSubDev + 1) + ActivDevType[CountUnoDev] + "<");   
      /*****************************************************************************************************************************************/

      CountSubDev++;
      if (CountSubDev >= NumbersSubDev[CountUnoDev])
      {
        CountSubDev = 0;
        CountUnoDev++;
        if (CountUnoDev >= TotalDev)
        {
          CountUnoDev = 0;
          CountSubDev = 0;
        }
      }
      bResponce = 1;
      CountResponce = 0;

    }


    if (bResponce)
    {
      if (CountResponce >= 50)
      {
        bResponce = 0;
        CountResponce = 0;
      }
      else
      {
        CountResponce++;
      }
    }

  }
  else
  {
    if (CountPause >= 100)
    {
      bPause = 0;
      CountPause = 0;
    }
    else
    {
      CountPause++;
    }
  }
}

void CheckRS485Port(void)
{
    // while there is data coming in, read it
  while (RS485.available() > 0)
  {
    char inByte = RS485.read();
    CountResponce = 0;
    if (inByte == '#' || inByte == '>')
    {
      if (inByte == '#')
      {
        strFromRS485 = "#";
        CountRestart = 0;
      }
      else
      {
        strFromRS485 = ">";
        CountRestart = 0;
      }
    }
    else
    {
      strFromRS485 += inByte;
    }
    if (inByte == '<')
    {
      bUnoResponce = 1;
    }
  }
}

String IdToString(uint8_t bAdr)
{
  if (bAdr > 9)
  {
    return String(bAdr);
  }
  else
  {
    return "0" + String(bAdr);
  }
}

uint8_t IdFromString(String str, uint8_t index)
{
  char  ch = ' ';
  uint8_t bt = 0;
  uint8_t ival = 0;

  for (byte i = 0; i < 2; i++)
  {
    ch = str[i + index];
    bt = ch;

    if ((ch >= 48 && ch <= 57) || (ch >= 65 && ch <= 70))
    {
      ival = ival * 16;
      if (bt < 65)
      {
        bt = bt - 48;
      }
      else
      {
        bt = bt - 55;
      }
      ival = ival + bt;
    }
    else
    {
      ival = 0;
      return ival;
    }
  }
  return ival;
}

void CheckUnoResponce(String uno)
{
  if (uno[0] == '>' && uno[1] == 'A')
  {
    SendToServer();
    bPause = 1;
  }
  else
  {
    if (!bNumbersSD)
    {
      uint8_t id = IdFromString(uno, 1);
      if (id >= 1 && id <= 20)
      {
        ActivDevAdr[TotalDev] = id;
        String TypeDev = "";
        for (uint8_t i = 3; i < uno.length(); i++)
        {
          if (uno[i] == '<')
            break;
          else
            TypeDev += uno[i];
        }
        if (TypeDev == "ELMETER" || TypeDev == "WTRMETER")
        {
          ActivDevType[TotalDev] = TypeDev;
          TotalDev++;
          bNumbersSD = 1;
          bPause = 1;
          //RS485.println(String(ActivDevAdr[TotalDev - 1]));
          //RS485.println(ActivDevType[TotalDev - 1]);
        }
      }
    }
    else
    {
      uint8_t id = IdFromString(uno, 1);
      if (id >= 1 && id <= 20)
      {
        String TypeDev = "";
        for (uint8_t i = 3; i < uno.length(); i++)
        {
          if (uno[i] == '<')
            break;
          else
            TypeDev += uno[i];
        }
        if (TypeDev == "DEVICES")
        {
          NumbersSubDev[TotalDev - 1] = id;
          bNumbersSD = 0;
          bResponce = 0;
          bPause = 1;
          //RS485.println(String(NumbersSubDev[TotalDev - 1]));
          //RS485.println(ActivDevType[TotalDev - 1]);
        }
        else
        {
          TotalDev--;
          bNumbersSD = 0;
          bResponce = 0;
          RS485.println("Not Sub Devices");
        }
        NumbersSubDev[TotalDev - 1] = id;
      }
    }
  }
}

void InfoToSerial()
{
  for (uint8_t i = 0; i < SCAN_END_ADR; i++)
  {
    RS485.println();
    RS485.println(String(ActivDevAdr[i]) + "TYPE - " + ActivDevType[i] + "  Devices >>> " + String(NumbersSubDev[i]));
  }
}

void SendToServer(void)
{
  String  parametri = "";
  uint8_t ID = IdFromString(strFromUno, 2);

  if (client.connect(server, 80))
  {
    parametri += "?in=1&inv=" + String(ID);
    for (uint8_t i = 4; i < strFromUno.length() - 1; i++)
    {
        parametri += strFromUno[i];
    }

    //parametri = "?in=1&inv=96&val0=69&val1=69&val2=96&val3=21&val4=100&val5=200&val6=300";

    client.print("GET /ems/ar_elmeter.php");
    client.println(parametri);
    client.println(" HTTP/1.1");
    client.println("Host: 192.168.0.210");
    client.print("Content-length:");
    client.println(parametri.length());
    client.println(parametri);
    client.println("Connection: Close");
    client.println("Content-Type: application/x-www-form-urlencoded;");
    client.println();
    //Serial.println(parametri);
  }
  else {
    //Serial.println("connection failed");
  }
  if (client.connected()){
    client.stop();
  }
}

