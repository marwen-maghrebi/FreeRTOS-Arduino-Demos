#include<Arduino.h>
#include <Arduino_FreeRTOS.h>

/**
 * @brief Task function to blink LED connected to pin 8 with 1 second period (500ms on, 500ms off)
 * @param pvParameters Pointer to task parameters (unused in this case)
 */
void TaskBlinkIO8(void *pvParameters) {
  (void) pvParameters; // Explicitly cast unused parameter to void
  
  // Initialize digital pin 8 as output
  pinMode(8, OUTPUT);
  
  // Infinite task loop
  while (1) {
    digitalWrite(8, HIGH);         // Turn LED on
    vTaskDelay(500 / portTICK_PERIOD_MS);  // Delay for 500ms
    digitalWrite(8, LOW);          // Turn LED off
    vTaskDelay(500 / portTICK_PERIOD_MS);  // Delay for 500ms
  }
}

/**
 * @brief Task function to blink LED connected to pin 9 with 0.5 second period (250ms on, 250ms off)
 * @param pvParameters Pointer to task parameters (unused in this case)
 */
void TaskBlinkIO9(void *pvParameters) {
  (void) pvParameters; // Explicitly cast unused parameter to void
  
  // Initialize digital pin 9 as output
  pinMode(9, OUTPUT);
  
  // Infinite task loop
  while (1) {
    digitalWrite(9, LOW);          // Turn LED off
    vTaskDelay(250 / portTICK_PERIOD_MS);  // Delay for 250ms
    digitalWrite(9, HIGH);         // Turn LED on
    vTaskDelay(250 / portTICK_PERIOD_MS);  // Delay for 250ms
  }
}

/**
 * @brief Arduino setup function - runs once at startup
 * Initializes and creates FreeRTOS tasks
 */
void setup() {
  // Create task for blinking pin 8
  xTaskCreate(
    TaskBlinkIO8,   // Task function
    "Blink8",       // Task name (for debugging)
    128,            // Stack size (bytes in AVR, words in ARM)
    NULL,           // Task parameters
    1,              // Task priority (higher number = higher priority)
    NULL            // Task handle (not used here)
  );
  
  // Create task for blinking pin 9
  xTaskCreate(
    TaskBlinkIO9,   // Task function
    "Blink9",       // Task name (for debugging)
    128,            // Stack size (bytes in AVR, words in ARM)
    NULL,           // Task parameters
    1,              // Task priority
    NULL            // Task handle
  );
  
}

/**
 * @brief Arduino loop function - not used when FreeRTOS scheduler is running
 * The loop() function must exist but remains empty as the RTOS scheduler
 * takes over control of task execution after setup() completes.
 */
void loop() {
  // Empty as FreeRTOS scheduler handles task execution
}