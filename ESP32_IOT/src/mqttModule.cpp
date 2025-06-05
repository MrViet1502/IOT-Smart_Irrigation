#include "globalConfig.h"
#include <ArduinoJson.h>

const char *mqttServer = "app.coreiot.io";
const int mqttPort = 1883;
const char *ACCESS_TOKEN = "gB69jhkhOWD1wEYj6mm7";

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
        Serial.print("JSON parse faipump: ");
        Serial.println(error.c_str());
        return;
    }

    // Xử lý Shared Attribute update
    if (topicStr.startsWith("v1/devices/me/attributes"))
    {
        if (doc.containsKey(PUMP_STATE_ATTR))
        {
            bool pumpState = doc[PUMP_STATE_ATTR];
            digitalWrite(PUMP_PIN, pumpState ? HIGH : LOW);
            Serial.print("pump state set from Shared Attribute: ");
            Serial.println(pumpState);
        }
        if (doc.containsKey(OTA_Key))
        {
            firmwareURL = doc["fw_url"].as<String>();
            otaTriggered = true;
            Serial.println("Received OTA URL: " + firmwareURL);
        }
        if (doc.containsKey("rfid_key"))
        {
            String rfidStr = doc["rfid_key"]; // e.g. "0x7C,0x1A,0x11,0x05"
            Serial.println("[RFID] Received RFID UID string: " + rfidStr);

            // Parse chuỗi thành mảng byte
            int index = 0;
            int lastPos = 0;

            for (int i = 0; i < rfidStr.length() && index < 4; i++)
            {
                if (rfidStr[i] == ',' || i == rfidStr.length() - 1)
                {
                    String hexVal = rfidStr.substring(lastPos, i + (i == rfidStr.length() - 1 ? 1 : 0));
                    hexVal.trim();

                    if (hexVal.startsWith("0x") || hexVal.startsWith("0X"))
                    {
                        hexVal = hexVal.substring(2);
                    }

                    authorizedUID[index] = (byte)strtol(hexVal.c_str(), NULL, 16);
                    index++;
                    lastPos = i + 1;
                }
            }

            if (index == 4 && rfidStr.indexOf(",") != -1)
            {
                rfidUIDValid = true;
                Serial.print("[RFID] Updated authorized UID: ");
                for (int i = 0; i < 4; i++)
                {
                    Serial.print("0x");
                    Serial.print(authorizedUID[i], HEX);
                    if (i < 3)
                        Serial.print(", ");
                }
                Serial.println();
            }
            else
            {
                Serial.println("[RFID] Error: Invalid UID format");
                rfidUIDValid = false;
            }
        }
    }

    // Xử lý RPC
    if (topicStr.startsWith("v1/devices/me/rpc/request"))
    {
        if (doc["method"] == "setValue")
        {
            bool pumpState = doc["params"];
            digitalWrite(PUMP_PIN, pumpState ? HIGH : LOW);
            Serial.println(pumpState ? "pump ON via RPC" : "pump OFF via RPC");

            StaticJsonDocument<128> response;
            response["value"] = pumpState;
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
            Serial.print(" Faipump, rc=");
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
