#pragma once
#include <BleAbsMouse.h>
#include "Mouse.h"

static const char* TAG = "MOUSE";

BleAbsMouse Mouse::_bleAbsMouse = BleAbsMouse("FT-VR-LR12", "Faaftech");

void Mouse::begin() {
  ESP_LOGD(TAG, "begin");
  _bleAbsMouse.begin();
}

void Mouse::move(unsigned int targetX, unsigned int targetY) {
  ESP_LOGD(TAG, "move %i %i", targetX, targetY);
  _bleAbsMouse.move(targetX, targetY);
}

void Mouse::release() {
  ESP_LOGD(TAG, "release");
  _bleAbsMouse.release();
}
