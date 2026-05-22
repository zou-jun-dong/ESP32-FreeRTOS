#include <Arduino.h>

//Declare two Mutexes
SemaphoreHandle_t xMutexA=NULL;
SemaphoreHandle_t xMutexB=NULL;

//Task 1:Take A,then B
void Task_Print1(void* pvParameters)
{
 for (;;)
 {
  Serial.println("[Task 1] Attempting to take Mutex A...");
    if (xSemaphoreTake(xMutexA,portMAX_DELAY)==pdTRUE)
    {
      Serial.println("[Task 1] Successfully took Mutex A!");
      //Intentional delay to yield CPU,allowing Task 2 to grab Mutex B
      vTaskDelay(50/portTICK_PERIOD_MS);
      Serial.println("[Task 1] Attempting to take Mutex B...");
      //This is will block forever because Mutex B is held by Task2
      if (xSemaphoreTake(xMutexB,portMAX_DELAY)==pdTRUE)
      {
        Serial.println("[Task 1] Got both! Working...");
        xSemaphoreGive(xMutexB);
      }
      xSemaphoreGive(xMutexA);
    }
    vTaskDelay(100/portTICK_PERIOD_MS);
 }
}

//Task 1:Take B,then A
void Task_Print2(void* pvParameters)
{
  for (;;)
  {
    if (xSemaphoreTake(xMutexB,portTICK_PERIOD_MS)==pdTRUE)
    {
      Serial.println("[Task 2] Successfully took Mutex B!");
      //Intentional delay to yield CPU,allowing Task 1 to grab Mutex A
      vTaskDelay(50/portTICK_PERIOD_MS);

      Serial.println("[Task 2] Attempting to take Mutex A...");
      //This will block forever becaues Mutex A is held by Task 1
      if (xSemaphoreTake(xMutexA,portMAX_DELAY)==pdTRUE)
      {
         Serial.println("[Task 2] Got both! Working...");
         xSemaphoreGive(xMutexA);
      }
      xSemaphoreGive(xMutexB);     
    }
    vTaskDelay(100/portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(115200);
  //Wait for Serial stability
  vTaskDelay(1000/portTICK_PERIOD_MS);

  Serial.println("--- FreeRTOS Deadlock Simulation Start ---");

  //Create two Mutexes
  xMutexA=xSemaphoreCreateMutex();
  xMutexB=xSemaphoreCreateMutex();
  
  if (xMutexA!=NULL&&xMutexB!=NULL)
  {
    //Create two tasks,same core,same priority
    xTaskCreatePinnedToCore(Task_Print1,"Print1Task",2048,NULL,1,NULL,1); 
    xTaskCreatePinnedToCore(Task_Print2,"Print2Task",2048,NULL,1,NULL,1);  
  Serial.println("[System] Mutex created.Task started.");
  }
}

void loop() {
  //Keep empty.The loop task runs on Core 1 by default
  vTaskDelete(NULL);
}
