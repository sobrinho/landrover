#ifndef PSU_H_
#define PSU_H_

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
    volatile bool ignitionSensor;
    volatile bool feedbackSensor;

    void readIgnition();
    void readFeedback();

  public:
    PsuState currentState;

    void Psu(byte pinIgnition, byte pinRelay, byte pinPower, byte pinFeedback);
    void setup();
    void loop();
}

#endif
