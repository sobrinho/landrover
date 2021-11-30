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
    static void begin(HardwareSerial* serial, FaafCallback onCoordinates);
    static void taskServer(void* pvParameters);

  private:
    static HardwareSerial* _serial;
    static FaafCallback _onCoordinates;
    static boolean _isPressed;

    static void perform();
};
