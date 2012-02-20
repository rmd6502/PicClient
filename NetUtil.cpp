#include <stdint.h>
#include <WiFly.h>
#include "NetUtil.h"

extern uint16_t __bss_end;

uint16_t checkHeader(Client &client, uint16_t *contentLength) {
  char buf[70];
  uint8_t p = 0;
  int c = 0;
  bool done = false;  // when we read the blank line or error
  uint16_t ret = 401;  // not found
  
  //Serial.print("__bss_end 0x");Serial.println(__bss_end,HEX);
  if (client == NULL) {
    Serial.println("Client is NULL!!"); return ret;
  }
  // read until \r\n\r\n then check header values
  while (!done) {
    // read a single line up to a newline \n
    p = cRead(&client, buf, sizeof(buf), '\n');
    // Serial.print("read "); Serial.println(p);
    // process the data in the line
    if (p && buf[p-1] == '\n')  {  // did we succeed in reading a line?
      buf[p] = 0;  // end the string
      //Serial.println(buf);
      ++c;
      if (c >= 20) {
        client.flush();
        return 402;
      }
      //Serial.print("read line of length "); Serial.print(p); Serial.println(": ");
      //Serial.println(buf);
      char *h;
      if ((h = strstr(buf, "HTTP")) != NULL) {
        char *q = strchr(h, ' ') ;
        if (q != NULL) {
          ret = atoi(q+1);
          //Serial.print("status "); Serial.println(buf);
          if (ret != 200) {
            client.flush();
            return ret;
          }
        }
      } else if (contentLength != NULL && !strncasecmp_P(buf, PSTR("Content-Length:"), 15)) {
        char *q = strchr(buf, ' ') ;
        if (q != NULL) {
          *contentLength = atoi(q+1);
        }
      } else if (p == 2) {  // blank line, done with header
        done = true;
      }
    } else {
      Serial.println("failed");
      // somthing went wrong, didn't read a full line
      ret = 403;  // indicate we're not successful
      client.flush();
      done = true;
    }
  }
  //Serial.print("status "); Serial.println(ret);
  return ret;
}

uint16_t cRead(Client *client, void *buf, uint16_t count, char terminator) {
  uint16_t bufPtr = 0;
  char b;
  
  while (bufPtr < count) {
    if (!client->connected()) {
      break;
    }
    while (client->connected() && !client->available()) ;   // wait
    if (!client->connected()) { client = NULL; break; }
    b = client->read();
    ((uint8_t *)buf)[bufPtr++] = b;
    if (terminator && b == terminator) break;
  }
  return bufPtr;
}

extern "C" void atexit(void) {}
