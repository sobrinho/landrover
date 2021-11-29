#include <BleAbsMouse.h>
#include "Faaf.h"
#include "Psu.h"

static const char* TAG = "LANDROVER";

const byte PIN_IGNITION = 18;
const byte PIN_RELAY = 19;
const byte PIN_POWER = 21;
const byte PIN_FEEDBACK = 23;

QueueHandle_t coordinatesQueue;

BleAbsMouse mouse;

Faaf faaf = Faaf(
  &Serial2,
  [](FaafCoordinates coordinates) {
    xQueueSend(coordinatesQueue, &coordinates, (TickType_t) 0);
  }
);

Psu psu = Psu(
  PIN_IGNITION,
  PIN_RELAY,
  PIN_POWER,
  PIN_FEEDBACK
);

void setup() {
  Serial.begin(115200);

  coordinatesQueue = xQueueCreate(64, sizeof(FaafCoordinates));

  if (coordinatesQueue == 0) {
    ESP_LOGE(TAG, "Error creating coordinatesQueue");

    while (true) {
      ;
    }
  }

  mouse.begin();
  faaf.begin();
  psu.begin();

  xTaskCreate(
    mouseTask, // Function that should be called
    "Mouse",   // Name of the task (for debugging)
    2048,      // Stack size (bytes)
    NULL,      // Parameter to pass
    3,         // Task priority
    NULL       // Task handle
  );

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

void mouseTask (void *pvParameters) {
  FaafCoordinates *coordinates;

  if (xQueueReceive(coordinatesQueue, &coordinates, (TickType_t) 0)) {
    if (coordinates->isPressed) {
      mouse.move(coordinates->targetX, coordinates->targetY);
    } else {
      mouse.release();
    }
  }
}

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

void loop() {
}
