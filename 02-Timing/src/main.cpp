#include <Arduino.h>

#include <Arduino_FreeRTOS.h>

// Pin definitions
#define LED_DELAY_PIN      8  // vTaskDelay (Red LED)
#define LED_DELAYUNTIL_PIN 9  // vTaskDelayUntil (Green LED)

// Timing parameters (ms)
#define TASK_DELAY_MS     2000
#define EXECUTION_TIME_MS 100 // Simulated task execution time

void TaskDelayDemo(void *pvParameters) {
  (void) pvParameters;
  pinMode(LED_DELAY_PIN, OUTPUT);
  
  for(;;) {
    digitalWrite(LED_DELAY_PIN, !digitalRead(LED_DELAY_PIN)); // Toggle alternative
    
    // Simulate variable execution time
    vTaskDelay(EXECUTION_TIME_MS / portTICK_PERIOD_MS);
    
    // Relative delay - period will drift
    vTaskDelay(TASK_DELAY_MS / portTICK_PERIOD_MS);
  }
}

void TaskDelayUntilDemo(void *pvParameters) {
  (void) pvParameters;
  TickType_t xLastWakeTime = xTaskGetTickCount();
  pinMode(LED_DELAYUNTIL_PIN, OUTPUT);
  
  for(;;) {
    digitalWrite(LED_DELAYUNTIL_PIN, !digitalRead(LED_DELAYUNTIL_PIN)); // Toggle alternative
    
    // Simulate the same execution time
    vTaskDelay(EXECUTION_TIME_MS / portTICK_PERIOD_MS);
    
    // Absolute delay - maintains exact period
    vTaskDelayUntil(&xLastWakeTime, TASK_DELAY_MS / portTICK_PERIOD_MS);
  }
}

void setup() {
  // Create both tasks with same priority
  xTaskCreate(
    TaskDelayDemo,
    "vTaskDelay",
    128,
    NULL,
    1,
    NULL
  );
  
  xTaskCreate(
    TaskDelayUntilDemo,
    "vTaskDelayUntil",
    128,
    NULL,
    1,
    NULL
  );
}

void loop() {}