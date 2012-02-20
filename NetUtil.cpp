#include <stdint.h>
#include <WiFly.h>
#include "NetUtil.h"

uint16_t checkHeader(Client &client, uint16_t *contentLength) {
  char buf[80];
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
    while (client.connected() && p < sizeof(buf) && b != '\n') {
      while (client.connected() && !client.available()) ;  // wait
      if (!client.connected()) break;  // nothing more to do
      b = client.read();
      buf[p++] = b;
    }
    // process the data in the line
    if (b == '\n')  {  // did we succeed in reading a line?
      buf[p] = 0;  // end the string
      ++c;
      if (c >= 20) {
        return 404;
      }
      //Serial.print("read line of length "); Serial.print(p); Serial.println(": ");
      //Serial.println(buf);
      if (!strncmp(buf, "HTTP", 4)) {
        char *q = strchr(buf, ' ') ;
        if (q != NULL) {
          ret = atoi(q+1);
          if (ret != 200) {
            return ret;
          }
        }
      } else if (contentLength != NULL && !strncasecmp(buf, "Content-Length:", 15)) {
        char *q = strchr(buf, ' ') ;
        if (q != NULL) {
          *contentLength = atoi(q+1);
        }
      } else if (p == 2) {  // blank line, done with header
        done = true;
      }
      p = 0; b=0;
    } else {
      Serial.println("failed");
      // somthing went wrong, didn't read a full line
      ret = 404;  // indicate we're not successful
      client.flush();
      done = true;
    }
  }
  Serial.print("status "); Serial.println(ret);
  return ret;
}

uint16_t cRead(Client *client, void *buf, uint16_t count) {
  uint16_t bufPtr = 0;
  
  while (bufPtr < count) {
    if (!client->connected()) {
      break;
    }
    while (client->connected() && !client->available()) ;   // wait
    if (!client->connected()) { client = NULL; break; }
    ((uint8_t *)buf)[bufPtr++] = client->read();
  }
  return bufPtr;
}
