#include "globalConfig.h"
#include "DHT20.h"
#include <ArduinoJson.h>
#include "../include/sensorsModule.h"

DHT20 dht20;
SemaphoreHandle_t i2cMutex = NULL;

const long telemetryInterval = 5000;
const long mq2Interval = 5000;

void exceptionHandler(void)
{
    digitalWrite(BUZZER_PIN, HIGH);
    Serial.println("The informations collected are not eligible to transmit to cloud!");
}

void sendTelemetry(void *pvParameters)
{
    dht20.begin();
    digitalWrite(BUZZER_PIN, LOW);

    for (;;)
    {
        if (otaInProgress)
        {
            vTaskDelay(telemetryInterval / portTICK_PERIOD_MS);
            continue;
        }

        if (WiFi.status() == WL_CONNECTED && client.connected())
        {
            if (xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(100)) == pdTRUE)
            {
                bool ok = dht20.read();
                if (!ok)
                {
                    float temp = dht20.getTemperature();
                    float hum = dht20.getHumidity();

                    if (isnan(temp) || isnan(hum))
                        exceptionHandler();
                    else
                    {
                        if (temp < thresholdTemperature or hum < thresholdHumidity)
                            exceptionHandler();
                        else
                        {
                            StaticJsonDocument<128> doc;
                            doc["temperature"] = temp;
                            doc["humidity"] = hum;
                            char buffer[128];
                            serializeJson(doc, buffer);
                            digitalWrite(BUZZER_PIN, LOW);
                            client.publish("v1/devices/me/telemetry", buffer);
                            Serial.println(" Sent DHT20: " + String(buffer));
                        }
                    }
                }
                else
                {
                    Serial.println(" DHT20 read failed");
                }

                xSemaphoreGive(i2cMutex);
            }
            else
            {
                Serial.println(" I2C busy, skip DHT20 read");
            }
        }

        vTaskDelay(telemetryInterval / portTICK_PERIOD_MS);
    }
}

void sendMQ2Data(void *pvParameters)
{
    for (;;)
    {
        if (otaInProgress)
        {
            vTaskDelay(mq2Interval / portTICK_PERIOD_MS);
            continue;
        }

        if (WiFi.status() == WL_CONNECTED && client.connected())
        {
            int mq2Value = analogRead(MQ2_AO_PIN);

            StaticJsonDocument<128> doc;
            doc["gas"] = mq2Value;
            char buffer[128];
            serializeJson(doc, buffer);
            client.publish("v1/devices/me/telemetry", buffer);
            Serial.println(" Sent MQ2: " + String(buffer));
        }

        vTaskDelay(mq2Interval / portTICK_PERIOD_MS);
    }
}
