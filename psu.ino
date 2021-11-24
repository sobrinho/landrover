// Original: https://github.com/rocketscream/Low-Power
// ATtiny85: https://github.com/rocketscream/Low-Power/pull/92
#include <LowPower.h>

const byte PIN_IGNITION = 0;
const byte PIN_RELAY = 1;
const byte PIN_POWER = 2;
const byte PIN_FEEDBACK = 3;
const byte PIN_NC1 = 4;

const byte STATE_OFF = 0;
const byte STATE_BOOTING = 1;
const byte STATE_ON = 2;
const byte STATE_SHUTTING_DOWN = 3;

const unsigned long BOOT_TIMEOUT = 1 * 60 * 1000;
const unsigned long SHUT_DOWN_DELAY = 5 * 60 * 1000;
const unsigned long SHUT_DOWN_TIMEOUT = 1 * 60 * 1000;

volatile boolean ignition = false;
volatile boolean feedback = false;

void setup() {
  pinMode(PIN_IGNITION, INPUT_PULLUP);
  pinMode(PIN_RELAY, OUTPUT);
  pinMode(PIN_POWER, OUTPUT);
  pinMode(PIN_FEEDBACK, INPUT_PULLUP);
  pinMode(PIN_NC1, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(PIN_IGNITION), readIgnition, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PIN_FEEDBACK), readFeedback, CHANGE);
}

void readIgnition() {
  ignition = digitalRead(PIN_IGNITION) == LOW;
}

void readFeedback() {
  feedback = digitalRead(PIN_FEEDBACK) == LOW;
}

void loop() {
  static byte currentState = STATE_OFF;

  LowPower.powerDown(SLEEP_2S, ADC_OFF, BOD_OFF);

  switch (currentState) {
    case STATE_OFF:
      if (ignition) {
        digitalWrite(PIN_RELAY, HIGH);
        digitalWrite(PIN_POWER, HIGH);
        currentState = STATE_BOOTING;
      }

      break;

    case STATE_BOOTING:
      unsigned long bootingSince = millis();

      do {
        if (feedback) {
          currentState = STATE_ON;
          return;
        }
      } while (millis() - bootingSince < BOOT_TIMEOUT);

      digitalWrite(PIN_RELAY, LOW);
      digitalWrite(PIN_POWER, LOW);
      currentState = STATE_OFF;

      break;

    case STATE_ON:
      unsigned long ignitionOffSince = millis();

      do {
        if (ignition) {
          return;
        }
      } while (millis() - ignitionOffSince < SHUT_DOWN_DELAY);

      digitalWrite(PIN_POWER, LOW);
      currentState = STATE_SHUTTING_DOWN;

      break;

    case STATE_SHUTTING_DOWN:
      unsigned long shuttingDownSince = millis();

      do {
        if (!feedback) {
          currentState = STATE_OFF;
          return;
        }
      } while (millis() - shuttingDownSince < SHUT_DOWN_TIMEOUT);

      digitalWrite(PIN_RELAY, LOW);
      currentState = STATE_OFF;

      break;
  }
}
