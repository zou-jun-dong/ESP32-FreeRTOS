#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid="iPhone";
const char* password="zjd20061212";
const char* mqtt_server="192.168.2.81";

WiFiClient espClient;
PubSubClient client(espClient);


//Task: MQTT Data Publishing
void Task_MQTT_Publish(void* pvParameters)
{
 for (;;)
 {
   if (!client.connected())  //Check MQTT connection status
   {
    Serial.println("[MQTT]Disconnected.Reconnecting...");
    if (client.connect("ESP32_S3_Client"))  //Simple reconnect logic
    {
      Serial.println("[MQTT] Connected");
    }
   }

   if (client.connected())
   {
    //Builde JSON data payload
    String payload="{\"device\":\"ESP32-S3\", \"status\":\"active\", \"uptime\":" + String(millis() / 1000) + "}";
    client.publish("esp32/sensor/data",payload.c_str());   //Publish to specific topic
    Serial.print("[MQTT] Published:");
    Serial.println(payload);
   }
   client.loop();   //Maintain MQTT client heartbeat
   vTaskDelay(5000/portTICK_PERIOD_MS);  //Publish data every 5 seconds
 }
}

void setup() {
  Serial.begin(115200);
  //Wait for Serial stability
  vTaskDelay(1000/portTICK_PERIOD_MS);
  //Initialize WiFi and MQTT
  WiFi.begin(ssid,password);
  if (!WiFi.isConnected())
  {
    delay(500);
    Serial.print(".");
  }
  client.setServer(mqtt_server,1883);
  

  Serial.println("---Shared Variable Communication Start---");

  //Pin network task to Core 0,leaving Core 1 for business logic
  xTaskCreatePinnedToCore(
    Task_MQTT_Publish,
    "MQTTPubTask",
    4096,
    NULL,
    1,
    NULL,
    0
  );

}

void loop() {
  //Keep empty.The loop task runs on Core 1 by default
  vTaskDelete(NULL);
}
