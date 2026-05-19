#include <Arduino.h>
typedef struct 
{
  uint32_t counter;
  uint8_t statusCode;
  uint8_t breathLightState;
}DataPacket;
QueueHandle_t xDataQueue;

//Task: MQTT Data Publishing
void Task_Sender(void* pvParameters)
{
  DataPacket packetToSend;
   packetToSend.counter=0;
   packetToSend.statusCode=0;
   packetToSend.breathLightState=0;
 for (;;)
 {
   packetToSend.counter++;
   packetToSend.breathLightState=(packetToSend.breathLightState+15)%255;
   BaseType_t xStatus=xQueueSend(xDataQueue,&packetToSend,(TickType_t)10);
   if (xStatus==pdPASS)
   {
    Serial.printf("[Sender] ENQUEUED -> Count: %lu | Status: %d | Light PWM: %d\n", packetToSend.counter, packetToSend.statusCode, packetToSend.breathLightState);
   }else{
    Serial.println("[Sender] WARNING: Queue is full! Data dropped.");
   }
   vTaskDelay(1000/portTICK_PERIOD_MS);
 }
}

void Task_Receiver(void *pvParameters)
{
  DataPacket receivedPacket;
  for (;;)
  {
    BaseType_t xStatus=xQueueReceive(xDataQueue,&receivedPacket,pdMS_TO_TICKS(2000));
    if (xStatus==pdPASS)
    {
      Serial.printf("[Receiver] DEQUEUED <- Count: %lu | Status: %d | Light PWM: %d\n",receivedPacket.counter,receivedPacket.statusCode,receivedPacket.breathLightState);
    }else{
      Serial.println("[Receiver] INFO: Waiting for data timeout. Queue is currently empty.");
    }
  }
}

void setup() {
  Serial.begin(115200);
  //Wait for Serial stability
  vTaskDelay(1000/portTICK_PERIOD_MS);
  //Initialize WiFi and MQTT
  Serial.println("--- FreeRTOS Producer-Consumer Queue Test ---");
  xDataQueue=xQueueCreate(10,sizeof(DataPacket));
  if (xDataQueue!=NULL)
  {
    
  xTaskCreatePinnedToCore(
    Task_Sender,
    "SenderTask",
    2048,
    NULL,
    1,
    NULL,
    1
  );

  xTaskCreatePinnedToCore(Task_Receiver,"RecievedTask",2048,NULL,1,NULL,0);

}else{
  Serial.println("System] FATAL ERROR: Failed to create queue.");
}

}

void loop() {
  //Keep empty.The loop task runs on Core 1 by default
  vTaskDelete(NULL);
}
