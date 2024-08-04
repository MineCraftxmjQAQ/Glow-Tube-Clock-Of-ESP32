#include "MyWiFi.h"

/* 网页元素 */
#define ROOT_HTML  "<!DOCTYPE html><html><head><title>WiFi</title><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"></head><style type=\"text/css\">.input{display: block; margin-top: 10px;}.input span{width: 100px; float: left; float: left; height: 36px; line-height: 36px;}.input input{height: 30px;width: 200px;}.btn{width: 120px; height: 35px; background-color: #000000; border:0px; color:#ffffff; margin-top:15px; margin-left:100px;}</style><body><form method=\"POST\" action=\"configwifi\"><label class=\"input\"><span>WiFi SSID</span><input type=\"text\" name=\"ssid\" value=\"\"></label><label class=\"input\"><span>WiFi PASS</span> <input type=\"text\"  name=\"pass\"></label><input class=\"btn\" type=\"submit\" name=\"submit\" value=\"Submie\"> <p><span> Nearby wifi:</P></form>"

const char* ntpServer = "pool.ntp.org";             //网络时间服务器
const long gmtOffset_sec = 8 * 3600;                //设置时区为东八区
const int daylightOffset_sec = 0;                   //非夏令时为0,夏令时为3600

const char* AP_SSID  = "Glow-Tube-Clock-Config";    //AP标识符
const char* HOST_NAME = "Glow-Tube-Clock_ESP32";    //设备名
const byte DNS_PORT = 53;                           //DNS端口号
const int webPort = 80;                             //Web端口号

IPAddress apIP(192, 168, 4, 1);                     //AP的IP地址
DNSServer dnsServer;                                //创建DnsServer实例
WebServer server(webPort);                          //开启Web服务

String scanNetworksID = "";                         //储存扫描到的WiFi标识符

String ssid = "TP-LINK_2C6046";                     //WiFi标识符
String password = "xywyj7152069127571516975";       //WiFi密码

String save_ssid;                                   //用于保存的WiFi标识符
String save_password;                               //用于保存的WiFi密码

nvs_handle wifi_nvs_handle;                         //NVS操作句柄

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
  unsigned char Connect_Timeout = 0;                                                //超时计数
  String ssid_temp;                                                                 //临时WiFi标识符
  String password_temp;                                                             //临时WiFi密码
  char ssid_nvs_temp[33];                                                           //WiFi标识符长度范围0~32
  char password_nvs_temp[64];                                                       //AES和TKIP加密的WiFi密码最大长度63
  size_t ssid_size = 32;                                                            //WiFi标识符长度
  size_t password_size = 63;                                                        //WiFi密码长度
  esp_err_t ret = nvs_flash_init();                                                 //NVS初始化
  if(ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    nvs_flash_erase();                                                              //NVS出现错误,执行擦除
    nvs_flash_init();                                                               //尝试再次初始化
  }
  if(nvs_open("WiFi_Config", NVS_READWRITE, &wifi_nvs_handle) == ESP_OK)            //打开命名空间"WiFi_Config"
  {                                
    nvs_get_str(wifi_nvs_handle, "ssid", ssid_nvs_temp, &ssid_size);                //获取当前命名空间中的键名为"ssid"的值
    nvs_get_str(wifi_nvs_handle, "password", password_nvs_temp, &password_size);    //获取当前命名空间中的键名为"password"的值
    ssid_temp = ssid_nvs_temp;
    password_temp = password_nvs_temp;
    if(ssid_temp.isEmpty() == false && password_temp.isEmpty() == false)            //如果存储的ssid和password均不为空,则使用其尝试连接WiFi,否则使用默认ssid和password
    {
      ssid = ssid_temp;
      password = password_temp;
    }
  }
  nvs_close(wifi_nvs_handle);                                                       //关闭命名空间"WiFi_Config"
  WiFi.begin(ssid, password);                                                       //开始连接WiFi
  while(WiFi.status() != WL_CONNECTED)                                              //WiFi连接状态不为WL_CONNECTED(已连接)时
  {
    delay(1000);
    Connect_Timeout++;
    if(Connect_Timeout > 9)
    {
      return false;                                                                 //一秒检查一次网络连接状态,10秒未成功连接则判定超时
    }
  }
  if(WiFi.status() == WL_CONNECTED)                                                 //WiFi连接状态为WL_CONNECTED(已连接)
  {
    return true;
  }
}

/**
 * @brief     网站根目录的访问请求处理
 * @param     无
 * @return    无
 * @exception 无
 * @author    CSDN喜暖知寒,Achan Wang修改
 * @date      2024-8-4 20:00:00
 * @version   1.0
 * @property  无
 * */
void Handle_Root(void)
{
  if(server.hasArg("selectSSID"))
  {
    server.send(200, "text/html", ROOT_HTML + scanNetworksID + "</body></html>");
  }
  else
  {
    server.send(200, "text/html", ROOT_HTML + scanNetworksID + "</body></html>");
  }
}

/**
 * @brief     提交数据后的显示页面
 * @param     无
 * @return    无
 * @exception 无
 * @author    CSDN喜暖知寒,Achan Wang修改
 * @date      2024-8-4 20:00:00
 * @version   1.0
 * @property  无
 * */
void Handle_Config(void)
{
  if(server.hasArg("ssid"))
  {
    ssid = server.arg("ssid");
    save_ssid = ssid;                     //在WiFi_Connect函数中可能会清除ssid的值,因此在从网页获取报表数据后立即转存
  }
  else
  {
    server.send(200, "text/html", "<meta charset='UTF-8'>error, not found ssid");
    return;
  }
  if(server.hasArg("pass"))
  {
    password = server.arg("pass");
    save_password = password;             //在WiFi_Connect函数中可能会清除password的值,因此在从网页获取报表数据后立即转存
  }
  else
  {
    server.send(200, "text/html", "<meta charset='UTF-8'>error, not found password");
    return;
  }
  server.send(200, "text/html", "<meta charset='UTF-8'>SSID:" + ssid + "<br />password:" + password + "<br />已取得WiFi信息,正在尝试连接,请手动关闭此页面。");
  delay(2000);
  WiFi.softAPdisconnect(true);            //关闭AP模式
  server.close();                         //关闭服务器
  WiFi.softAPdisconnect();                //空参调用,清除AP模式的网络名和密码
  if(WiFi.status() != WL_CONNECTED)
  {
    WiFi_Connect();
  }
}

/**
 * @brief     检测客户端DNS&HTTP请求
 * @param     无
 * @return    无
 * @exception 无
 * @author    CSDN喜暖知寒,Achan Wang修改
 * @date      2024-8-4 20:00:00
 * @version   1.0
 * @property  无
 * */
void Check_Request(void)
{
  dnsServer.processNextRequest();
  server.handleClient();
}

/**
 * @brief     进入AP模式
 * @param     无
 * @return    无
 * @exception 无
 * @author    CSDN喜暖知寒,Achan Wang修改
 * @date      2024-8-4 20:00:00
 * @version   1.0
 * @property  无
 * */
void APMode_Init(void)
{
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  if(WiFi.softAP(AP_SSID) == 0)
  {
    ESP.restart();
  }
}

/**
 * @brief     开启DNS服务器
 * @param     无
 * @return    无
 * @exception 无
 * @author    CSDN喜暖知寒,Achan Wang修改
 * @date      2024-8-4 20:00:00
 * @version   1.0
 * @property  无
 * */
void DNS_Init(void)
{
  if(dnsServer.start(DNS_PORT, "*", apIP) == 0)
  {
    ESP.restart();
  }
}

/**
 * @brief     初始化DNS服务器
 * @param     无
 * @return    无
 * @exception 无
 * @author    CSDN喜暖知寒,Achan Wang修改
 * @date      2024-8-4 20:00:00
 * @version   1.0
 * @property  无
 * */
void WebServer_Init(void)
{
  server.on("/", HTTP_GET, Handle_Root);
  server.on("/configwifi", HTTP_POST, Handle_Config);
  server.onNotFound(Handle_Root);
  server.begin();
}

/**
 * @brief     扫描附近WiFi并显示
 * @param     无
 * @return    布尔型 返回true表示已扫描到WiFi,返回false表示未扫描到WiFi
 * @exception 无
 * @author    CSDN喜暖知寒,Achan Wang修改
 * @date      2024-8-4 20:00:00
 * @version   1.0
 * @property  无
 * */
boolean WiFi_Scan(void)
{
  int n = WiFi.scanNetworks();
  if(n == 0)
  {
    scanNetworksID = "no networks found";
    return false;
  }
  else
  {
    for(int i = 0; i < n; ++i)
    {
      scanNetworksID += "<P>" + WiFi.SSID(i) + "</P>";
      delay(10);
    }
    return true;
  }
}

/**
 * @brief     连接WiFi
 * @param     无
 * @return    无
 * @exception 无
 * @author    CSDN喜暖知寒,Achan Wang修改
 * @date      2024-8-4 20:00:00
 * @version   1.0
 * @property  无
 * */
void WiFi_Connect(void)
{
  unsigned char Connect_Timeout = 0;                //超时计数
  WiFi.hostname(HOST_NAME);
  WiFi.mode(WIFI_STA);
  if(ssid != "")                                    //如果WiFi标识符不为空(可以没有密码)
  {
    WiFi.begin(ssid.c_str(), password.c_str());
    ssid = "";
    password = "";
  }
  else                                              //如果WiFi标识符为空则空参调用,尝试使用最后一次正常连接的参数连接WiFi
  {
    WiFi.begin();
  }
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Connect_Timeout++;
    if(Connect_Timeout > 9)                         //一秒检查一次网络连接状态,10秒未成功连接则判定超时
    {
      APMode_Init();                                //初始化AP模式
      DNS_Init();                                   //初始化DNS
      WebServer_Init();                             //初始化网络服务器
      WiFi_Scan();                                  //扫描附近WiFi
      return;                                       //判断超时后退出循环,等待网页填写报表
    }
  }
  if(WiFi.status() == WL_CONNECTED)
  {
    server.stop();                                  //终止服务器
  }
}

/**
 * @brief     检查WiFi是否连接
 * @param     布尔型 传入true表示需要重连,传入false表示不需要重连
 * @return    布尔型 返回true表示WiFi连接成功,返回false表示WiFi连接失败
 * @exception 无
 * @author    CSDN喜暖知寒,Achan Wang修改
 * @date      2024-8-4 20:00:00
 * @version   1.0
 * @property  无
 * */
boolean Check_Connect(boolean reConnect)
{
  if(WiFi.status() != WL_CONNECTED)                                               //WiFi连接失败
  {
    if(reConnect == true && WiFi.getMode() != WIFI_AP && WiFi.getMode() != WIFI_AP_STA) 
    {
      return false;
    }
  }
  else                                                                            //WiFi连接成功
  {
    esp_err_t ret = nvs_flash_init();
    if(ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
      nvs_flash_erase();                                                          //NVS出现错误，执行擦除
      nvs_flash_init();                                                           //重新尝试初始化
    }
    if(nvs_open("WiFi_Config", NVS_READWRITE, &wifi_nvs_handle) == ESP_OK)        //打开命名空间"WiFi_Config"
    {
      esp_err_t ret = nvs_set_str(wifi_nvs_handle, "ssid", save_ssid.c_str());    //写入当前命名空间中的键名为"ssid"的值
      ret = nvs_set_str(wifi_nvs_handle, "password", save_password.c_str());      //写入当前命名空间中的键名为"password"的值
      nvs_commit(wifi_nvs_handle);                                                //提交修改
    }
    nvs_close(wifi_nvs_handle);                                                   //关闭命名空间"WiFi_Config"
    return true;
  }
}

/**
 * @brief     联网校准时间
 * @param     无
 * @return    无
 * @exception 无
 * @author    Achan Wang
 * @date      2024-8-3 21:00:00
 * @version   1.0
 * @property  无
 * */
void Timer(void)
{
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);                       //上电校时,校时时长视具体情况变化,最长不超过两分钟
}