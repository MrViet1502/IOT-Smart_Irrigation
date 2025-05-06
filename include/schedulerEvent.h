#ifndef __SCHEDULEREVENT_H__
#define __SCHEDULEREVENT_H__

// #include <ThingsBoard.h>
#include "../lib/ThingsBoard/Shared_Attribute_Callback.h"
#include "../lib/HTTPClient/src/HTTPClient.h"
#include <WiFi.h>


#define FIRMWARE_UPDATE_ATTR "fw_url"

extern bool OTAOnProgress;

void processSharedAttributes(const Shared_Attribute_Data &data);
void taskSchedulerEvent(void* ptrParameter);
void taskOTAFirmwareUpdate(void* ptrParameter);

#endif
