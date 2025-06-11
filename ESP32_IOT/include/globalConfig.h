#ifndef GLOBAL_CONFIG_H
#define GLOBAL_CONFIG_H

#include <WiFi.h>
#include <PubSubClient.h>

// WiFi credentials
extern const char *ssid;
extern const char *password;

// MQTT broker details
extern const char *mqttServer;
extern const int mqttPort;
extern const char *ACCESS_TOKEN;

// WiFi and MQTT clients
extern WiFiClient espClient;
extern PubSubClient client;

// OTA update flags
extern String firmwareURL;
extern bool otaTriggered;
extern volatile bool otaInProgress;

// I2C mutex for sensor reads
extern SemaphoreHandle_t i2cMutex;

// Shared pin definitions
#define PUMP_PIN 12
#define OTA_Key "fw_url"
#define PUMP_STATE_ATTR "pump"
#define Soil_AO_PIN 34

// Telemetry intervals (ms)
extern const long telemetryInterval;
extern const long soilInterval;

// RFID module
#define BUZZER_PIN 25
extern byte authorizedUID[4]; // UID được cập nhật từ server
extern bool rfidUIDValid;     // Cờ xác nhận đã nhận UID hợp lệ
// Các task handle
extern TaskHandle_t WiFiTaskHandle;
extern TaskHandle_t MQTTaskHandle;
extern TaskHandle_t TelemetryTaskHandle;
extern TaskHandle_t SoilTaskHandle;
extern TaskHandle_t OTAUpdateTaskHandle;

#endif // GLOBAL_CONFIG_H
