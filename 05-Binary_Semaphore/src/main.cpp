#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>  // Required for using FreeRTOS semaphores

// Pin definitions
#define LED_RED     8
#define BUTTON_USER 2

// Declare a handle for the binary semaphore
SemaphoreHandle_t xLedSemaphore = NULL;

/**
 * @brief Task to turn on the LED.
 * Waits for the semaphore to become available, simulates LED occupation for 1 second.
 * @param pvParameters Pointer to task parameters (unused)
 */
void TaskTurnOnLed(void *pvParameters) {
  (void) pvParameters;  // Unused parameter

  // Infinite task loop
  while (1) {
    // Wait indefinitely to take the semaphore
    if (xSemaphoreTake(xLedSemaphore, portMAX_DELAY) == pdTRUE) {
      Serial.println("Task ON: Semaphore taken");
      digitalWrite(LED_RED, HIGH);
      Serial.println("Task ON: LED turned ON");

      // Simulate resource occupation
      vTaskDelay(pdMS_TO_TICKS(1000));

      // Do NOT give the semaphore here; the OFF task will release it
      Serial.println("Task ON: Waiting for OFF task to release LED");
    }

    vTaskDelay(pdMS_TO_TICKS(100));  // Small delay to yield
  }
}

/**
 * @brief Task to turn off the LED when button is pressed, and release the semaphore when button is released.
 * @param pvParameters Pointer to task parameters (unused)
 */
void TaskTurnOffLed(void *pvParameters) {
  (void) pvParameters;  // Unused parameter

  // Infinite task loop
  while (1) {
    int buttonState = digitalRead(BUTTON_USER);

    if (buttonState == LOW) {
      // Button pressed: immediately turn off LED
      digitalWrite(LED_RED, LOW);
      Serial.println("Task OFF: Button pressed, LED turned OFF");
    }
    else if (buttonState == HIGH) {
      // Button released: release the semaphore
      if (xSemaphoreGive(xLedSemaphore) == pdTRUE) {
        Serial.println("Task OFF: Button released, semaphore given");
      }
    }

    vTaskDelay(pdMS_TO_TICKS(50));  // Debounce delay
  }
}

/**
 * @brief Arduino setup function - runs once at startup.
 * Initializes I/O pins, semaphore, and creates FreeRTOS tasks.
 */
void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  while (!Serial);  // Wait for Serial if necessary

  // Configure pins
  pinMode(LED_RED, OUTPUT);
  pinMode(BUTTON_USER, INPUT_PULLUP);  // Active LOW button

  // Create a binary semaphore
  xLedSemaphore = xSemaphoreCreateBinary();

  // Check semaphore creation
  if (xLedSemaphore == NULL) {
    Serial.println("Error creating semaphore.");
    while (1);  // Halt if semaphore failed
  }

  // Initial give to make the semaphore available at start
  xSemaphoreGive(xLedSemaphore);

  // Create task to turn on the LED
  xTaskCreate(
    TaskTurnOnLed,   // Task function
    "LED_On_Task",   // Task name
    128,             // Stack size
    NULL,            // Task parameters
    1,               // Task priority
    NULL             // Task handle
  );

  // Create task to turn off the LED
  xTaskCreate(
    TaskTurnOffLed,  // Task function
    "LED_Off_Task",  // Task name
    128,             // Stack size
    NULL,            // Task parameters
    1,               // Task priority
    NULL             // Task handle
  );
}

/**
 * @brief Arduino main loop - unused with FreeRTOS scheduler running.
 */
void loop() {
  // Empty - FreeRTOS manages task scheduling
}
