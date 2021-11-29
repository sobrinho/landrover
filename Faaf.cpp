#include "Faaf.h"

const size_t PACKET_SIZE = 6;
const unsigned long PACKET_TIMEOUT = 30;

class Faaf {
  private:
    Serial serial;
    onCoordinates faafCallback;

  public:
    void Faaf(Serial serial, faafCallback onCoordinates) {
      this->serial = serial;
      this->onCoordinates = onCoordinates;
    }

    void setup() {
      this->serial->begin(19200);
    }

    void loop() {
      byte packet[PACKET_SIZE];
      size_t recvd = 0;
      unsigned long receivingSince = millis();
      unsigned int receivedX;
      unsigned int receivedY;
      unsigned int targetX;
      unsigned int targetY;

      do {
        if (this->serial.available() <= 0) {
          continue;
        }

        byte data = this->serial.read();

        if ((recvd == 0 && data != 0xa1) || (recvd == 1 && data != 0x00)) {
          recvd = 0;
          continue;
        }

        packet[recvd++] = data;

        if (recvd == PACKET_SIZE) {
          break;
        }
      } while (millis() - receivingSince < PACKET_TIMEOUT);

      if (recvd == PACKET_SIZE) {
        receivedX = word(packet[2], packet[3]);
        receivedY = word(packet[4], packet[5]);

        targetX = map(receivedX, 0, 65535, 0, 10000);
        targetY = map(receivedY, 65535, 0, 0, 10000);

        this->onCoordinates(true, targetX, targetY);
      } else {
        this->onCoordinates(false, unsigned int 0, unsigned int 0);
      }
    }
};
