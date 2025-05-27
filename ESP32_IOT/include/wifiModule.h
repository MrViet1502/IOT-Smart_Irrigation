#ifndef __WIFI_MODULE_H__
#define __WIFI_MODULE_H__

#include <Arduino.h>
#include <WiFi.h>

#define WIFI_SSID "viet"
#define WIFI_PASSWORD "20252025"

void initWifi(void *ptrParameter);
void taskCheckWifiConnection(void *ptrParameter);

#endif
