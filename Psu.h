#pragma once
#include <Arduino.h>

enum PsuState {
  psuStateOff,
  psuStateBooting,
  psuStateOn,
  psuStateShuttingDown
};

class Psu {
  public:
    static void begin(byte pinIgnition, byte pinRelay, byte pinPower, byte pinFeedback);
    static void taskServer(void* pvParameters);

  private:
    static byte _pinIgnition;
    static byte _pinRelay;
    static byte _pinPower;
    static byte _pinFeedback;
    volatile static boolean _ignitionState;
    volatile static boolean _feedbackState;
    volatile static PsuState _psuState;

    static void _perform();
    static void _readIgnition();
    static void _readFeedback();
};
