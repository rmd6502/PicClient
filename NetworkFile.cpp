#include <WiFly.h>
#include "NetworkFile.h"

void NetworkFile::seek(uint32_t new_offset) {
  if (offset >= new_offset) {
    // Can't seek backwards, silently fail
    return;
  }
  Serial.print("offset "); Serial.print(offset); Serial.print(" new_offset "); Serial.println(new_offset);
  while (offset < new_offset) read();
}

uint16_t NetworkFile::read(void *buf, uint16_t count) {
  uint32_t bufPtr = 0;
  if (client == NULL) { return 0; }
  while (bufPtr < count) {
    if (!client->connected()) {
      client = NULL;
      break;
    }
    while (client->connected() && !client->available()) ;   // wait
    if (!client->connected()) { client = NULL; break; }
    ((uint8_t *)buf)[bufPtr] = client->read();
    bufPtr = bufPtr + 1;
    offset = offset + 1;
  }
  return bufPtr;
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
    if (instance.checkHeader() != 200) {
      instance.client = NULL;
    }
    Serial.println("return from check header");
  } else {
    Serial.println("connection failed");
    // indicate we're not really open
    instance.client = NULL;
  }
  return instance;
}

uint16_t NetworkFile::checkHeader() {
  char buf[64];
  uint8_t p = 0;
  char b = 0;
  int c = 0;
  bool done = false;  // when we read the blank line or error
  uint16_t ret = 404;  // not found
  
  if (client == NULL) {
    Serial.println("Client is NULL!!"); return ret;
  }
  // read until \r\n\r\n then check header values
  while (!done) {
    // read a single line up to a newline \n
    while (client->connected() && b != '\n') {
      while (client->connected() && !client->available()) ;  // wait
      if (!client->connected()) break;  // nothing more to do
      b = client->read();
      buf[p++] = b;
    }
    // process the data in the line
    if (b == '\n')  {  // did we succeed in reading a line?
      buf[p] = 0;  // end the string
      ++c;
      if (c >= 20) {
        client = NULL;
        return 404;
      }
      Serial.print("read line of length "); Serial.print(p); Serial.println(": ");
      Serial.println(buf);
      if (!strncmp(buf, "HTTP", 4)) {
        char *q = strchr(buf, ' ') ;
        if (q != NULL) {
          ret = atoi(q+1);
          if (ret != 200) {
            return ret;
          }
        }
      } else if (p == 2) {  // blank line, done with header
        offset = 0;  // now we're at the beginning of the file content
        done = true;
      }
      p = 0; b=0;
    } else {
      Serial.println("failed");
      // somthing went wrong, didn't read a full line
      ret = 404;  // indicate we're not successful
      done = true;
    }
  }
  Serial.print("status "); Serial.println(ret);
  return ret;
}


