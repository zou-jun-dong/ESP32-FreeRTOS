#include <Arduino.h>

SemaphoreHandle_t xSerialMutex=NULL;  //Global declaration of Mutex Handel

//Task A:Simulating high-frequency Serial usage
void Task_PrintA(void* pvParameters)
{
 for (;;)
 {
    if (xSemaphoreTake(xSerialMutex,portMAX_DELAY)==pdTRUE)
    {
      Serial.print("[Task A] ");
      Serial.print("Data packet: ");
      Serial.print("10101010");
      Serial.println(" -> Transaction Complete.");
      xSemaphoreGive(xSerialMutex);   //Must release the lock immediately after using the shared resource
    }
    vTaskDelay(10/portTICK_PERIOD_MS);
 }
}

//Task B:Simulating high-frequency Serial usage
void Task_PrintB(void* pvParameters)
{
  for (;;)
  {
    if (xSemaphoreTake(xSerialMutex,portTICK_PERIOD_MS))
    {
      Serial.print("[Task B] ");
      Serial.print("WARNING! ");
      Serial.print("Sensor reading is abnormal.");
      Serial.println(" -> Alert Sent.");
      xSemaphoreGive(xSerialMutex);  
    }
    vTaskDelay(10/portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(115200);
  //Wait for Serial stability
  vTaskDelay(1000/portTICK_PERIOD_MS);

  Serial.println("--- FreeRTOS Mutex Peripheral Protection Test ---");

  xSerialMutex=xSemaphoreCreateMutex();  //Create a Mutex
  if (xSerialMutex!=NULL)
  {
    //Create task with high priority to ensure real-time responsiveness
    xTaskCreatePinnedToCore(Task_PrintA,"PrintATask",2048,NULL,1,NULL,1); 
    xTaskCreatePinnedToCore(Task_PrintB,"PrintBTask",2048,NULL,1,NULL,1);  
  Serial.println("[System] Mutex created.Task started.");
  }else{
    Serial.println("[System] FATAL ERROR; Mutex creation failed.");
  }
}

void loop() {
  //Keep empty.The loop task runs on Core 1 by default
  vTaskDelete(NULL);
}
