#include "../include/wifiModule.h"
#include <ThingsBoard.h>
#include "../include/schedulerEvent.h"
#include "../include/ledControl.h"
bool initWifi()
{
  Serial.print("Connecting to Wifi: ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && (millis() - startTime < 20000))
  {
    Serial.print(".");
    vTaskDelay(pdMS_TO_TICKS(500)); // FreeRTOS delay
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("\nConnected to Wifi. IP: " + WiFi.localIP().toString());
    return true;
  }
  else
  {
    Serial.println("\nWiFi connect timeout!");
    return false;
  }
}

void taskCheckWifiConnection(void *ptrParameter)
{
  Serial.println("Connecting to Wifi ...");
  initWifi(); // Gọi đúng
  while (true)
  {
    if (OTAOnProgress == true)
    {
      Serial.println("OTA in progress, skip wifi check");
      vTaskDelay(pdMS_TO_TICKS(10000));
      continue;
    }

    if (WiFi.status() != WL_CONNECTED)
    {
      Serial.println("WiFi disconnected. Reconnecting...");
      initWifi();
    }

    vTaskDelay(pdMS_TO_TICKS(10000));
  }
}
