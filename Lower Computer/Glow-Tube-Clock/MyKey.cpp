#include <stdint.h>
#include "MyKey.h"
#include "MyHC595.h"
#include "MyNeonLamp.h"
#include "MyWiFi.h"

#define BUTTON_1                  39
#define BUTTON_1_FIX              32                //由于PCB设计问题,BUTTON_1的上拉由GPIO32提供
#define BUTTON_2                  25
#define BUTTON_3                  13

uint8_t state;                                      //按键状态机

/**
 * @brief     按键初始化
 * @param     无
 * @return    无
 * @exception 无
 * @author    Achan Wang
 * @date      2024-8-15 18:00:00
 * @version   1.1
 * @property  无
 * */
void Key_Init(void)
{
  pinMode(BUTTON_1, INPUT);
  pinMode(BUTTON_1_FIX, OUTPUT);
  digitalWrite(BUTTON_1_FIX, HIGH);
  pinMode(BUTTON_2, INPUT_PULLUP);
  pinMode(BUTTON_3, INPUT_PULLUP);
  state = 0;
}

/**
 * @brief     按键控制
 * @param     无
 * @return    无
 * @exception 无
 * @author    Achan Wang
 * @date      2024-8-16 18:00:00
 * @version   1.0
 * @property  无
 * */
void Key_Ctrl(void)
{
  if(digitalRead(BUTTON_1) == LOW)        //重新校时
  {
    delay(10);
    if(digitalRead(BUTTON_1) == LOW)
    {
      Timer();
    }

  }
  else if(digitalRead(BUTTON_2) == LOW)   //切换显示模式
  {
    delay(10);
    if(digitalRead(BUTTON_2) == LOW)
    {
      if(GlowTube_Clock_State == 0)
      {
        GlowTube_Clock_State = 1;
      }
      else if(GlowTube_Clock_State == 1)
      {
        GlowTube_Clock_State = 0;
      }
    }
  }
  else if(digitalRead(BUTTON_3) == LOW)   //切换闪烁速度
  {
    delay(10);
    if(digitalRead(BUTTON_3) == LOW)
    {
      if(Delay_Times >= 10000)
      {
        Delay_Times == 500;
      }
      else
      {
        Delay_Times += 500;
      }
    }
  }
}