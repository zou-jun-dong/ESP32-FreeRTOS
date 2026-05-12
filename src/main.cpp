#include <Arduino.h>

//Priority Definitions
#define HIGH_PRIO 3
#define LOW_PRIO 1

//Task A: High Priority Monitor
void Task_High(void* pvParameters)
{
 for (;;)
 {
   Serial.println("[HIGH] priority task is running");
   //Crucial: High priority task must sleep to let lower priority tasks run
    vTaskDelay(1000/portTICK_PERIOD_MS);
 }
}

//Task B: Low Priority Preemption Test
void Task_Low(void* pvParameters)
{
  
  long count=0;
  for (;;)
  {
    count++;
    //Simulating heavy computation without delay
    if (count%1000000==0)
    {
      Serial.println("[LOW] I am doing heavy calculations...");
    }
    //If you uncomment the line below,the system resumes normal scheduling
    //vTaskDelay(1/portTICK_PERIOD_MS);
  }  
}

void setup() {
  Serial.begin(115200);
  //Wait for Serial stability
  vTaskDelay(1000/portTICK_PERIOD_MS);

  Serial.println("---Priority & Preemption Test Start---");

  //Create High Priority Task
  xTaskCreatePinnedToCore(
    Task_High,
    "HIGHTask",
    2048,
    NULL,
    HIGH_PRIO,
    NULL,
    1
  );

  //Create Low Priority Task
  xTaskCreatePinnedToCore(
    Task_Low,
    "LOWTask",
    2048,
    NULL,
    LOW_PRIO,
    NULL,
    1
  );
}

void loop() {
  //Keep empty
}
