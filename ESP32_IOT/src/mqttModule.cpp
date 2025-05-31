#include "globalConfig.h"
#include <ArduinoJson.h>

const char *mqttServer = "app.coreiot.io";
const int mqttPort = 1883;
const char *ACCESS_TOKEN = "gB69jhkhOWD1wEYj6mm7";
float thresholdHumidity = 0;
float thresholdTemperature = 0;

// MQTT callback
void callback(char *topic, byte *payload, unsigned int length)
{
    payload[length] = '\0';
    String topicStr = String(topic);
    Serial.print("Received on topic: ");
    Serial.println(topicStr);
    Serial.print("Payload: ");
    Serial.println((char *)payload);

    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, payload);
    if (error)
    {
        Serial.print("JSON parse failed: ");
        Serial.println(error.c_str());
        return;
    }

    // Xử lý Shared Attribute update
    if (topicStr.startsWith("v1/devices/me/attributes"))
    {
        if (doc.containsKey(LED_STATE_ATTR))
        {
            bool ledState = doc[LED_STATE_ATTR];
            digitalWrite(LED_PIN, ledState ? HIGH : LOW);
            Serial.print("LED state set from Shared Attribute: ");
            Serial.println(ledState);
        }
        if (doc.containsKey(OTA_Key))
        {
            firmwareURL = doc["fw_url"].as<String>();
            otaTriggered = true;
            Serial.println("Received OTA URL: " + firmwareURL);
        }
        if (doc.containsKey(THRESHOLD_HUMIDITY))
        {
            thresholdHumidity  = doc[THRESHOLD_HUMIDITY].as<float>();
            Serial.println("The threshold value of humidity is updated by Server");
        }
        if (doc.containsKey(THRESHOLD_TEMPERATURE))
        {
            thresholdTemperature = doc[THRESHOLD_TEMPERATURE].as<float>();
            Serial.println("The threshold value of temperature is updated by Server");
        }
    }

    // Xử lý RPC
    if (topicStr.startsWith("v1/devices/me/rpc/request"))
    {
        if (doc["method"] == "setValue")
        {
            bool ledState = doc["params"];
            digitalWrite(LED_PIN, ledState ? HIGH : LOW);
            Serial.println(ledState ? "LED ON via RPC" : "LED OFF via RPC");

            StaticJsonDocument<128> response;
            response["value"] = ledState;
            char buffer[128];
            serializeJson(response, buffer);
            client.publish("v1/devices/me/attributes", buffer);
        }
    }
}

// MQTT reconnect
void reconnectMQTT()
{
    while (!client.connected())
    {
        Serial.print("Connecting to MQTT...");
        if (client.connect("ESP32", ACCESS_TOKEN, ""))
        {
            Serial.println(" MQTT Connected to ThingsBoard");
            client.subscribe("v1/devices/me/rpc/request/+");
            client.subscribe("v1/devices/me/attributes");
            client.setCallback(callback);
        }
        else
        {
            Serial.print(" Failed, rc=");
            Serial.print(client.state());
            Serial.println(" → retry in 5s");
            vTaskDelay(5000 / portTICK_PERIOD_MS);
        }
    }
}

// MQTT task
void MQTTask(void *pvParameters)
{
    for (;;)
    {
        if (!client.connected())
        {
            reconnectMQTT();
        }
        client.loop();
        vTaskDelay(100 / portTICK_PERIOD_MS); // Tối ưu delay MQTT loop
    }
}
