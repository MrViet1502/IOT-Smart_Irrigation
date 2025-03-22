#include "../include/readDHT20.h"
#include "../include/wifiModule.h"

void taskReadSensor(void* ptrParameter)
{
  // declare DHT20 object and wire obejct
  // DHT20 dht;
  // Wire.begin(SDA_PIN, SCL_PIN); // the oneWire communication
  // dht.begin();

  // declare thingsboard object
  WiFiClient wifiClient;
  Arduino_MQTT_Client mqttClient(wifiClient);
  ThingsBoard tb(mqttClient, MAX_MESSAGE_SIZE);

  int temperature = 0;
  int humidity = 0;
  while (true)
  {

    if (!tb.connected()) {
      Serial.print("Connecting to: ");
      Serial.print(THINGSBOARD_SERVER);
      Serial.print(" with token ");
      Serial.println(TOKEN_DEVICE);
      if (!tb.connect(THINGSBOARD_SERVER, TOKEN_DEVICE, THINGSBOARD_PORT))
        Serial.println("Failed to connect");
      else
        Serial.println("Connected to thingsboard");

      // tb.sendAttributeData("macAddress", WiFi.macAddress().c_str());

      // Serial.println("Subscribing for RPC...");
      // if (!tb.RPC_Subscribe(callbacks.cbegin(), callbacks.cend())) {
      //   Serial.println("Failed to subscribe for RPC");
      //   return;
      // }

      // if (!tb.Shared_Attributes_Subscribe(attributes_callback)) {
      //   Serial.println("Failed to subscribe for shared attribute updates");
      //   return;
      // }

      // Serial.println("Subscribe done");

      // if (!tb.Shared_Attributes_Request(attribute_shared_request_callback)) {
      //   Serial.println("Failed to request for shared attributes");
      //   return;
      // }
    }


    // dht.read();
    // double temperature = dht.getTemperature();
    // double humidity = dht.getHumidity();
    // if (isnan(temperature) || isnan(humidity))
    //   Serial.println("Failed to read from DHT20 sensor!");
    // else
    // {
    //   tb.sendTelemetryData("temperature", temperature);
    //   tb.sendTelemetryData("humidity", humidity);
    //   Serial.println("Temp: " + String(temperature) + " *C");
    //   Serial.println("Humidity: " + String(humidity) +  " %");
    //   Serial.println("\n \n");
    // }   
    tb.sendTelemetryData("temperature", temperature);
    tb.sendTelemetryData("humidity", humidity);
    Serial.println("Temp: " + String(temperature) + " *C");
    Serial.println("Humidity: " + String(humidity) +  " %");
    temperature +=3 ;
    humidity += 3;

    vTaskDelay(5000);
  }
}