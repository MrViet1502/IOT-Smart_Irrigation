#include "../include/blinkLed.h"
#include "../include/wifiModule.h"
#include "../include/readDHT20.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  xTaskCreate(taskCheckWifiConnection, "wifiConnection", 4096, NULL, 1, NULL);
  xTaskCreate(taskReadSensor, "readSensor", 16384, NULL, 2, NULL);
  // xTaskCreate(taskLEDControl, "LED_control", 2048, NULL, 3, NULL);
}

void loop() {
  // put your main code here, to run repeatedly:

}