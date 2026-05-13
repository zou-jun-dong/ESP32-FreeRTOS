#include <Arduino.h>

/**
 * Task A: Simulating high-frequency sensor sampling
 * Runs on Core 0 
 */
void Task_Sensor(void* pvParameters)
{
 for (;;)
 {
   Serial.print("[Sensor] Reading on Core:");
   Serial.println(xPortGetCoreID());  //Get current Core ID
    vTaskDelay(1000/portTICK_PERIOD_MS);
 }
}

/**
 * Task B: Simulating complex logic computation
 * Runs on Core 1
 */
void Task_Logic(void* pvParameters)
{
  
  for (;;)
  {
    Serial.print("[Logic] Computing on Core: ");
    Serial.println(xPortGetCoreID());
    vTaskDelay(1500/portTICK_PERIOD_MS);
  }  
}

void setup() {
  Serial.begin(115200);
  //Wait for Serial stability
  vTaskDelay(1000/portTICK_PERIOD_MS);

  Serial.println("---Starting Dual-Core Execution---");

  //Blind sensor task to Core 0
  xTaskCreatePinnedToCore(
    Task_Sensor,
    "SensorTask",
    2048,
    NULL,
    1,
    NULL,
    0
  );

  //Blind logic task to Core 1
  xTaskCreatePinnedToCore(
    Task_Logic,
    "LogicTask",
    2048,
    NULL,
    1,
    NULL,
    1
  );
}

void loop() {
  //Keep empty.The loop task runs on Core 1 by default
  vTaskDelete(NULL);
}
