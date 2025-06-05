#include "globalConfig.h"
#include <HTTPClient.h>
#include <Update.h>
#include <ArduinoJson.h>

String firmwareURL = "";
bool otaTriggered = false;
volatile bool otaInProgress = false;

void sendOTAStatus(String status)
{
    StaticJsonDocument<128> doc;
    doc["ota_status"] = status;
    char buffer[128];
    serializeJson(doc, buffer);
    client.publish("v1/devices/me/attributes", buffer);
    Serial.println("[OTA] Status sent to ThingsBoard: " + status);
}

bool OTA_Update(String url)
{
    HTTPClient http;
    http.begin(url);
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    int httpCode = http.GET();

    if (httpCode == 200)
    {
        int len = http.getSize();
        WiFiClient *stream = http.getStreamPtr();

        if (!Update.begin(len))
        {
            Serial.println("Update Begin Failed");
            return false;
        }

        size_t written = Update.writeStream(*stream);
        if (written == len)
        {
            Serial.println("Written: " + String(written) + " successfully");
        }
        else
        {
            Serial.println("Written only: " + String(written) + "/" + String(len) + ". Retry?");
            return false;
        }

        if (Update.end())
        {
            if (Update.isFinished())
            {
                Serial.println("Update successfully completed. Rebooting.");
                return true;
            }
            else
            {
                Serial.println("Update not finished? Something went wrong!");
                return false;
            }
        }
        else
        {
            Serial.println("Update.end() failed: " + String(Update.getError()));
            return false;
        }
    }
    else
    {
        Serial.printf("HTTP GET failed. Code: %d\n", httpCode);
        return false;
    }
}

void RunOTA_Update(void *pvParameters)
{
    for (;;)
    {
        if (otaTriggered && firmwareURL.length() > 0)
        {
            otaInProgress = true;
            Serial.println("[OTA] Starting update from: " + firmwareURL);

            if (OTA_Update(firmwareURL))
            {
                sendOTAStatus("OTA Update success");
                Serial.println("[OTA] Update successful! Rebooting...");
                vTaskDelay(2000 / portTICK_PERIOD_MS);
                esp_restart();
            }
            else
            {
                sendOTAStatus("OTA Update failed");
                Serial.println("[OTA] Update failed!");
            }

            otaTriggered = false;
            otaInProgress = false;
        }

        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}
