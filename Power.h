#pragma once
#include <Arduino.h>

enum PowerState {
  powerStateOff,
  powerStateBooting,
  powerStateOn,
  powerStateShuttingDown
};

class Power {
  public:
    static void begin(byte pinIgnition, byte pinRelay, byte pinPower, byte pinFeedback);

  private:
    static byte _pinIgnition;
    static byte _pinRelay;
    static byte _pinPower;
    static byte _pinFeedback;
    static PowerState _powerState;
    volatile static boolean _ignitionState;
    volatile static boolean _feedbackState;

    static void _readIgnition();
    static void _readFeedback();
    static void _taskServer(void* pvParameters);
    static void _perform();
};
