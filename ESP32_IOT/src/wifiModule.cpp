#include "globalConfig.h"

const char *ssid = "viet";
const char *password = "20252025";
WiFiClient espClient;

PubSubClient client(espClient);

void checkWifiTask(void *pvParameters)
{
    for (;;)
    {
        if (otaInProgress) // Nếu đang cập nhật OTA thì không kiểm tra WiFi
        {
            Serial.println("OTA rebooting ...");
            vTaskDelay(10000 / portTICK_PERIOD_MS);
            continue;
        }

        if (WiFi.status() != WL_CONNECTED)
        {
            Serial.println("WiFi disconnected! Reconnecting...");
            WiFi.begin(ssid, password);
            unsigned long start = millis();
            while (WiFi.status() != WL_CONNECTED && millis() - start < 10000)
            {
                vTaskDelay(500 / portTICK_PERIOD_MS);
                Serial.print(".");
            }

            if (WiFi.status() == WL_CONNECTED)
            {
                Serial.print("\nWiFi connected: ");
                Serial.println(WiFi.localIP());
            }
            else
            {
                Serial.println("\nWiFi reconnect failed.");
            }
        }
        else
        {
            // Để tránh log spam: có thể chỉ in khi lần đầu kết nối thành công
            static bool loggedOK = false;
            if (!loggedOK)
            {
                Serial.print("WiFi OK, IP: ");
                Serial.println(WiFi.localIP());
                loggedOK = true;
            }
        }

        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
}
