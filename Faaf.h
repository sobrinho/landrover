#pragma once
#include <Arduino.h>
#include <functional>

typedef struct {
  boolean isPressed;
  unsigned int targetX;
  unsigned int targetY;
} FaafCoordinates;

typedef std::function<void(FaafCoordinates coordinates)> FaafCallback;

class Faaf {
  public:
    Faaf(HardwareSerial* serial, FaafCallback onCoordinates);
    void begin();
    void loop();

  private:
    HardwareSerial* serial;
    FaafCallback onCoordinates;
    boolean isPressed;

    void perform();
};
