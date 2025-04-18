#include "../include/readDHT20.h"
#include "../include/wifiModule.h"
#include "../include/thingsboard.h"

WiFiClient wifiClient;
Arduino_MQTT_Client mqttClient = Arduino_MQTT_Client(wifiClient);

ThingsBoard tb = ThingsBoard(mqttClient, MAX_MESSAGE_SIZE);


void taskReadSensor(void* ptrParameter)
{
  // declare DHT20 object and wire obejct
  DHT20 dht;
  Wire.begin(SDA_PIN, SCL_PIN); // the oneWire communication
  dht.begin();

  int temperature = 0;
  int humidity = 0;

  while (true)
  {

    // read data from DHT20 sensor
    dht.read();
    double temperature = dht.getTemperature();
    double humidity = dht.getHumidity();
    if (isnan(temperature) || isnan(humidity))
      Serial.println("Failed to read from DHT20 sensor!");
    else
    {
      // check connection to thingsboard server
      if (!tb.connected()) 
      {
        Serial.print("Connecting to: ");
        Serial.print(THINGSBOARD_SERVER);
        Serial.print(" with token ");
        Serial.println(TOKEN_DEVICE);
        if (!tb.connect(THINGSBOARD_SERVER, TOKEN_DEVICE, THINGSBOARD_PORT))
          Serial.println("Failed to connect");
        else
          Serial.println("Connected to thingsboard");
      }
      
      // send data to thingsboard server
      if (tb.connected())
      {
        tb.sendTelemetryData("temperature", temperature);
        tb.sendTelemetryData("humidity", humidity);
        Serial.println("Temp: " + String(temperature) + " *C");
        Serial.println("Humidity: " + String(humidity) +  " %");
        Serial.println("\n \n");
      }
    }   
    vTaskDelay(5000);
    tb.loop();
  }
}