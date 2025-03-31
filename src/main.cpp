#include "../include/ledControl.h"
#include "../include/wifiModule.h"
#include "../include/readDHT20.h"
#include "../include/schedulerEvent.h"

void setup() {
  Serial.begin(115200);
  xTaskCreate(taskCheckWifiConnection, "wifiConnection", 4096, NULL, 1, NULL);
  xTaskCreate(taskReadSensor, "readSensor", 16384, NULL, 2, NULL);
  xTaskCreate(taskSchedulerEvent, "schedulerEvent", 4096, NULL, 3, NULL);
  xTaskCreate(taskSubscribeRPC, "subscribeRPC", 4096, NULL, 4, NULL);
}

void loop() {
  // put your main code here, to run repeatedly:

}