#pragma once
#include "Faaf.h"

static const char* TAG = "FAAF";

const size_t PACKET_SIZE = 6;
const unsigned long PACKET_TIMEOUT = 30;

Stream* Faaf::_serial = nullptr;
FaafCallback Faaf::_onCoordinates = nullptr;
boolean Faaf::_isPressed = false;

void Faaf::begin(Stream* serial, FaafCallback onCoordinates) {
  _serial = serial;
  _onCoordinates = onCoordinates;

  xTaskCreatePinnedToCore(
    Faaf::_taskServer,
    "Faaf",
    2048,
    NULL,
    0,
    NULL,
    1
  );
}

void Faaf::_taskServer(void* pvParameters) {
  while (true) {
    _perform();
  }
}

void Faaf::_perform() {
  byte packet[PACKET_SIZE];
  size_t recvd = 0;
  unsigned long receivingSince = millis();
  unsigned int receivedX;
  unsigned int receivedY;
  unsigned int targetX;
  unsigned int targetY;
  FaafCoordinates coordinates;

  ESP_LOGD(TAG, "tick");

  do {
    if (_serial->available() <= 0) {
      ESP_LOGV(TAG, "no data");
      continue;
    }

    byte data = _serial->read();

    if ((recvd == 0 && data != 0xa1) || (recvd == 1 && data != 0x00)) {
      ESP_LOGE(TAG, "corrupted data %i 0x%04X", recvd, data);
      recvd = 0;
      memset(packet, 0, sizeof(packet));
      continue;
    }

    ESP_LOGV(TAG, "received data %i 0x%04X", recvd, data);

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

    coordinates = FaafCoordinates{
      true,
      targetX,
      targetY
    };

    ESP_LOGV(TAG, "coordinates %i %i", targetX, targetY);
    _onCoordinates(coordinates);
    _isPressed = true;
  } else if (_isPressed) {
    coordinates = FaafCoordinates{
      false,
      0,
      0
    };

    ESP_LOGV(TAG, "release");
    _onCoordinates(coordinates);
    _isPressed = false;
  } else {
    ESP_LOGV(TAG, "idle");
  }

  vTaskDelay(10 / portTICK_PERIOD_MS);
}
