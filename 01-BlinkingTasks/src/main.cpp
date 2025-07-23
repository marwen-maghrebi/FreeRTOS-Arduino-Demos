#include <Arduino.h>
#include <Arduino_FreeRTOS.h>

// Pin definitions
#define LED_DELAY_PIN      8  // vTaskDelay (Red LED)
#define LED_DELAYUNTIL_PIN 9  // vTaskDelayUntil (Green LED)

// Timing parameters (ms)
#define TASK_DELAY_MS     2000
#define EXECUTION_TIME_MS 100 // Simulated task execution time

/**
 * @brief Task demonstrating vTaskDelay functionality
 * @param pvParameters Pointer to task parameters (unused in this case)
 * 
 * This task toggles an LED using vTaskDelay, which creates a relative delay.
 * Note that the period may drift due to variable execution time.
 */
void TaskDelayDemo(void *pvParameters) {
  (void) pvParameters; // Explicitly cast unused parameter to void
  
  // Initialize digital pin as output
  pinMode(LED_DELAY_PIN, OUTPUT);
  
  // Infinite task loop
  while (1) {
    digitalWrite(LED_DELAY_PIN, !digitalRead(LED_DELAY_PIN)); // Toggle LED
    
    // Simulate variable execution time
    vTaskDelay(EXECUTION_TIME_MS / portTICK_PERIOD_MS);
    
    // Relative delay - period will drift
    vTaskDelay(TASK_DELAY_MS / portTICK_PERIOD_MS);
  }
}

/**
 * @brief Task demonstrating vTaskDelayUntil functionality
 * @param pvParameters Pointer to task parameters (unused in this case)
 * 
 * This task toggles an LED using vTaskDelayUntil, which maintains
 * a consistent period by accounting for execution time.
 */
void TaskDelayUntilDemo(void *pvParameters) {
  (void) pvParameters; // Explicitly cast unused parameter to void
  TickType_t xLastWakeTime = xTaskGetTickCount();
  
  // Initialize digital pin as output
  pinMode(LED_DELAYUNTIL_PIN, OUTPUT);
  
  // Infinite task loop
  while (1) {
    digitalWrite(LED_DELAYUNTIL_PIN, !digitalRead(LED_DELAYUNTIL_PIN)); // Toggle LED
    
    // Simulate the same execution time
    vTaskDelay(EXECUTION_TIME_MS / portTICK_PERIOD_MS);
    
    // Absolute delay - maintains exact period
    vTaskDelayUntil(&xLastWakeTime, TASK_DELAY_MS / portTICK_PERIOD_MS);
  }
}

/**
 * @brief Arduino setup function - runs once at startup
 * Initializes and creates FreeRTOS tasks
 */
void setup() {
  // Create task for vTaskDelay demonstration
  xTaskCreate(
    TaskDelayDemo,    // Task function
    "vTaskDelay",     // Task name (for debugging)
    128,              // Stack size (bytes in AVR, words in ARM)
    NULL,             // Task parameters
    1,                // Task priority (higher number = higher priority)
    NULL              // Task handle (not used here)
  );
  
  // Create task for vTaskDelayUntil demonstration
  xTaskCreate(
    TaskDelayUntilDemo, // Task function
    "vTaskDelayUntil",  // Task name (for debugging)
    128,                // Stack size
    NULL,               // Task parameters
    1,                  // Task priority
    NULL                // Task handle
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