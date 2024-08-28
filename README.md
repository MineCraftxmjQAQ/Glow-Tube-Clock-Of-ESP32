# Glow-Tube-Clock-Of-ESP32
本项目是使用ESP32为主控、辉光管为显示的数字时钟  
  
ESP32使用搭载ESP-WROOM-32的ESP32 DEVKIT V1开发板  
数显辉光管使用国产风光QS30-1圆形辉光管,工作电压170V,串接20K限流电阻  
时分秒间隔符使用俄罗斯INS-1顶显辉光管,工作电压170V,串接100K限流电阻  
  
时钟包含上电自动联网校时、AP网页配网、掉电配网信息保存、按钮显示模式切换等功能,以及由WS2812B灯组构建的随机平滑过度的RGB灯效  
  
PCB使用嘉立创EDA专业版设计,双面打板,厚度1.6mm
  
本项目长期开发,计划后续编写对辉光管显示内容设定及WS2812B灯效设定的上位机应用(基于.NET的桌面端和基于web的移动端)  
  
!!!注意:当前上传的PCB存在问题,8片74HC595级联后出现输出混乱,使用两片级联测试单个辉光管时没有出现问题,等待后续解决!!!
