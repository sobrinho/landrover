#ifndef PSU_H
#define PSU_H

enum PsuState = {
  psuStateOff,
  psuStateBooting,
  psuStateOn,
  psuStateShuttingDown
}

class Psu {
  private:
    byte pinIgnition;
    byte pinRelay;
    byte pinPower;
    byte pinFeedback;

    PsuState currentState;

    void readIgnition();
    void readFeedback();

  public:
    void Psu(byte pinIgnition, byte pinRelay, byte pinPower, byte pinFeedback);
    void begin();
    void loop();
}

#endif
