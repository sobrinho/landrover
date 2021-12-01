#pragma once
#include <Arduino.h>
#include <BleAbsMouse.h>

class Mouse {
  public:
    static void begin();
    static void move(unsigned int targetX, unsigned int targetY);
    static void release();

  private:
    static BleAbsMouse _bleAbsMouse;
};
