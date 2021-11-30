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

  Faaf::begin(
    &Serial2,
    [](FaafCoordinates coordinates) {
      xQueueSend(coordinatesQueue, &coordinates, (TickType_t) 0);
    }
  );

  Psu::begin(
    PIN_IGNITION,
    PIN_RELAY,
    PIN_POWER,
    PIN_FEEDBACK
  );

  xTaskCreate(
    mouseTask,
    "Mouse",
    2048,
    (void*) &mouse,
    3,
    NULL
  );

  xTaskCreate(
    Faaf::taskServer,
    "Faaf",
    2048,
    NULL,
    2,
    NULL
  );

  xTaskCreate(
    Psu::taskServer,
    "PSU",
    2048,
    NULL,
    1,
    NULL
  );

  // TODO: Can we deep sleep while ignition is off?
  // esp_sleep_enable_ext0_wakeup(PIN_IGNITION, 0);
  // esp_deep_sleep_start();
}

void mouseTask (void* pvParameters) {
  BleAbsMouse* mouse = (BleAbsMouse*) pvParameters;
  FaafCoordinates* coordinates;

  while (true) {
    xQueueReceive(coordinatesQueue, &coordinates, portMAX_DELAY);

    if (coordinates->isPressed) {
      mouse->move(coordinates->targetX, coordinates->targetY);
    } else {
      mouse->release();
    }
  }
}

void loop() {
}
