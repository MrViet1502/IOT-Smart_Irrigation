#include "../include/ledControl.h"
#include "../include/wifiModule.h"
#include "../include/readDHT20.h"
#include "../include/schedulerEvent.h"
// #inlcude "../include/readNFC.h"

void setup() {
  Serial.begin(115200);
  pinMode(GPIO_LED, OUTPUT);
  xTaskCreate(taskCheckWifiConnection,  "wifiConnection",     4096,   NULL, 1, NULL);
  xTaskCreate(taskReadSensor,           "readSensor",         16384,  NULL, 2, NULL);
  xTaskCreate(taskSchedulerEvent,       "schedulerEvent",     4096,   NULL, 3, NULL);
  xTaskCreate(taskSubscribeRPC,         "subscribeRPC",       4096,   NULL, 4, NULL);
  xTaskCreate(taskOTAFirmwareUpdate,    "otaFirmwareUpdate",  16384,   NULL, 5, NULL);
  // xTaskCreate(taskReadNFC, 		"ScanNFC", 	      2048,   NULL, 6, NULL);
}

void loop() {

}
