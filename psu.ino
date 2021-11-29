#include <BleAbsMouse.h>
#include "Faaf.h"
#include "Psu.h"

const byte PIN_IGNITION = 3;
const byte PIN_RELAY = 14;
const byte PIN_POWER = 16;
const byte PIN_FEEDBACK = 7;

BleAbsMouse mouse;

Faaf faaf = Faaf(
  &Serial2,
  [&mouse](boolean isPressed, unsigned int targetX, unsigned int targetY) {
    if (isPressed) {
      mouse.move(targetX, targetY);
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
    faaf.loop();
  }
}

void psuTask (void *pvParameters) {
  while (true) {
    psu.loop();
  }
}

void setup() {
  mouse.begin();
  faaf.begin();
  psu.begin();

  xTaskCreate(
    psuTask, // Function that should be called
    "PSU",   // Name of the task (for debugging)
    1024,    // Stack size (bytes)
    NULL,    // Parameter to pass
    1,       // Task priority
    NULL     // Task handle
  );

  xTaskCreate(
    faafTask, // Function that should be called
    "Faaf",   // Name of the task (for debugging)
    1024,     // Stack size (bytes)
    NULL,     // Parameter to pass
    2,        // Task priority
    NULL      // Task handle
  );

  // TODO: Can we deep sleep while ignition is off?
  // esp_sleep_enable_ext0_wakeup(PIN_IGNITION, 0);
  // esp_deep_sleep_start();
}

void loop() {
}
