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
    Psu(byte pinIgnition, byte pinRelay, byte pinPower, byte pinFeedback);
    void begin();
    void loop();

  private:
    byte pinIgnition;
    byte pinRelay;
    byte pinPower;
    byte pinFeedback;
    PsuState psuState;
    volatile boolean ignitionState;
    volatile boolean feedbackState;

    void perform();
    void readIgnition();
    void readFeedback();

    static void readIgnitionISR();
    static void readFeedbackISR();
};
