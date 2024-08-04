#include <Arduino.h>

#define NeonLamp_Ctrl             17                //氖管控制引脚

/* 氖管控制引脚函数别名 */
#define NeonLamp_Low()            digitalWrite(NeonLamp_Ctrl, LOW)
#define NeonLamp_High()           digitalWrite(NeonLamp_Ctrl, HIGH)

/**
 * @brief     时分秒间隔符(氖管)控制线程
 * @param     无
 * @return    无
 * @exception 无
 * @author    Achan Wang
 * @date      2024-8-3 18:00:00
 * @version   1.0
 * @property  无
 * */
void NeonLamp_Task(void* pvParameters)
{
  pinMode(NeonLamp_Ctrl, OUTPUT);
  while(1)
  {
    NeonLamp_Low();
    delay(1000);
    NeonLamp_High();
    delay(1000);
  }
}