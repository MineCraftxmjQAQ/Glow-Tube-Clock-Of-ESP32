#include "MyLED.h"
#include "MyHC595.h"
#include "MyNeonLamp.h"
#include "MyWiFi.h"

TaskHandle_t Task0;                                 //核心0线程对象
TaskHandle_t Task1;                                 //核心1线程对象

/**
 * @brief     双线程初始化
 * @param     无
 * @return    无
 * @exception 无
 * @author    Achan Wang
 * @date      2024-8-3 18:00:00
 * @version   1.0
 * @property  无
 * */
void Task_Init(void)
{
  xTaskCreatePinnedToCore(NeonLamp_Task, "Task0", 10000, NULL, 1, &Task0, 0);   //核心0控制氖管亮灭
  delay(500);
  xTaskCreatePinnedToCore(Main_Task, "Task1", 10000, NULL, 1, &Task1, 1);       //核心1控制主函数
  delay(500);
}

/**
 * @brief     辉光管时钟主线程
 * @param     无
 * @return    无
 * @exception 无
 * @author    Achan Wang
 * @date      2024-8-3 18:00:00
 * @version   1.0
 * @property  无
 * */
void Main_Task(void* pvParameters)
{
  while(1)
  {
    WS2812B_ColorCtrl();
    GlowTube_Clock_Ctrl();
  }
}

void setup()
{
  Serial.begin(115200);
  WS2812B_Init();                         //LED灯串初始化
  HC595_Init();                           //HC595初始化
  if(Internet_Init() == false)            //如果网络连接初始化失败(即直接连接失败)
  {
    WiFi_Connect();                       //尝试重连,若仍失败则进入配网流程
    while(1)
    {
      Check_Request();
      if(Check_Connect(true) == true)     //配网成功
      {
        break;
      }
    }
  }
  Timer();
  Task_Init();
}

void loop()
{
}