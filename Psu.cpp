#pragma once
#include "Psu.h"

#define SHUT_DOWN_TIMEOUT 5000
#define BOOT_TIMEOUT 5000
#define SHUT_DOWN_DELAY 5000

static const char* TAG = "PSU";

byte Psu::_pinIgnition = NULL;
byte Psu::_pinRelay = NULL;
byte Psu::_pinPower = NULL;
byte Psu::_pinFeedback = NULL;
volatile boolean Psu::_ignitionState = false;
volatile boolean Psu::_feedbackState = false;
volatile PsuState Psu::_psuState = psuStateOff;

void Psu::begin(byte pinIgnition, byte pinRelay, byte pinPower, byte pinFeedback) {
  _pinIgnition = pinIgnition;
  _pinRelay = pinRelay;
  _pinPower = pinPower;
  _pinFeedback = pinFeedback;

  pinMode(_pinIgnition, INPUT_PULLUP);
  pinMode(_pinRelay, OUTPUT);
  pinMode(_pinPower, OUTPUT);
  pinMode(_pinFeedback, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(_pinIgnition), _readIgnition, CHANGE);
  attachInterrupt(digitalPinToInterrupt(_pinFeedback), _readFeedback, CHANGE);

  _readIgnition();
  _readFeedback();
}

void Psu::taskServer(void* pvParameters) {
  while (true) {
    _perform();
  }
}

void Psu::_perform() {
  unsigned long bootingSince;
  unsigned long ignitionOffSince;
  unsigned long shuttingDownSince;

  ESP_LOGD(TAG, "loop");
  ESP_LOGV(TAG, "ignitionState %i", _ignitionState);
  ESP_LOGV(TAG, "feedbackState %i", _feedbackState);
  ESP_LOGV(TAG, "psuState %i", _psuState);

  switch (_psuState) {
    case psuStateOff:
      if (_ignitionState) {
        _psuState = psuStateBooting;
      }

      break;

    case psuStateBooting:
      digitalWrite(_pinRelay, HIGH);
      digitalWrite(_pinPower, HIGH);

      bootingSince = millis();

      do {
        ESP_LOGV(TAG, "feedbackState %i", _feedbackState);

        if (_feedbackState) {
          _psuState = psuStateOn;
          break;
        }

        vTaskDelay(100 / portTICK_PERIOD_MS);
      } while (millis() - bootingSince < BOOT_TIMEOUT);

      if (_psuState == psuStateOn) {
        break;
      }

      digitalWrite(_pinRelay, LOW);
      digitalWrite(_pinPower, LOW);

      _psuState = psuStateOff;

      break;

    case psuStateOn:
      if (_ignitionState) {
        break;
      }

      ignitionOffSince = millis();

      do {
        ESP_LOGV(TAG, "ignitionState %i", _ignitionState);

        if (_ignitionState) {
          break;
        }

        vTaskDelay(100 / portTICK_PERIOD_MS);
      } while (millis() - ignitionOffSince < SHUT_DOWN_DELAY);

      if (!_ignitionState) {
        _psuState = psuStateShuttingDown;
      }

      break;

    case psuStateShuttingDown:
      digitalWrite(_pinPower, LOW);

      shuttingDownSince = millis();

      do {
        ESP_LOGV(TAG, "feedbackState %i", _feedbackState);

        if (!_feedbackState) {
          break;
        }

        vTaskDelay(100 / portTICK_PERIOD_MS);
      } while (millis() - shuttingDownSince < SHUT_DOWN_TIMEOUT);

      digitalWrite(_pinRelay, LOW);
      _psuState = psuStateOff;

      break;
  }

  vTaskDelay(100 / portTICK_PERIOD_MS);
}

void Psu::_readIgnition() {
  _ignitionState = digitalRead(_pinIgnition) == LOW;
}

void Psu::_readFeedback() {
  _feedbackState = digitalRead(_pinFeedback) == LOW;
}
