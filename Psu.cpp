#pragma once
#include "Psu.h"

#define SHUT_DOWN_TIMEOUT 60000
#define BOOT_TIMEOUT 60000
#define SHUT_DOWN_DELAY 60000

Psu::Psu(byte pinIgnition, byte pinRelay, byte pinPower, byte pinFeedback) {
  this->pinIgnition = pinIgnition;
  this->pinRelay = pinRelay;
  this->pinPower = pinPower;
  this->pinFeedback = pinFeedback;
  this->currentState = psuStateOff;
}

void Psu::begin() {
  pinMode(this->pinIgnition, INPUT_PULLUP);
  pinMode(this->pinRelay, OUTPUT);
  pinMode(this->pinPower, OUTPUT);
  pinMode(this->pinFeedback, INPUT_PULLUP);
}

void Psu::loop() {
  unsigned long bootingSince;
  unsigned long ignitionOffSince;
  unsigned long shuttingDownSince;

  switch (this->currentState) {
    case psuStateOff:
      if (this->readIgnition()) {
        this->currentState = psuStateBooting;
      }

      break;

    case psuStateBooting:
      digitalWrite(this->pinRelay, HIGH);
      digitalWrite(this->pinPower, HIGH);

      bootingSince = millis();

      do {
        if (this->readFeedback()) {
          this->currentState = psuStateOn;
          break;
        }
      } while (millis() - bootingSince < BOOT_TIMEOUT);

      if (this->currentState == psuStateOn) {
        break;
      }

      digitalWrite(this->pinRelay, LOW);
      digitalWrite(pinPower, LOW);

      this->currentState = psuStateOff;

      break;

    case psuStateOn:
      if (this->readIgnition()) {
        break;
      }

      ignitionOffSince = millis();

      do {
        if (this->readIgnition()) {
          break;
        }
      } while (millis() - ignitionOffSince < SHUT_DOWN_DELAY);

      if (!this->readIgnition()) {
        this->currentState = psuStateShuttingDown;
      }

      break;

    case psuStateShuttingDown:
      digitalWrite(this->pinPower, LOW);

      shuttingDownSince = millis();

      do {
        if (!this->readFeedback()) {
          break;
        }
      } while (millis() - shuttingDownSince < SHUT_DOWN_TIMEOUT);

      digitalWrite(this->pinRelay, LOW);
      this->currentState = psuStateOff;

      break;
  }
}

bool Psu::readIgnition() {
  return digitalRead(this->pinIgnition) == LOW;
}

bool Psu::readFeedback() {
  return digitalRead(this->pinFeedback) == LOW;
}
