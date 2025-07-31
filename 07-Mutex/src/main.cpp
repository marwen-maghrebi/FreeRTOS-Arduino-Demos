#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>  // Required for using FreeRTOS semaphores

// 📌 Pin Definitions
#define POTENTIOMETER_PIN  A0
#define RED_LED_PIN        5
#define YELLOW_LED_PIN     6
#define GREEN_LED_PIN      7

// 🔄 Shared Resource
volatile int adcValue = 0;  // Shared ADC value (volatile for task access)
SemaphoreHandle_t xADCMutex = NULL;  // Mutex to protect shared resource

/**
 * @brief Task to periodically read the analog input from the potentiometer.
 *        Updates shared adcValue under mutex protection.
 * @param pvParameters Pointer to task parameters (unused)
 */
void TaskReadADC(void *pvParameters) {
  (void) pvParameters;

  while (1) {
    if (xSemaphoreTake(xADCMutex, portMAX_DELAY) == pdTRUE) {
      adcValue = analogRead(POTENTIOMETER_PIN);  // Critical section
      xSemaphoreGive(xADCMutex);
    }

    vTaskDelay(pdMS_TO_TICKS(50));  // Read every 50ms
  }
}

/**
 * @brief Task to print the current ADC value to the Serial Monitor.
 *        Accesses shared adcValue under mutex protection.
 * @param pvParameters Pointer to task parameters (unused)
 */
void TaskPrintADC(void *pvParameters) {
  (void) pvParameters;

  while (1) {
    if (xSemaphoreTake(xADCMutex, portMAX_DELAY) == pdTRUE) {
      Serial.print("ADC Value: ");
      Serial.println(adcValue);
      xSemaphoreGive(xADCMutex);
    }

    vTaskDelay(pdMS_TO_TICKS(200));  // Print every 200ms
  }
}

/**
 * @brief Task to control the RGB LEDs based on adcValue range.
 *        Reads adcValue under mutex, then acts based on thresholds.
 * @param pvParameters Pointer to task parameters (unused)
 */
void TaskControlLEDs(void *pvParameters) {
  (void) pvParameters;

  while (1) {
    int localValue = 0;

    if (xSemaphoreTake(xADCMutex, portMAX_DELAY) == pdTRUE) {
      localValue = adcValue;  // Copy safely
      xSemaphoreGive(xADCMutex);
    }

    // LED control based on value range
    if (localValue < 341) {
      digitalWrite(RED_LED_PIN, HIGH);
      digitalWrite(YELLOW_LED_PIN, LOW);
      digitalWrite(GREEN_LED_PIN, LOW);
    }
    else if (localValue < 682) {
      digitalWrite(RED_LED_PIN, LOW);
      digitalWrite(YELLOW_LED_PIN, HIGH);
      digitalWrite(GREEN_LED_PIN, LOW);
    }
    else {
      digitalWrite(RED_LED_PIN, LOW);
      digitalWrite(YELLOW_LED_PIN, LOW);
      digitalWrite(GREEN_LED_PIN, HIGH);
    }

    vTaskDelay(pdMS_TO_TICKS(100));  // Update every 100ms
  }
}

/**
 * @brief Arduino setup function - initializes I/O, mutex, and FreeRTOS tasks.
 */
void setup() {
  Serial.begin(9600);
  while (!Serial);  // Wait for Serial if needed

  // Configure I/O pins
  pinMode(POTENTIOMETER_PIN, INPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);

  // Initialize LED states
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(YELLOW_LED_PIN, LOW);
  digitalWrite(GREEN_LED_PIN, LOW);

  // Create mutex
  xADCMutex = xSemaphoreCreateMutex();
  if (xADCMutex == NULL) {
    Serial.println("Error: Failed to create ADC mutex");
    while (1);  // Stop execution
  }
// Create task for reading ADC value
  xTaskCreate(
    TaskReadADC,         // Task function
    "ADC_Read_Task",     // Task name (for debugging)
    128,                 // Stack size
    NULL,                // Parameters
    2,                   // Priority (higher)
    NULL                 // Task handle (not used)
  );

  // Create task for printing ADC value
  xTaskCreate(
    TaskPrintADC,        // Task function
    "ADC_Print_Task",    // Task name
    128,                 // Stack size
    NULL,                // Parameters
    1,                   // Priority
    NULL                 // Task handle
  );

  // Create task for controlling LEDs based on ADC
  xTaskCreate(
    TaskControlLEDs,     // Task function
    "LED_Control_Task",  // Task name
    128,                 // Stack size
    NULL,                // Parameters
    1,                   // Priority
    NULL                 // Task handle
  );
  // Startup message
  Serial.println("ADC Monitoring System Started");
}

/**
 * @brief Arduino main loop - unused when using FreeRTOS.
 */
void loop() {
  // Empty - FreeRTOS scheduler takes over
}
