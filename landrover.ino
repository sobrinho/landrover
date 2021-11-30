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
    mouseTask,
    "Mouse",
    2048,
    (void*) &mouse,
    3,
    NULL
  );

  xTaskCreate(
    faafTask,
    "Faaf",
    2048,
    (void*) &faaf,
    2,
    NULL
  );

  xTaskCreate(
    psuTask,
    "PSU",
    2048,
    (void*) &psu,
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

void faafTask (void* pvParameters) {
  Faaf* faaf = (Faaf*) pvParameters;

  faaf->task();
}

void psuTask (void* pvParameters) {
  Psu* psu = (Psu*) pvParameters;

  psu->task();
}

void loop() {
}
