// Original: https://github.com/rocketscream/Low-Power
// ATtiny85: https://github.com/rocketscream/Low-Power/pull/92
#include <LowPower.h>

const byte PIN_IGNITION = 0;
const byte PIN_RELAY = 1;
const byte PIN_POWER = 2;
const byte PIN_FEEDBACK = 3;
const byte PIN_NC1 = 4;

const byte STATE_OFF = 0;
const byte STATE_POWERING_UP = 1;
const byte STATE_ON = 2;
const byte STATE_SHUTTING_DOWN = 3;

void setup() {
  pinMode(PIN_IGNITION, INPUT_PULLUP);
  pinMode(PIN_RELAY, OUTPUT);
  pinMode(PIN_POWER, OUTPUT);
  pinMode(PIN_FEEDBACK, INPUT_PULLUP);
  pinMode(PIN_NC1, INPUT_PULLUP);
}

void loop() {
  LowPower.powerDown(SLEEP_2S, ADC_OFF, BOD_OFF);

  static byte currentState = STATE_OFF;

  boolean ignition = digitalRead(PIN_IGNITION) == LOW;
  boolean feedback = digitalRead(PIN_FEEDBACK) == LOW;

  switch (currentState) {
    case STATE_OFF:
      if (ignition) {
        digitalWrite(PIN_RELAY, HIGH);
        digitalWrite(PIN_POWER, HIGH);
        currentState = STATE_POWERING_UP;
      }

      break;

    case STATE_POWERING_UP:
      if (feedback) {
        currentState = STATE_ON;
      }

      break;

    case STATE_ON:
      if (!ignition) {
        digitalWrite(PIN_POWER, LOW);
        currentState = STATE_SHUTTING_DOWN;
      }

      break;

    case STATE_SHUTTING_DOWN:
      if (!feedback) {
        digitalWrite(PIN_RELAY, LOW);
        currentState = STATE_OFF;
      }

      break;
  }
}
