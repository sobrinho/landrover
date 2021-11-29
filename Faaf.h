#pragma once
#include <Arduino.h>
#include <functional>

typedef std::function<void(boolean pressed, unsigned int targetX, unsigned int targetY)> FaafCallback;

class Faaf {
  public:
    Faaf(HardwareSerial *serial, FaafCallback onCoordinates);
    void begin();
    void perform();

  private:
    HardwareSerial *serial;
    FaafCallback onCoordinates;
};
