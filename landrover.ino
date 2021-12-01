#include "Mouse.h"
#include "Faaf.h"
#include "Power.h"

const byte PIN_IGNITION = 15;
const byte PIN_RELAY = 2;
const byte PIN_POWER = 2;
const byte PIN_FEEDBACK = 23;

void setup() {
  Serial.begin(115200);

  Mouse::begin();

  Faaf::begin(
    &Serial2,
    [](FaafCoordinates faafCoordinates) {
      if (faafCoordinates.isPressed) {
        Mouse::move(faafCoordinates.targetX, faafCoordinates.targetY);
      } else {
        Mouse::release();
      }
    }
  );

  Power::begin(
    PIN_IGNITION,
    PIN_RELAY,
    PIN_POWER,
    PIN_FEEDBACK
  );
}

void loop() {
}
