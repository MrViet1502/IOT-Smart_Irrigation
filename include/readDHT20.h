#ifndef __SENSOR_H__
#define __SENSOR_H__


#include <Arduino_MQTT_Client.h>
#include <WiFi.h>
#include <DHT20.h>
#include <ThingsBoard.h>

// define the position of pin in I2C protocol
#define SDA_PIN GPIO_NUM_21
#define SCL_PIN GPIO_NUM_22

// define the thingsboard information
#define THINGSBOARD_SERVER  "app.coreiot.io"
#define THINGSBOARD_PORT    1883U


// define the token of the device
#define TOKEN_DEVICE        "w1d1cwh8qzkfbf5rfrvk"
#define MAX_MESSAGE_SIZE    1024U

void taskReadSensor(void* ptrParameter);

#endif