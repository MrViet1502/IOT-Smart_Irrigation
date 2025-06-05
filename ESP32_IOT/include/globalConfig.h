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
#define LED_PIN 12
#define OTA_Key "fw_url"
#define LED_STATE_ATTR "led"
#define MQ2_AO_PIN 34

// Telemetry intervals (ms)
extern const long telemetryInterval;
extern const long mq2Interval;

// Các task handle
extern TaskHandle_t WiFiTaskHandle;
extern TaskHandle_t MQTTaskHandle;
extern TaskHandle_t TelemetryTaskHandle;
extern TaskHandle_t MQ2TaskHandle;
extern TaskHandle_t OTAUpdateTaskHandle;

#endif // GLOBAL_CONFIG_H
