#ifndef __WIFI_MODULE_H__
#define __WIFI_MODULE_H__

#include <Arduino.h>
#include <WiFi.h>

#define WIFI_SSID           "onlyPhone"
#define WIFI_PASSWORD       "DSA2023A"


void initWifi(void* ptrParameter);
void taskCheckWifiConnection(void* ptrParameter);


#endif

