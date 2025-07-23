#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <task.h>

// Pin definitions
#define LED_GREEN      9
#define LED_RED        8
#define BUTTON_EMERG   2
#define BUTTON_START   3
#define BUTTON_STOP    4

// Global state flags (volatile since accessed in multiple tasks)
volatile bool emergency = false;
volatile bool systemStarted = false;

// Handle for the blinking task to suspend/resume it
TaskHandle_t xHandleBlink = NULL;

/**
 * @brief Task function to blink the green LED every 500 ms (500 ms ON, 500 ms OFF)
 * @param pvParameters Pointer to task parameters (unused)
 */
void TaskBlink(void *pvParameters) {
  (void) pvParameters; // Explicitly cast unused parameter to void
  
  // Initialize the green LED pin as output
  pinMode(LED_GREEN, OUTPUT);
  
  // Infinite task loop
  while (1) {
    digitalWrite(LED_GREEN, HIGH);           // Turn green LED ON
    vTaskDelay(500 / portTICK_PERIOD_MS);    // Delay for 500 ms
    
    digitalWrite(LED_GREEN, LOW);            // Turn green LED OFF
    vTaskDelay(500 / portTICK_PERIOD_MS);    // Delay for 500 ms
  }
}

/**
 * @brief Task function to monitor emergency button and handle emergency state.
 *        When emergency button is pressed, suspend blinking task, turn red LED ON.
 * @param pvParameters Pointer to task parameters (unused)
 */
void TaskEmergency(void *pvParameters) {
  (void) pvParameters;
  
  // Initialize emergency button input with internal pull-up resistor
  pinMode(BUTTON_EMERG, INPUT_PULLUP);
  
  // Initialize red LED pin as output
  pinMode(LED_RED, OUTPUT);
  
  // Infinite task loop
  while (1) {
    // Check if emergency button pressed (active low) and emergency not already triggered
    if (digitalRead(BUTTON_EMERG) == LOW && !emergency) {
      emergency = true;                      // Set emergency flag
      digitalWrite(LED_RED, HIGH);          // Turn red LED ON
      digitalWrite(LED_GREEN, LOW);         // Ensure green LED is OFF
      vTaskSuspend(xHandleBlink);           // Suspend the green LED blinking task
      Serial.println("🛑 EMERGENCY ACTIVATED");
    }
    vTaskDelay(50 / portTICK_PERIOD_MS);    // Poll button every 50 ms
  }
}

/**
 * @brief Task function to monitor start button and handle starting/resuming system.
 *        Starts or resumes blinking if system is not in emergency and not already started.
 * @param pvParameters Pointer to task parameters (unused)
 */
void TaskStart(void *pvParameters) {
  (void) pvParameters;
  
  // Initialize start button input with internal pull-up resistor
  pinMode(BUTTON_START, INPUT_PULLUP);
  
  // Infinite task loop
  while (1) {
    if (digitalRead(BUTTON_START) == LOW && !emergency && !systemStarted) {
      systemStarted = true;                  // Set system started flag
      vTaskResume(xHandleBlink);             // Resume blinking task
      Serial.println("✅ SYSTEM STARTED");
    }
    vTaskDelay(50 / portTICK_PERIOD_MS);    // Poll button every 50 ms
  }
}

/**
 * @brief Task function to monitor stop button and handle stopping the system.
 *        Stops blinking task and turns green LED off when stop button pressed.
 * @param pvParameters Pointer to task parameters (unused)
 */
void TaskStop(void *pvParameters) {
  (void) pvParameters;
  
  // Initialize stop button input with internal pull-up resistor
  pinMode(BUTTON_STOP, INPUT_PULLUP);
  
  // Infinite task loop
  while (1) {
    if (digitalRead(BUTTON_STOP) == LOW && !emergency && systemStarted) {
      digitalWrite(LED_GREEN, LOW);          // Turn green LED OFF
      vTaskSuspend(xHandleBlink);             // Suspend blinking task
      systemStarted = false;                   // Reset system started flag
      Serial.println("⏹️ SYSTEM STOPPED");
    }
    vTaskDelay(50 / portTICK_PERIOD_MS);      // Poll button every 50 ms
  }
}

/**
 * @brief Arduino setup function - initializes serial communication, creates FreeRTOS tasks,
 *        and suspends blinking task initially.
 */
void setup() {
  Serial.begin(9600);
  while (!Serial); // Wait for Serial to be ready
  
  // Create task to blink green LED with low priority
  xTaskCreate(
    TaskBlink,           // Task function pointer
    "Blink",             // Task name for debugging
    128,                 // Stack size (in words, depends on architecture)
    NULL,                // Parameters to pass to the task
    1,                   // Task priority (1 = low)
    &xHandleBlink        // Task handle (used for suspend/resume)
  );
  
  // Create emergency button monitoring task with highest priority
  xTaskCreate(
    TaskEmergency,       // Task function pointer
    "Emergency",         // Task name for debugging
    96,                  // Stack size
    NULL,                // Parameters
    3,                   // Highest priority
    NULL                 // No task handle needed
  );
  
  // Create start button monitoring task with medium priority
  xTaskCreate(
    TaskStart,           // Task function pointer
    "Start",             // Task name
    96,                  // Stack size
    NULL,                // Parameters
    2,                   // Medium priority
    NULL                 // No task handle needed
  );
  
  // Create stop button monitoring task with medium priority
  xTaskCreate(
    TaskStop,            // Task function pointer
    "Stop",              // Task name
    96,                  // Stack size
    NULL,                // Parameters
    2,                   // Medium priority
    NULL                 // No task handle needed
  );
  
  Serial.println("PriorityTaskAPI Control RTOS Starting...");
  
  // Initially suspend blinking task until started
  vTaskSuspend(xHandleBlink);
}

/**
 * @brief Arduino loop function - unused because FreeRTOS manages tasks
 */
void loop() {
  // Empty; FreeRTOS tasks handle all operations
}
