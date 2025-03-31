#ifndef __SCHEDULEREVENT_H__
#define __SCHEDULEREVENT_H__

// #include <ThingsBoard.h>
#include "../lib/ThingsBoard/Shared_Attribute_Callback.h"

void processSharedAttributes(const Shared_Attribute_Data &data);
void taskSchedulerEvent(void* ptrParameter);

#endif
