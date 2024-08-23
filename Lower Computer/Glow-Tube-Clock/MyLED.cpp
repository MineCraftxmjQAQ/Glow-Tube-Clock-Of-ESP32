#include "FastLED.h"
#include "MyHC595.h"

#define LED_TYPE                  WS2812            //LED型号
#define WS2812B_DIN               16                //LED数据输入引脚
#define WS2812B_NUM               6                 //LED数量
#define WS2812B_MAX_BRIGHT        255               //LED亮度,范围0~255
#define WS2812B_COLOR_ORDER       GRB               //LED灯珠排列顺序

CRGB WS2812B_LIST[WS2812B_NUM];                     //LED灯对象
unsigned char hVal;                                 //色调数值
unsigned char target_hVal;                          //目标色调数值
unsigned char sVal;                                 //饱和度数值
unsigned char target_sVal;                          //目标饱和度数值
unsigned char vVal;                                 //明亮度数值
unsigned char target_vVal;                          //目标明亮度数值
unsigned char LED_LOOP;                             //LED变色循环值

/**
 * @brief     LED灯串初始化
 * @param     无
 * @return    无
 * @exception 无
 * @author    Achan Wang
 * @date      2024-7-28 18:00:00
 * @version   1.0
 * @property  无
 * */
void WS2812B_Init(void)
{
  LEDS.addLeds<LED_TYPE, WS2812B_DIN, WS2812B_COLOR_ORDER>(WS2812B_LIST, WS2812B_NUM);  //初始化LED灯对象
  FastLED.setBrightness(WS2812B_MAX_BRIGHT);                                            //设置LED灯亮度
  srand((unsigned)time(NULL));                                                          //LED初始化时尚未校准时间
  hVal = rand() % 256;                                                                  //随机第一个当前颜色
  srand((unsigned)time(NULL));                                                          //LED初始化时尚未校准时间
  target_hVal = rand() % 256;                                                           //随机第一个目标颜色
}

/**
 * @brief     LED灯串颜色控制
 * @param     无
 * @return    无
 * @exception 无
 * @author    Achan Wang
 * @date      2024-7-28 18:00:00
 * @version   1.0
 * @property  无
 * */
void WS2812B_ColorCtrl(void)
{
  if(hVal != target_hVal)             //当前颜色与目标颜色不同,每次循环使当前颜色逼近目标颜色
  {
    if(hVal > target_hVal)
    {
      hVal--;
    }
    else
    {
      hVal++;
    }
  }
  else                                //当前颜色与目标颜色相同,随机产生新的与当前颜色不同的目标颜色
  {
    srand((unsigned)time(&now));
    target_hVal = rand() % 256;
    while(hVal == target_hVal)
    {
      srand((unsigned)time(&now));
      target_hVal = rand() % 256;
    }
  }
  WS2812B_LIST[0] = CHSV(hVal, 255, 255);
  WS2812B_LIST[1] = CHSV(hVal, 255, 255);
  WS2812B_LIST[2] = CHSV(hVal, 255, 255);
  WS2812B_LIST[3] = CHSV(hVal, 255, 255);
  WS2812B_LIST[4] = CHSV(hVal, 255, 255);
  WS2812B_LIST[5] = CHSV(hVal, 255, 255);
  FastLED.show();
}