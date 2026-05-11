#include <Arduino.h>

/**
 * Task Handles
 * Used to control tasks later,such as deleting,suspending,or resuming
 */
TaskHandle_t TaskBlinkHandle=NULL;
TaskHandle_t TaskSerialHandle=NULL;

const int LED_PIN=2;  //Define LED Pin

//Task A:LED Blinking
void Task_Blink(void* pvParameters)
{
  /*Task Setup*/
  pinMode(LED_PIN,OUTPUT);

  /*Task Infinite Loop*/
  for (;;)
  {
    digitalWrite(LED_PIN,HIGH);
    vTaskDelay(500/portTICK_PERIOD_MS);
    digitalWrite(LED_PIN,LOW);
    vTaskDelay(500/portTICK_PERIOD_MS);
  }
}

//Task B:System Status Print
void Task_Serial(void* pvParameters)
{
  for(;;){
  Serial.print("[INFO] System running on Core: ");
  Serial.print(xPortGetCoreID()); //Get the core ID
  Serial.print("|Uptime:");
  Serial.print(millis()/1000);
  Serial.print("s");

  //Print every 2 seconds
  vTaskDelay(2000/portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(115200);
  //Wait for Serial stability
  vTaskDelay(1000/portTICK_PERIOD_MS);

  Serial.println("Creating FreeRTOS Tasks...");

  /**
   * 1.Task_Blink:Function pointer
   * 2."Blink":Debug name
   * 3.2048:Stack size in bytes
   * 4.NULL:Parameters passed to task
   * 5.1:priority,higher is more urgent
   * 6.&TaskBlinkHandle:Handle Reference
   * 7.Core ID: 0 or 1
   */
  xTaskCreatePinnedToCore(
    Task_Blink,
    "BlinkTask",
    2048,
    NULL,
    1,
    &TaskBlinkHandle,
    1
  );

  xTaskCreatePinnedToCore(
    Task_Serial,
    "SerialTask",
    2048,
    NULL,
    1,
    &TaskSerialHandle,
    1
  );

  Serial.println("Tasks started successfully");
}

void loop() {
  /**
   * In RTOS,loop() runs in the priorty 1 IDLE task.
   * Better to keep is empty and use dedicated tasks.
   */
}
