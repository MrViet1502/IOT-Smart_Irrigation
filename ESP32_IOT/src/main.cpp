
// #include <WiFi.h>
// #include <Wire.h>
// #include <PubSubClient.h>
// #include <ArduinoJson.h>
// #include <freertos/FreeRTOS.h>
// #include <freertos/task.h>
// #include "DHT20.h"
// #include <Arduino.h>

// // WiFi Credentials
// const char *ssid = "viet";
// const char *password = "20252025";

// // ThingsBoard MQTT Broker
// const char *mqttServer = "app.coreiot.io";
// const int mqttPort = 1883;
// const char *ACCESS_TOKEN = "gB69jhkhOWD1wEYj6mm7"; // Token của thiết bị

// WiFiClient espClient;
// PubSubClient client(espClient);

// // Chân Analog MQ2 (AO)
// #define MQ2_AO_PIN 1
// DHT20 dht20;

// unsigned long lastTelemetryTime = 0;
// unsigned long lastMQ2Time = 0;
// const long telemetryInterval = 5000; // Gửi dữ liệu mỗi 5 giây
// const long MQ2Interval = 5000;       // Gửi dữ liệu MQ2 mỗi 5 giây

// // Task handles
// TaskHandle_t WiFiTaskHandle = NULL;
// TaskHandle_t MQTTaskHandle = NULL;
// TaskHandle_t MQ2TaskHandle = NULL;
// TaskHandle_t TelemetryTaskHandle = NULL;

// // Kết nối WiFi
// void connectWiFi()
// {
//     Serial.print("Connecting to WiFi...");
//     WiFi.begin(ssid, password);
//     while (WiFi.status() != WL_CONNECTED)
//     {
//         Serial.print(".");
//         vTaskDelay(pdMS_TO_TICKS(1000)); // Delay 1 giây
//     }
//     Serial.println("\nWiFi connected: " + WiFi.localIP().toString());
// }

// // Kết nối lại MQTT
// void reconnectMQTT()
// {
//     while (!client.connected())
//     {
//         Serial.print("Connecting to MQTT...");
//         if (client.connect("ESP32_MQ2", ACCESS_TOKEN, ""))
//         {
//             Serial.println("Connected to ThingsBoard!");
//         }
//         else
//         {
//             Serial.print("Failed, rc=");
//             Serial.print(client.state());
//             Serial.println(" retrying in 5 seconds...");
//             vTaskDelay(pdMS_TO_TICKS(5000)); // Delay 5 giây
//         }
//     }
// }

// // Gửi dữ liệu MQ2
// void sendMQ2Data(void *pvParameters)
// {
//     while (1)
//     {
//         unsigned long currentMillis = millis();
//         if (currentMillis - lastMQ2Time >= MQ2Interval)
//         {
//             lastMQ2Time = currentMillis;

//             int gasAnalogValue = analogRead(MQ2_AO_PIN);
//             Serial.println(gasAnalogValue);

//             StaticJsonDocument<128> doc;
//             doc["mq2_analog"] = gasAnalogValue;

//             char buffer[128];
//             serializeJson(doc, buffer);

//             if (client.connected())
//             {
//                 client.publish("v1/devices/me/telemetry", buffer);
//                 Serial.println("Sent MQ2 data: " + String(buffer));
//             }
//             else
//             {
//                 Serial.println("MQTT not connected, cannot send MQ2 data!");
//                 reconnectMQTT(); // Cố gắng kết nối lại MQTT nếu mất kết nối
//             }
//         }
//         vTaskDelay(pdMS_TO_TICKS(5000)); // Gửi dữ liệu MQ2 mỗi 5 giây
//     }
// }

// // Gửi telemetry từ DHT20
// void sendTelemetry(void *pvParameters)
// {
//     float temperature = 30;
//     float humidity = 50;
//     dht20.begin();
//     while (1)
//     {
//         unsigned long currentMillis = millis();
//         if (currentMillis - lastTelemetryTime >= telemetryInterval)
//         {
//             lastTelemetryTime = currentMillis;

//             if (dht20.read())
//             {
//                 // float temperature = dht20.getTemperature();
//                 // float humidity = dht20.getHumidity();

//                 Serial.println(temperature);
//                 Serial.println(humidity);

//                 StaticJsonDocument<128> doc;
//                 doc["temperature"] = temperature;
//                 doc["humidity"] = humidity;

//                 char buffer[128];
//                 serializeJson(doc, buffer);

//                 if (client.connected())
//                 {
//                     client.publish("v1/devices/me/telemetry", buffer);
//                     Serial.println("Sent telemetry: " + String(buffer));
//                 }
//                 else
//                 {
//                     Serial.println("MQTT not connected, cannot send telemetry data!");
//                     reconnectMQTT(); // Cố gắng kết nối lại MQTT nếu mất kết nối
//                 }
//             }
//             else
//             {
//                 Serial.println("Failed to read DHT20 sensor!");
//             }

//             temperature++;
//             humidity++;
//         }
//         vTaskDelay(pdMS_TO_TICKS(telemetryInterval)); // Gửi dữ liệu từ DHT20 mỗi 5 giây
//     }
// }

// // Tạo WiFi task
// void WiFiTask(void *pvParameters)
// {
//     connectWiFi();
// }

// // Tạo MQTT task
// void MQTTask(void *pvParameters)
// {
//     client.setServer(mqttServer, mqttPort);
//     reconnectMQTT();
// }

// void setup()
// {
//     Serial.begin(115200);

//     pinMode(MQ2_AO_PIN, INPUT);

//     // Create tasks
//     xTaskCreate(WiFiTask, "WiFiTask", 4096, NULL, 1, &WiFiTaskHandle);
//     xTaskCreate(MQTTask, "MQTTask", 4096, NULL, 1, &MQTTaskHandle);
//     xTaskCreate(sendMQ2Data, "MQ2Task", 2048, NULL, 1, &MQ2TaskHandle);
//     xTaskCreate(sendTelemetry, "TelemetryTask", 2048, NULL, 1, &TelemetryTaskHandle);
// }

// void loop()
// {
//     // FreeRTOS will handle tasks, no need for code here
// }

#include <WiFi.h>
#include <PubSubClient.h>
#define SOIL_SENSOR_PIN 34
#define RELAY_PIN 5

const char *ssid = "viet";
const char *password = "20252025";
const char *mqtt_server = "192.168.1.100";

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi()
{
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
    }
}

void callback(char *topic, byte *payload, unsigned int length)
{
    String msg;
    for (int i = 0; i < length; i++)
        msg += (char)payload[i];
    if (msg == "on")
        digitalWrite(RELAY_PIN, HIGH);
    else if (msg == "off")
        digitalWrite(RELAY_PIN, LOW);
}

void reconnect()
{
    while (!client.connected())
    {
        if (client.connect("ESP32Client"))
        {
            client.subscribe("pump/control");
        }
        else
        {
            delay(5000);
        }
    }
}

void setup()
{
    Serial.begin(115200);
    pinMode(RELAY_PIN, OUTPUT);
    setup_wifi();
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
}

void loop()
{
    if (!client.connected())
        reconnect();
    client.loop();

    int soil = analogRead(SOIL_SENSOR_PIN);
    float moisture = map(soil, 0, 4095, 100, 0);

    if (moisture < 30)
        digitalWrite(RELAY_PIN, HIGH);
    else
        digitalWrite(RELAY_PIN, LOW);

    String payload = "{\"soil_moisture\":" + String(moisture) + "}";
    client.publish("sensor/data", payload.c_str());
    delay(10000);
}