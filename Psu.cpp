#pragma once
#include "Psu.h"

static const char* TAG = "PSU";

#define SHUT_DOWN_TIMEOUT 5000
#define BOOT_TIMEOUT 5000
#define SHUT_DOWN_DELAY 5000

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

void Psu::perform() {
  unsigned long bootingSince;
  unsigned long ignitionOffSince;
  unsigned long shuttingDownSince;

  ESP_LOGD(TAG, "loop");
  ESP_LOGV(TAG, "currentState %i", this->currentState);
  ESP_LOGV(TAG, "ignitionState %i", this->readIgnition());
  ESP_LOGV(TAG, "feedbackState %i", this->readFeedback());

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
        ESP_LOGV(TAG, "feedbackState %i", this->readFeedback());

        if (this->readFeedback()) {
          this->currentState = psuStateOn;
          break;
        }

        vTaskDelay(10 / portTICK_PERIOD_MS);
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
        ESP_LOGV(TAG, "ignitionState %i", this->readIgnition());

        if (this->readIgnition()) {
          break;
        }

        vTaskDelay(10 / portTICK_PERIOD_MS);
      } while (millis() - ignitionOffSince < SHUT_DOWN_DELAY);

      if (!this->readIgnition()) {
        this->currentState = psuStateShuttingDown;
      }

      break;

    case psuStateShuttingDown:
      digitalWrite(this->pinPower, LOW);

      shuttingDownSince = millis();

      do {
        ESP_LOGV(TAG, "feedbackState %i", this->readFeedback());

        if (!this->readFeedback()) {
          break;
        }

        vTaskDelay(10 / portTICK_PERIOD_MS);
      } while (millis() - shuttingDownSince < SHUT_DOWN_TIMEOUT);

      digitalWrite(this->pinRelay, LOW);
      this->currentState = psuStateOff;

      break;
  }

  vTaskDelay(10 / portTICK_PERIOD_MS);
}

bool Psu::readIgnition() {
  return digitalRead(this->pinIgnition) == LOW;
}

bool Psu::readFeedback() {
  return digitalRead(this->pinFeedback) == LOW;
}
