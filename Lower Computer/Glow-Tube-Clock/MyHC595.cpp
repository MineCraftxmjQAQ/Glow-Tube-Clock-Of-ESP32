#include <Arduino.h>

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

time_t now;                                         //当前时间的时间戳
struct tm time_data;                                //当前时间的时间戳所对应的具体时间数据

char GlowTube_Clock_HH;                             //辉光钟的时高位
char GlowTube_Clock_HL;                             //辉光钟的时低位
char GlowTube_Clock_MH;                             //辉光钟的分高位
char GlowTube_Clock_ML;                             //辉光钟的分低位
char GlowTube_Clock_SH;                             //辉光钟的秒高位
char GlowTube_Clock_SL;                             //辉光钟的秒低位

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