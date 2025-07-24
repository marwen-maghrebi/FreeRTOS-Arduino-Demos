#include <Arduino.h>

#include <Arduino_FreeRTOS.h>
#include <queue.h>  // Required for using FreeRTOS queues

// Pin definitions
#define BUTTON_PIN 2
#define LED_PIN    8

// Declare a handle for the queue
QueueHandle_t xQueue = NULL;

/**
 * @brief Task function to read button state and send "ON"/"OFF" messages
 * @param pvParameters Pointer to task parameters (unused in this case)
 */
void TaskButton(void *pvParameters) {
  (void) pvParameters;  // Explicitly cast unused parameter to void

  int lastState = LOW;  // Store last button state

  // Infinite task loop
  while (1) {
    int currentState = digitalRead(BUTTON_PIN);

    // Detect state change
    if (currentState != lastState) {
      char message[4];

      if (currentState == HIGH) {
        strcpy(message, "ON");
      } else {
        strcpy(message, "OFF");
      }

      // Send the message to the queue
      if (xQueueSend(xQueue, &message, portMAX_DELAY) == pdPASS) {
        Serial.print("Sent: ");
        Serial.println(message);
      }

      lastState = currentState;  // Update last state
    }

    vTaskDelay(50 / portTICK_PERIOD_MS);  // Debounce delay
  }
}

/**
 * @brief Task function to receive messages and control LED accordingly
 * @param pvParameters Pointer to task parameters (unused in this case)
 */
void TaskLED(void *pvParameters) {
  (void) pvParameters;  // Explicitly cast unused parameter to void

  char receivedMessage[4];  // Buffer to hold received string

  // Infinite task loop
  while (1) {
    // Wait indefinitely for a message from the queue
    if (xQueueReceive(xQueue, receivedMessage, portMAX_DELAY) == pdPASS) {
      Serial.print("Received: ");
      Serial.println(receivedMessage);

      // Control LED based on message
      if (strcmp(receivedMessage, "ON") == 0) {
        digitalWrite(LED_PIN, HIGH);
      } else if (strcmp(receivedMessage, "OFF") == 0) {
        digitalWrite(LED_PIN, LOW);
      }
    }
  }
}

/**
 * @brief Arduino setup function - runs once at startup
 * Initializes pins, queue, and creates FreeRTOS tasks
 */
void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  while (!Serial);  // Wait for Serial to be ready (for boards like Leonardo)

  // Configure I/O pins
  pinMode(BUTTON_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);

  // Create a queue that can hold 5 string messages (up to 4 chars each)
  xQueue = xQueueCreate(5, sizeof(char) * 4);

  // Check if queue was created successfully
  if (xQueue == NULL) {
    Serial.println("Error creating the queue.");
    while (1);  // Halt execution
  }

  // Create the button task
  xTaskCreate(
    TaskButton,     // Task function
    "ButtonTask",   // Task name
    128,            // Stack size
    NULL,           // Task parameters
    1,              // Priority
    NULL            // Task handle
  );

  // Create the LED task
  xTaskCreate(
    TaskLED,        // Task function
    "LEDTask",      // Task name
    128,            // Stack size
    NULL,           // Task parameters
    1,              // Priority
    NULL            // Task handle
  );
}

/**
 * @brief Arduino loop function - not used when FreeRTOS scheduler is running
 * Required by Arduino IDE but remains empty as RTOS handles task execution
 */
void loop() {
  // Empty as FreeRTOS scheduler handles task execution
}
