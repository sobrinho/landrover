#pragma once
#include "Psu.h"

static const char* TAG = "PSU";

#define SHUT_DOWN_TIMEOUT 5000
#define BOOT_TIMEOUT 5000
#define SHUT_DOWN_DELAY 5000

Psu *instance;

Psu::Psu(byte pinIgnition, byte pinRelay, byte pinPower, byte pinFeedback) {
  this->pinIgnition = pinIgnition;
  this->pinRelay = pinRelay;
  this->pinPower = pinPower;
  this->pinFeedback = pinFeedback;
  this->ignitionState = false;
  this->feedbackState = false;
  this->psuState = psuStateOff;

  instance = this;
}

void Psu::begin() {
  pinMode(pinIgnition, INPUT_PULLUP);
  pinMode(pinRelay, OUTPUT);
  pinMode(pinPower, OUTPUT);
  pinMode(pinFeedback, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(pinIgnition), readIgnitionISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(pinFeedback), readFeedbackISR, CHANGE);

  readIgnition();
  readFeedback();
}

void Psu::loop() {
  while (true) {
    perform();
  }
}

void Psu::perform() {
  unsigned long bootingSince;
  unsigned long ignitionOffSince;
  unsigned long shuttingDownSince;

  ESP_LOGD(TAG, "loop");
  ESP_LOGV(TAG, "ignitionState %i", ignitionState);
  ESP_LOGV(TAG, "feedbackState %i", feedbackState);
  ESP_LOGD(TAG, "psuState %i", psuState);

  switch (psuState) {
    case psuStateOff:
      if (ignitionState) {
        psuState = psuStateBooting;
      }

      break;

    case psuStateBooting:
      digitalWrite(pinRelay, HIGH);
      digitalWrite(pinPower, HIGH);

      bootingSince = millis();

      do {
        ESP_LOGV(TAG, "feedbackState %i", feedbackState);

        if (feedbackState) {
          psuState = psuStateOn;
          break;
        }

        vTaskDelay(100 / portTICK_PERIOD_MS);
      } while (millis() - bootingSince < BOOT_TIMEOUT);

      if (psuState == psuStateOn) {
        break;
      }

      digitalWrite(pinRelay, LOW);
      digitalWrite(pinPower, LOW);

      psuState = psuStateOff;

      break;

    case psuStateOn:
      if (ignitionState) {
        break;
      }

      ignitionOffSince = millis();

      do {
        ESP_LOGV(TAG, "ignitionState %i", ignitionState);

        if (ignitionState) {
          break;
        }

        vTaskDelay(100 / portTICK_PERIOD_MS);
      } while (millis() - ignitionOffSince < SHUT_DOWN_DELAY);

      if (!ignitionState) {
        psuState = psuStateShuttingDown;
      }

      break;

    case psuStateShuttingDown:
      digitalWrite(pinPower, LOW);

      shuttingDownSince = millis();

      do {
        ESP_LOGV(TAG, "feedbackState %i", feedbackState);

        if (!feedbackState) {
          break;
        }

        vTaskDelay(100 / portTICK_PERIOD_MS);
      } while (millis() - shuttingDownSince < SHUT_DOWN_TIMEOUT);

      digitalWrite(pinRelay, LOW);
      psuState = psuStateOff;

      break;
  }

  vTaskDelay(100 / portTICK_PERIOD_MS);
}

void Psu::readIgnition() {
  ignitionState = digitalRead(pinIgnition) == LOW;
}

void Psu::readFeedback() {
  feedbackState = digitalRead(pinFeedback) == LOW;
}

void Psu::readIgnitionISR() {
  instance->readIgnition();
}

void Psu::readFeedbackISR() {
  instance->readFeedback();
}
