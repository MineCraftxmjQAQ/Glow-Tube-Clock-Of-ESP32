#ifndef __MYWIFI_H__
#define __MYWIFI_H__

#include "WiFi.h"
#include <DNSServer.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <esp_wifi.h>
#include <nvs_flash.h>

boolean Internet_Init(void);
void Handle_Root(void);
void Handle_Config(void);
void Check_Request(void);
void APMode_Init(void);
void DNS_Init(void);
void WebServer_Init(void);
boolean WiFi_Scan(void);
void WiFi_Connect(void);
boolean Check_Connect(boolean reConnect);
void Timer(void);

#endif