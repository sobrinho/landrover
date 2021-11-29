#include <BleAbsMouse.h>
#include "Faaf.h"
#include "Psu.h"

const byte PIN_IGNITION = 18;
const byte PIN_RELAY = 19;
const byte PIN_POWER = 21;
const byte PIN_FEEDBACK = 23;

BleAbsMouse mouse;

Faaf faaf = Faaf(
  &Serial2,
  [&mouse](FaafCoordinates coordinates) {
    if (coordinates.isPressed) {
      mouse.move(coordinates.targetX, coordinates.targetY);
    } else {
      mouse.release();
    }
  }
);

Psu psu = Psu(
  PIN_IGNITION,
  PIN_RELAY,
  PIN_POWER,
  PIN_FEEDBACK
);

void faafTask (void *pvParameters) {
  while (true) {
    faaf.perform();
  }
}

void psuTask (void *pvParameters) {
  while (true) {
    psu.perform();
  }
}

void setup() {
  Serial.begin(115200);

  mouse.begin();
  faaf.begin();
  psu.begin();

  xTaskCreate(
    faafTask, // Function that should be called
    "Faaf",   // Name of the task (for debugging)
    2048,     // Stack size (bytes)
    NULL,     // Parameter to pass
    2,        // Task priority
    NULL      // Task handle
  );

  xTaskCreate(
    psuTask, // Function that should be called
    "PSU",   // Name of the task (for debugging)
    2048,    // Stack size (bytes)
    NULL,    // Parameter to pass
    1,       // Task priority
    NULL     // Task handle
  );

  // TODO: Can we deep sleep while ignition is off?
  // esp_sleep_enable_ext0_wakeup(PIN_IGNITION, 0);
  // esp_deep_sleep_start();
}

void loop() {
}
