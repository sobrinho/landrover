#include "Psu.h"

class Psu {
  private:
    byte pinIgnition;
    byte pinRelay;
    byte pinPower;
    byte pinFeedback;
    volatile bool ignitionSensor;
    volatile bool feedbackSensor;

    void readIgnition() {
      this->ignitionSensor = digitalRead(this->pinIgnition) == LOW;
    }

    void readFeedback() {
      this->feedbackSensor = digitalRead(this->pinFeedback) == LOW;
    }

  public:
    PsuState currentState;

    void Psu(byte pinIgnition, byte pinRelay, byte pinPower, byte pinFeedback) {
      this->pinIgnition = pinIgnition;
      this->pinRelay = pinRelay;
      this->pinPower = pinPower;
      this->pinFeedback = pinFeedback;
      this->ignitionSensor = false;
      this->feedbackSensor = false;
      this->currentState = psuStateOff;
    }

    void setup() {
      pinMode(this->pinIgnition, INPUT_PULLUP);
      pinMode(this->pinRelay, OUTPUT);
      pinMode(this->pinPower, OUTPUT);
      pinMode(this->pinFeedback, INPUT_PULLUP);

      attachInterrupt(digitalPinToInterrupt(this->pinIgnition), this.readIgnition, CHANGE);
      attachInterrupt(digitalPinToInterrupt(this->pinFeedback), this.readFeedback, CHANGE);

      this->readIgnition();
      this->readFeedback();
    }

    void loop() {
      unsigned long bootingSince;
      unsigned long ignitionOffSince;
      unsigned long shuttingDownSince;

      switch (this->currentState) {
        case psuStateOff:
          if (this->ignitionSensor) {
            this->currentState = psuStateBooting;
          }

          break;

        case psuStateBooting:
          digitalWrite(this->pinRelay, HIGH);
          digitalWrite(this->pinPower, HIGH);

          bootingSince = millis();

          do {
            if (this->feedbackSensor) {
              this->currentState = psuStateOn;
              break;
            }
          } while (millis() - bootingSince < BOOT_TIMEOUT);

          if (this->currentState == psuStateOn) {
            break;
          }

          digitalWrite(this->pinRelay, LOW);
          digitalWrite(this->pinPower, LOW);

          this->currentState = psuStateOff;

          break;

        case psuStateOn:
          if (this->ignitionSensor) {
            break;
          }

          ignitionOffSince = millis();

          do {
            if (this->ignitionSensor) {
              break;
            }
          } while (millis() - ignitionOffSince < SHUT_DOWN_DELAY);

          if (!this->ignitionSensor) {
            this->currentState = psuStateShuttingDown;
          }

          break;

        case psuStateShuttingDown:
          digitalWrite(this->pinPower, LOW);

          shuttingDownSince = millis();

          do {
            if (!this->feedbackSensor) {
              break;
            }
          } while (millis() - shuttingDownSince < SHUT_DOWN_TIMEOUT);

          digitalWrite(this->pinRelay, LOW);
          this->currentState = psuStateOff;

          break;
      }
    }
}
