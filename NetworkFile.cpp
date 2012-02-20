#include <WiFly.h>
#include "NetworkFile.h"
#include "NetUtil.h"

void NetworkFile::seek(uint32_t new_offset) {
  if (offset >= new_offset) {
    // Can't seek backwards, silently fail
    return;
  }
  Serial.print("offset "); Serial.print(offset); Serial.print(" new_offset "); Serial.println(new_offset);
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
  static Client myClient(server, port);
  
  instance.client = &myClient; instance.offset = 0;
  if (myClient.connect()) {
    Serial.println("connected");
    myClient.print("GET "); myClient.print(fileName); myClient.println(" HTTP/1.0");
    myClient.println();
    Serial.println("about to check header");
    if (checkHeader(myClient) != 200) {
      myClient.stop();
      instance.client = NULL;
    }
    instance.offset = 0;
    Serial.println("return from check header");
  } else {
    Serial.println("connection failed");
    // indicate we're not really open
    myClient.stop();
    instance.client = NULL;
  }
  return instance;
}



