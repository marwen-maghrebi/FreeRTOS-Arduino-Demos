#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>  // Required for using FreeRTOS semaphores

// Pin definitions
#define ENTRY_GATE_LED   7
#define EXIT_GATE_LED    8
#define OVERRIDE_LED     9
#define EXIT_BUTTON     10

#define TOTAL_PARKING_SPACES 2  // Total parking slots
#define NUM_CARS              3 // Number of cars to simulate

const int parkingLEDs[] = {2, 3};  // LEDs representing occupied parking spots

// Declare a handle for the counting semaphore
SemaphoreHandle_t xParkingSemaphore = NULL;

/**
 * @brief Task for Car 1.
 * Attempts to park, occupies space, simulates parking, and then exits.
 * @param pvParameters Pointer to task parameters (unused)
 */
void CarTask1(void *pvParameters) {
  (void) pvParameters;  // Unused parameter

  while (1) {
    if (xSemaphoreTake(xParkingSemaphore, portMAX_DELAY) == pdTRUE) {
      Serial.println("Car 1: Entered parking");

      digitalWrite(parkingLEDs[0], HIGH);
      digitalWrite(ENTRY_GATE_LED, HIGH);
      vTaskDelay(pdMS_TO_TICKS(200));
      digitalWrite(ENTRY_GATE_LED, LOW);

      Serial.print("Car 1: Parked. Spaces left: ");
      Serial.println(uxSemaphoreGetCount(xParkingSemaphore));

      vTaskDelay(pdMS_TO_TICKS(3000));  // Simulate parked time

      digitalWrite(parkingLEDs[0], LOW);
      digitalWrite(EXIT_GATE_LED, HIGH);
      vTaskDelay(pdMS_TO_TICKS(200));
      digitalWrite(EXIT_GATE_LED, LOW);

      xSemaphoreGive(xParkingSemaphore);
      Serial.println("Car 1: Left parking");

      vTaskDelay(pdMS_TO_TICKS(2000));  // Wait before re-entering
    }
  }
}

/**
 * @brief Task for Car 2.
 * Waits slightly before trying to park, occupies space, simulates parking, and then exits.
 * @param pvParameters Pointer to task parameters (unused)
 */
void CarTask2(void *pvParameters) {
  (void) pvParameters;  // Unused parameter

  vTaskDelay(pdMS_TO_TICKS(1000));  // Stagger start

  while (1) {
    if (xSemaphoreTake(xParkingSemaphore, portMAX_DELAY) == pdTRUE) {
      Serial.println("Car 2: Entered parking");

      digitalWrite(parkingLEDs[1], HIGH);
      digitalWrite(ENTRY_GATE_LED, HIGH);
      vTaskDelay(pdMS_TO_TICKS(200));
      digitalWrite(ENTRY_GATE_LED, LOW);

      Serial.print("Car 2: Parked. Spaces left: ");
      Serial.println(uxSemaphoreGetCount(xParkingSemaphore));

      vTaskDelay(pdMS_TO_TICKS(4000));  // Simulate parked time

      digitalWrite(parkingLEDs[1], LOW);
      digitalWrite(EXIT_GATE_LED, HIGH);
      vTaskDelay(pdMS_TO_TICKS(200));
      digitalWrite(EXIT_GATE_LED, LOW);

      xSemaphoreGive(xParkingSemaphore);
      Serial.println("Car 2: Left parking");

      vTaskDelay(pdMS_TO_TICKS(3000));  // Wait before re-entering
    }
  }
}

/**
 * @brief Task to monitor the manual exit button.
 * Forces release of a parking space when the button is pressed.
 * @param pvParameters Pointer to task parameters (unused)
 */
void ExitButtonTask(void *pvParameters) {
  (void) pvParameters;  // Unused parameter

  while (1) {
    if (digitalRead(EXIT_BUTTON) == LOW) {
      if (uxSemaphoreGetCount(xParkingSemaphore) < TOTAL_PARKING_SPACES) {
        xSemaphoreGive(xParkingSemaphore);
        digitalWrite(OVERRIDE_LED, HIGH);
        Serial.println("Override: Manual exit triggered");
        Serial.print("Spaces available: ");
        Serial.println(uxSemaphoreGetCount(xParkingSemaphore));
        vTaskDelay(pdMS_TO_TICKS(500));
        digitalWrite(OVERRIDE_LED, LOW);
      }
      vTaskDelay(pdMS_TO_TICKS(500));  // Debounce delay
    }

    vTaskDelay(pdMS_TO_TICKS(100));  // Polling interval
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

  // Configure parking LEDs
  for (int i = 0; i < TOTAL_PARKING_SPACES; i++) {
    pinMode(parkingLEDs[i], OUTPUT);
    digitalWrite(parkingLEDs[i], LOW);
  }

  // Configure gate and override LEDs, and exit button
  pinMode(ENTRY_GATE_LED, OUTPUT);
  pinMode(EXIT_GATE_LED, OUTPUT);
  pinMode(OVERRIDE_LED, OUTPUT);
  pinMode(EXIT_BUTTON, INPUT_PULLUP);  // Active LOW button

  // Create the counting semaphore
  xParkingSemaphore = xSemaphoreCreateCounting(TOTAL_PARKING_SPACES, TOTAL_PARKING_SPACES);

  // Check semaphore creation
  if (xParkingSemaphore == NULL) {
    Serial.println("Error creating parking semaphore.");
    while (1);  // Halt if semaphore failed
  }

  // Create task for Car 1
  xTaskCreate(
    CarTask1,         // Task function
    "Car_1_Task",     // Task name
    128,              // Stack size
    NULL,             // Task parameters
    1,                // Task priority
    NULL              // Task handle
  );

  // Create task for Car 2
  xTaskCreate(
    CarTask2,         // Task function
    "Car_2_Task",     // Task name
    128,              // Stack size
    NULL,             // Task parameters
    1,                // Task priority
    NULL              // Task handle
  );

  // Create task for manual exit button
  xTaskCreate(
    ExitButtonTask,   // Task function
    "Exit_Button_Task", // Task name
    128,              // Stack size
    NULL,             // Task parameters
    2,                // Task priority (higher)
    NULL              // Task handle
  );

  // System startup message
  Serial.println("Parking lot system started!");
  Serial.print("Total parking spaces: ");
  Serial.println(TOTAL_PARKING_SPACES);
}

/**
 * @brief Arduino main loop - unused with FreeRTOS scheduler running.
 */
void loop() {
  // Empty - FreeRTOS manages task scheduling
}
