#ifndef __MYWIFI_H__
#define __MYWIFI_H__

#include "WiFi.h"
#include <DNSServer.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <esp_wifi.h>
#include <nvs_flash.h>

boolean Internet_Init(void);
void Check_Request(void);
void WiFi_Connect(void);
boolean Check_Connect(boolean reConnect);
void Timer(void);

#endif