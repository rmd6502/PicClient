#include <stdint.h>

class Client;
class NetworkFile {
  protected:
    uint32_t offset;
    Client *client;
    
  public:
    NetworkFile() { offset = 0; client = NULL; }
    void seek(uint32_t offset);
    uint16_t read(void *buf, uint16_t count);
    uint8_t read() { uint8_t ret; read(&ret, 1); return ret; }
    static NetworkFile &open(const char *server, const char *fileName, const uint16_t port = 80);
    operator bool() { return client != NULL; }
    uint32_t getOffset() const { return offset; }
    void close() { client->stop(); client = NULL; }
};
