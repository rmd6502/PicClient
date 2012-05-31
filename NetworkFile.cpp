#include <Ethernet.h>
#include "NetworkFile.h"
#include "NetUtil.h"

void NetworkFile::seek(uint32_t new_offset) {
  if (offset >= new_offset) {
    // Can't seek backwards, silently fail
    return;
  }
  //Serial.print("offset "); Serial.print(offset); Serial.print(" new_offset "); Serial.println(new_offset);
  while (offset < new_offset) read();
}

uint16_t NetworkFile::read(void *buf, uint16_t count) {
  uint16_t bufCount = 0;
  if (client == NULL) { return 0; }
  bufCount = cRead(client, buf, count);
  offset += bufCount;
  return bufCount;
}

NetworkFile &NetworkFile::open(const char *server, const char *fileName, const uint16_t port) {
  static NetworkFile instance;
  
  if (instance) {
    instance.close();
  }
  static EthernetClient myClient;
  
  instance.client = &myClient; instance.offset = 0;
  if (myClient.connect(server, port)) {
    //Serial.print("connected, filename "); Serial.println(fileName);
    myClient.print("GET "); myClient.print(fileName); myClient.println(" HTTP/1.0");
    myClient.println();
    //Serial.println("check header");
    uint16_t len;
    uint16_t ret = checkHeader(myClient, &len);
    if (ret != 200) {
      Serial.print("fail, ret "); Serial.println(ret);
      instance.close();
    }
    instance.offset = 0;
    //Serial.print("return from check header ");Serial.println(len);
  } else {
    Serial.println("open failed");
    // indicate we're not really open
    instance.close();
  }
  return instance;
}



