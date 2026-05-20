#include <Arduino.h>
const int BUTTON_PIN=0;  //Define button pin,BOOT button is usually on GPIO 0
SemaphoreHandle_t xButtonSemaphore=NULL;   //Declare a binary semaphore handle

/**
 * Interrupt Service Rountine-ISR
 * IRAM_ATTR forces this function to run from RAM for maximum speed
 */
void IRAM_ATTR button_isr() {
  //In a interrupt context,must use APIs with the 'FromISR' suffix!
  BaseType_t xHigherPriorityTaskWoken=pdFALSE;
  xSemaphoreGiveFromISR(xButtonSemaphore,&xHigherPriorityTaskWoken);  //Give the semaphore
  //Force a context switch if a higher priority task was woken
  if (xHigherPriorityTaskWoken==pdTRUE)
  {
    portYIELD_FROM_ISR();
  }
  
}
//Task: Receives semaphore and executes business logic
void Task_ButtonHandler(void* pvParameters)
{
 uint32_t pressCount=0;
 for (;;)
 {
  //Block and wait for the semaphore
  //portMAX_DELAY means waiting indefinitely without consuming CPU
   if (xSemaphoreTake(xButtonSemaphore,portMAX_DELAY))
   {
    pressCount++;
    Serial.println("=================================");
    Serial.printf("[Handler Task] Button Pressed!Count: %lu\n",pressCount);
    Serial.printf("[Handler Task] Core ID: %d\n",xPortGetCoreID());
    Serial.println("=================================");
    vTaskDelay(200/portTICK_PERIOD_MS);
   }
 }
}


void setup() {
  Serial.begin(115200);
  //Wait for Serial stability
  vTaskDelay(1000/portTICK_PERIOD_MS);

  Serial.println("--- FreeRTOS Semaphore Interrupt Test ---");
  /**
   * Initialize Binary Semaphore
   * A newly creater binary semaphore is 'empty' by default.
   */
  xButtonSemaphore=xSemaphoreCreateBinary();
  if (xButtonSemaphore!=NULL)
  {
    //Create task with high priority to ensure real-time responsiveness
    xTaskCreatePinnedToCore(Task_ButtonHandler,"btnTask",2048,NULL,3,NULL,1); 
  
  pinMode(BUTTON_PIN,INPUT_PULLUP);  //Configure button pin as pull-up input
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN),button_isr,FALLING);
  Serial.println("[System] Semaphore created and interrupt attached.");
  }else{
    Serial.println("[System] FATAL ERROR; Semaphore creation failer.");
  }
}

void loop() {
  //Keep empty.The loop task runs on Core 1 by default
  vTaskDelete(NULL);
}
