#include <SPI.h>
#include <Ethernet.h>

byte mac[] = {0x90,0xA2,0xDA,0x0D,0x6A,0xB3};  
byte ip[] = {192,168,2,99};    
byte gateway[] = {192,168,2,1};
byte subnet[] = {255,255,254,0};

EthernetServer server = EthernetServer(2202);

void setup()
{
  Ethernet.begin(mac,ip,gateway,subnet);
  server.begin();
}

void loop()
{
  EthernetClient client = server.available();
  if (client) {
    while (client.connected()) {
      if (client.available()) {
        client.write(">00 01 02 03 04 05 06 07 08 09 00 01 02 03 04 05 06 07 08 09 00 01 02 03 04 05 06 07 08 09 00 01 02 03 04 05 06 07 08 09 00 01 02 03 04 05 06 07 08 09<");
        delay(1000);
      }
    }
  }
}
//server.write(">00 01 02 03 04 05 06 07 08 09 00 01 02 03 04 05 06 07 08 09 00 01 02 03 04 05 06 07 08 09 00 01 02 03 04 05 06 07 08 09 00 01 02 03 04 05 06 07 08 09<");
