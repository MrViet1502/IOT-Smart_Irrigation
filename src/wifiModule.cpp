#include "../include/wifiModule.h"
#include <ThingsBoard.h>
#include "../include/schedulerEvent.h"

void initWifi(void* ptrParameter) 
{
  Serial.println("Connecting to Wifi ...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to Wifi");
}

void taskCheckWifiConnection(void* ptrParameter) 
{
  Serial.println("Connecting to  ...");
  initWifi(NULL);
  while (true)
  {
    if (OTAOnProgress == true)
    {
      Serial.println("OTA in progress, skip wifi check");
      vTaskDelay(10000);
      continue;
    }
    if (WiFi.status() != WL_CONNECTED)
      initWifi(NULL);
    // else
    //   Serial.println("Connected to Wifi");
    vTaskDelay(10000);
  }
}