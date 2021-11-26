const byte PIN_IGNITION = 3;
const byte PIN_RELAY = 14;
const byte PIN_POWER = 16;
const byte PIN_FEEDBACK = 7;

const byte STATE_OFF = 0;
const byte STATE_BOOTING = 1;
const byte STATE_ON = 2;
const byte STATE_SHUTTING_DOWN = 3;

const unsigned long BOOT_TIMEOUT = 60000;
const unsigned long SHUT_DOWN_DELAY = 60000;
const unsigned long SHUT_DOWN_TIMEOUT = 60000;

volatile boolean ignition = false;
volatile boolean feedback = false;

void setup() {
  pinMode(PIN_IGNITION, INPUT_PULLUP);
  pinMode(PIN_RELAY, OUTPUT);
  pinMode(PIN_POWER, OUTPUT);
  pinMode(PIN_FEEDBACK, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(PIN_IGNITION), readIgnition, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PIN_FEEDBACK), readFeedback, CHANGE);

  readIgnition();
  readFeedback();
}

void readIgnition() {
  ignition = digitalRead(PIN_IGNITION) == LOW;
}

void readFeedback() {
  feedback = digitalRead(PIN_FEEDBACK) == LOW;
}

void loop() {
  static byte currentState = STATE_OFF;

  unsigned long bootingSince;
  unsigned long ignitionOffSince;
  unsigned long shuttingDownSince;

  switch (currentState) {
    case STATE_OFF:
      if (ignition) {
        currentState = STATE_BOOTING;
      }

      break;

    case STATE_BOOTING:
      digitalWrite(PIN_RELAY, HIGH);
      digitalWrite(PIN_POWER, HIGH);

      bootingSince = millis();

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
      if (ignition) {
        return;
      }

      ignitionOffSince = millis();

      do {
        if (ignition) {
          return;
        }
      } while (millis() - ignitionOffSince < SHUT_DOWN_DELAY);

      currentState = STATE_SHUTTING_DOWN;

      break;

    case STATE_SHUTTING_DOWN:
      digitalWrite(PIN_POWER, LOW);

      shuttingDownSince = millis();

      do {
        if (!feedback) {
          break;
        }
      } while (millis() - shuttingDownSince < SHUT_DOWN_TIMEOUT);

      digitalWrite(PIN_RELAY, LOW);
      currentState = STATE_OFF;

      break;
  }
}
