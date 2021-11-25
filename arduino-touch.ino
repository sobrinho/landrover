// Original: https://github.com/sobrinho/arduino-touch
#include <Touch.h>

const size_t PACKET_SIZE = 6;
const unsigned int FINGER_IDENTIFIER = 0;

byte packet[PACKET_SIZE];
unsigned int targetX = 0;
unsigned int targetY = 0;
bool isPressed = false;

void setup () {
  setupSerial1();
  setupHID();
}

void setupSerial1 () {
  Serial1.begin(19200);

  while (!Serial1) {
    ;
  }
}

void setupHID () {
  Touch.begin();
}

void loop () {
  if (recvCoordinates()) {
    parseCoordinates();
    moveToCoordinates();
  } else {
    releaseFinger();
  }
}

bool recvCoordinates () {
  size_t recvd = 0;

  unsigned long startMillis = millis();
  unsigned long timeout = 30;

  do {
    if (Serial1.available() == 0) {
      continue;
    }

    byte data = Serial1.read();

    if ((recvd == 0 && data != 0xa1) || (recvd == 1 && data != 0x00)) {
      recvd = 0;
      continue;
    }

    packet[recvd++] = data;

    if (recvd == PACKET_SIZE) {
      return true;
    }
  } while (millis() - startMillis < timeout);

  return false;
}

void parseCoordinates () {
  targetX = word(packet[2], packet[3]);
  targetY = word(packet[4], packet[5]);

  targetX = map(targetX, 0, 65535, 0, 10000);
  targetY = map(targetY, 65535, 0, 0, 10000);
}

void moveToCoordinates () {
  Touch.moveFingerTo(FINGER_IDENTIFIER, targetX, targetY);
  isPressed = true;
}

void releaseFinger () {
  if (!isPressed) {
    return;
  }

  Touch.releaseFinger(FINGER_IDENTIFIER);
  isPressed = false;
}
