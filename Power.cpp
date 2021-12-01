#pragma once
#include "Power.h"

#define SHUT_DOWN_TIMEOUT 2500
#define BOOT_TIMEOUT 2500
#define SHUT_DOWN_DELAY 2500
#define DEEP_SLEEP_TIMEOUT 5000

static const char* TAG = "POWER";

byte Power::_pinIgnition = NULL;
byte Power::_pinRelay = NULL;
byte Power::_pinPower = NULL;
byte Power::_pinFeedback = NULL;
volatile boolean Power::_ignitionState = false;
volatile boolean Power::_feedbackState = false;
volatile PowerState Power::_powerState = powerStateOff;

void Power::begin(byte pinIgnition, byte pinRelay, byte pinPower, byte pinFeedback) {
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

  xTaskCreate(
    Power::_taskServer,
    "POWER",
    2048,
    NULL,
    1,
    NULL
  );
}

void Power::_taskServer(void* pvParameters) {
  while (true) {
    _perform();
  }
}

void Power::_perform() {
  unsigned long offSince;
  unsigned long bootingSince;
  unsigned long ignitionOffSince;
  unsigned long shuttingDownSince;

  ESP_LOGD(TAG, "tick");
  ESP_LOGV(TAG, "ignitionState %i", _ignitionState);
  ESP_LOGV(TAG, "feedbackState %i", _feedbackState);
  ESP_LOGV(TAG, "powerState %i", _powerState);

  switch (_powerState) {
    case powerStateOff:
      offSince = millis();

      do {
        ESP_LOGV(TAG, "ignitionState %i", _ignitionState);

        if (_ignitionState) {
          _powerState = powerStateBooting;
          break;
        }

        vTaskDelay(100 / portTICK_PERIOD_MS);
      } while (millis() - offSince < DEEP_SLEEP_TIMEOUT);

      if (_powerState == powerStateOff) {
        ESP_LOGV(TAG, "deep sleep mode");

        esp_sleep_enable_ext0_wakeup((gpio_num_t) _pinIgnition, (int) LOW);
        esp_deep_sleep_start();
      }

      break;

    case powerStateBooting:
      digitalWrite(_pinRelay, HIGH);
      digitalWrite(_pinPower, HIGH);

      bootingSince = millis();

      do {
        ESP_LOGV(TAG, "feedbackState %i", _feedbackState);

        if (_feedbackState) {
          _powerState = powerStateOn;
          break;
        }

        vTaskDelay(100 / portTICK_PERIOD_MS);
      } while (millis() - bootingSince < BOOT_TIMEOUT);

      if (_powerState == powerStateOn) {
        break;
      }

      digitalWrite(_pinRelay, LOW);
      digitalWrite(_pinPower, LOW);

      _powerState = powerStateOff;

      break;

    case powerStateOn:
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
        _powerState = powerStateShuttingDown;
      }

      break;

    case powerStateShuttingDown:
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
      _powerState = powerStateOff;

      break;
  }

  vTaskDelay(1000 / portTICK_PERIOD_MS);
}

void Power::_readIgnition() {
  _ignitionState = digitalRead(_pinIgnition) == LOW;
}

void Power::_readFeedback() {
  _feedbackState = digitalRead(_pinFeedback) == LOW;
}
