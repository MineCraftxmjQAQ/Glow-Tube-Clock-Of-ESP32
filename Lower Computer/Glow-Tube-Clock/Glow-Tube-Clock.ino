#include "FastLED.h"
#include "driver/mcpwm.h"
#include "WiFi.h"

#define LED_TYPE                  WS2812            //LED型号
#define WS2812B_DIN               16                //LED数据输入引脚
#define WS2812B_NUM               1                 //LED数量
#define WS2812B_MAX_BRIGHT        255               //LED亮度,范围0~255
#define WS2812B_COLOR_ORDER       GRB               //LED灯珠排列顺序

#define HC595_DS                  18                //HC595串行数据输入引脚
#define HC595_OE                  19                //HC595输出使能控制引脚
#define HC595_STCP                21                //HC595存储寄存器时钟引脚
#define HC595_SHCP                22                //HC595移位寄存器时钟引脚

/* HC595控制引脚函数别名 */
#define HC595_DS_Low()            digitalWrite(HC595_DS, LOW)
#define HC595_DS_High()           digitalWrite(HC595_DS, HIGH)
#define HC595_OE_Low()            digitalWrite(HC595_OE, LOW)
#define HC595_OE_High()           digitalWrite(HC595_OE, HIGH)
#define HC595_STCP_Low()          digitalWrite(HC595_STCP, LOW)
#define HC595_STCP_High()         digitalWrite(HC595_STCP, HIGH)
#define HC595_SHCP_Low()          digitalWrite(HC595_SHCP, LOW)
#define HC595_SHCP_High()         digitalWrite(HC595_SHCP, HIGH)

#define NeonLamp_Ctrl             17                //氖管控制引脚

CRGB WS2812B_LIST[WS2812B_NUM];                     //LED灯对象
unsigned char hVal;                                 //色调数值
unsigned char target_hVal;                          //目标色调数值
unsigned char sVal;                                 //饱和度数值
unsigned char target_sVal;                          //目标饱和度数值
unsigned char vVal;                                 //明亮度数值
unsigned char target_vVal;                          //目标明亮度数值
unsigned char LED_LOOP;                             //LED变色循环值

const char* ssid = "TP-LINK_2C6046";                //WIFI标识符
const char* password = "xywyj7152069127571516975";  //WIFI密码

const char* ntpServer = "pool.ntp.org";             //网络时间服务器
const long gmtOffset_sec = 8 * 3600;                //设置时区为东八区
const int daylightOffset_sec = 0;                   //非夏令时为0,夏令时为3600
time_t now;                                         //当前时间的时间戳
struct tm time_data;                                //当前时间的时间戳所对应的具体时间数据

int GlowTube_Clock_HH;                              //辉光钟的时高位
int GlowTube_Clock_HL;                              //辉光钟的时低位
int GlowTube_Clock_MH;                              //辉光钟的分高位
int GlowTube_Clock_ML;                              //辉光钟的分低位
int GlowTube_Clock_SH;                              //辉光钟的秒高位
int GlowTube_Clock_SL;                              //辉光钟的秒低位

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
  FastLED.show();
}

/**
 * @brief     HC595初始化
 * @param     无
 * @return    无
 * @exception 无
 * @author    Achan Wang
 * @date      2024-7-28 18:00:00
 * @version   1.0
 * @property  无
 * */
void HC595_Init(void)
{
  pinMode(HC595_DS, OUTPUT);
  pinMode(HC595_OE, OUTPUT);
  pinMode(HC595_STCP, OUTPUT);
  pinMode(HC595_SHCP, OUTPUT);
  HC595_DS_Low();
  HC595_OE_High();                //HC595输出使能控制引脚为低电平使能,初始化拉高电平以失能HC595输出
  HC595_STCP_Low();
  HC595_SHCP_Low();
}

/**
 * @brief     HC595发送一个字节数据
 * @param     无符号字符型
 * @return    无
 * @exception 无
 * @author    Achan Wang
 * @date      2024-7-28 18:00:00
 * @version   1.0
 * @property  无
 * */
void HC595_SendByte(unsigned char byte)
{
	unsigned char i;
	for (i = 0; i < 8; i++)
	{
		if(byte & 0x80)       //HC595发送字符高位先行
    {
      HC595_DS_High();    //发送位为1时拉高HC595串行数据输入引脚电平
    }
		else
    {
      HC595_DS_Low();     //发送位为0时拉低HC595串行数据输入引脚电平
    }
    byte <<= 1;
		HC595_SHCP_High();    //拉高HC595移位寄存器时钟引脚产生上升沿,使HC595移位寄存器内的数据向后移动一位,同时可以接收新的一位
    delay(1);
    HC595_SHCP_Low();     //拉低HC595移位寄存器时钟引脚,为下一次移位做准备
	}
  HC595_STCP_High();      //拉高HC595存储寄存器时钟引脚产生上升沿,使HC595移位寄存器内的数据转移至存储寄存器中
  delay(1);
  HC595_STCP_Low();       //拉低HC595存储寄存器时钟引脚,一字节数据发送完成
}

/**
 * @brief     辉光钟控制
 * @param     无
 * @return    无
 * @exception 无
 * @author    Achan Wang
 * @date      2024-7-28 18:00:00
 * @version   1.0
 * @property  无
 * */
void GlowTube_Clock_Ctrl(void)
{
  time(&now);                                   //获取当前时间的时间戳
  time_data = *localtime(&now);                 //转换具体时间数据
  GlowTube_Clock_HH = time_data.tm_hour / 10;
  GlowTube_Clock_HL = time_data.tm_hour % 10;
  GlowTube_Clock_MH = time_data.tm_min / 10;
  GlowTube_Clock_ML = time_data.tm_min % 10;
  GlowTube_Clock_SH = time_data.tm_sec / 10;
  GlowTube_Clock_SL = time_data.tm_sec % 10;

  HC595_OE_High();                              //失能HC595输出

  HC595_OE_Low();                               //使能HC595输出
}

/**
 * @brief     时分秒间隔符(氖管)初始化
 * @param     无
 * @return    无
 * @exception 无
 * @author    Achan Wang
 * @date      2024-7-28 18:00:00
 * @version   1.0
 * @property  无
 * */
void NeonLamp_Init(void)
{
  mcpwm_config_t mcpwmConfig;
  pinMode(NeonLamp_Ctrl, OUTPUT);
  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, NeonLamp_Ctrl);
  mcpwmConfig.frequency = 10;
  mcpwmConfig.cmpr_a = 10;
  mcpwmConfig.cmpr_b = 0;
  mcpwmConfig.counter_mode = MCPWM_UP_COUNTER;
  mcpwmConfig.duty_mode = MCPWM_DUTY_MODE_0;
  mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &mcpwmConfig);
}

/**
 * @brief     网络连接初始化
 * @param     无
 * @return    布尔型 返回true表示WiFi连接成功,返回false表示WiFi连接超时
 * @exception 无
 * @author    Achan Wang
 * @date      2024-7-28 18:00:00
 * @version   1.0
 * @property  无
 * */
boolean Internet_Init(void)
{
  unsigned char Connect_Timeout = 0;
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Connect_Timeout++;
    if(Connect_Timeout > 9)
    {
      return false;             //一秒检查一次网络连接状态,10秒未成功连接则判定超时
    }
  }
  if(WiFi.status() == WL_CONNECTED)
  {
    return true;
  }
}

void setup()
{
  WS2812B_Init();
  HC595_Init();
  NeonLamp_Init();
  if(Internet_Init() == false)
  {
  }
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);   //上电校时,校时时长约两分钟

  Serial.begin(115200);
}

void loop()
{
  WS2812B_ColorCtrl();
  GlowTube_Clock_Ctrl();
  delay(500);
}