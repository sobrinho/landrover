#include <BleAbsMouse.h>

BleAbsMouse bleAbsMouse;

const size_t PACKET_SIZE = 6;

byte packet[PACKET_SIZE];

bool isPressed = false;

unsigned int targetX = 0;
unsigned int targetY = 0;

void setup () {
  setupHID();
  setupSerial2();
}

void setupHID () {
  bleAbsMouse.begin();
}

void setupSerial2 () {
  Serial2.begin(19200);
}

void loop () {
  if (!bleAbsMouse.isConnected()) {
    return;
  }

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
    if (Serial2.available() == 0) {
      continue;
    }

    byte data = Serial2.read();

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
  word receivedX = word(packet[2], packet[3]);
  word receivedY = word(packet[4], packet[5]);

  targetX = map(receivedX, 0, 65535, 0, 10000);
  targetY = map(receivedY, 65535, 0, 0, 10000);
}

void moveToCoordinates () {
  bleAbsMouse.move(targetX, targetY);
  isPressed = true;
}

void releaseFinger () {
  if (!isPressed) {
    return;
  }

  bleAbsMouse.release();
  isPressed = false;
}
