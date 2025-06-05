#include "globalConfig.h"
#include "wifiModule.h"
#include "mqttModule.h"
#include "sensorsModule.h"
#include "otaModule.h"
#include "rfidModule.h"
#include <SPI.h>
#include <Wire.h>

TaskHandle_t WiFiTaskHandle = NULL;
TaskHandle_t MQTTaskHandle = NULL;
TaskHandle_t TelemetryTaskHandle = NULL;
TaskHandle_t SoilTaskHandle = NULL;
TaskHandle_t OTAUpdateTaskHandle = NULL;
TaskHandle_t RFIDTaskHandle = NULL;

void setup()
{
    Serial.begin(115200);
    delay(1000);
    Wire.begin();
    SPI.begin();
    i2cMutex = xSemaphoreCreateMutex();
    pinMode(Soil_AO_PIN, INPUT);
    WiFi.begin(ssid, password);
    client.setServer(mqttServer, mqttPort);
    pinMode(PUMP_PIN, OUTPUT);
    digitalWrite(PUMP_PIN, LOW);
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);
    client.setCallback(callback);

    // Tạo task
    xTaskCreate(checkWifiTask, "WiFiTask", 4096, NULL, 2, &WiFiTaskHandle);
    xTaskCreate(MQTTask, "MQTTask", 4096, NULL, 2, &MQTTaskHandle);
    xTaskCreate(sendTelemetry, "TelemetryTask", 4096, NULL, 1, &TelemetryTaskHandle);
    xTaskCreate(sendSoilData, "SoilTask", 4096, NULL, 1, &SoilTaskHandle);
    xTaskCreate(RunOTA_Update, "OTA_Update", 8192, NULL, 1, &OTAUpdateTaskHandle);
    xTaskCreate(readRFID, "RFIDTask", 4096, NULL, 1, &RFIDTaskHandle);
}

void loop() {}
