#ifndef __SENSOR_H__
#define __SENSOR_H__


#include <Arduino_MQTT_Client.h>
#include <WiFi.h>
#include <DHT20.h>
#include <ThingsBoard.h>

// define the position of pin in I2C protocol
#define SDA_PIN GPIO_NUM_11
#define SCL_PIN GPIO_NUM_12

void taskReadSensor(void* ptrParameter);

#endif