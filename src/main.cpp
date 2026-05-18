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

void setup() {
  Serial.begin(115200);
  //Wait for Serial stability
  vTaskDelay(1000/portTICK_PERIOD_MS);
  //Initialize WiFi and MQTT
  Serial.println("--- FreeRTOS Queue Initialization ---");
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

}else{
  Serial.println("--- FreeRTOS Queue Initialization ---");
}

}

void loop() {
  //Keep empty.The loop task runs on Core 1 by default
  vTaskDelete(NULL);
}
