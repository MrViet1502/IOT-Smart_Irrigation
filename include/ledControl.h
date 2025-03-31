#ifndef __RPC_H__
#define __RPC_H__

#include <ThingsBoard.h>

#define GPIO_LED 48
#define LED_STATE_ATTR "setStateLED"

RPC_Response controlLed(const RPC_Data &data);

void taskSubscribeRPC(void* ptrParameter);

#endif