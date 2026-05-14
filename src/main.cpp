#include <Arduino.h>

const int PWM_CHANNEL=0;
const int PWM_FREQ=5000;
const int PWM_RESOLUTION=8;  //0-255
const int LED_PIN=2;

volatile int shared_breath_step;  //Prevents compiler optimization

//Task A: State Controller
void Task_Controller(void* pvParameters)
{
 for (;;)
 {
  //Simulate State 1: Slow breathing
   shared_breath_step=1;
   Serial.println("[Controller] Set breathing speed to SLOW");
   vTaskDelay(5000/portTICK_PERIOD_MS);
   //Simulate State 2: Fast breathing
   shared_breath_step=5;
   Serial.println("[Controller] Set breathing speed to FAST");
    vTaskDelay(1000/portTICK_PERIOD_MS);
 }
}

//Task B: Breathing Light Driver
//Execute breathing logic based on global variable
void Task_BreathingLight(void* pvParameters)
{
  //Initialize PWM 
  ledcSetup(PWM_CHANNEL,PWM_FREQ,PWM_RESOLUTION);
    ledcAttachPin(LED_PIN,PWM_CHANNEL);
    int dutyCycle=0;
    bool fadeUp=true;
  
    for (;;)
    {
      ledcWrite(PWM_CHANNEL,dutyCycle);
      //Read shared variable for step size
      if (fadeUp)
      {
        dutyCycle=shared_breath_step+dutyCycle;
        if (dutyCycle>255)
        {
          dutyCycle=255;
          fadeUp=false;
        }
      }else{
        dutyCycle=dutyCycle-shared_breath_step;
        if (dutyCycle<0)
        {
          dutyCycle=0;
          fadeUp=true;
        }
        
      }
      
    
    //Base delay for smoothness
    vTaskDelay(10/portTICK_PERIOD_MS);}
  
}

void setup() {
  Serial.begin(115200);
  //Wait for Serial stability
  vTaskDelay(1000/portTICK_PERIOD_MS);

  Serial.println("---Shared Variable Communication Start---");

  //Blind controller task to Core 0
  xTaskCreatePinnedToCore(
    Task_Controller,
    "ControllerTask",
    2048,
    NULL,
    1,
    NULL,
    0
  );

  //Blind breathingDriver task to Core 1
  xTaskCreatePinnedToCore(
    Task_BreathingLight,
    "BreathTask",
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
