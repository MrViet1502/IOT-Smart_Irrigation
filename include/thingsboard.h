#ifndef __THINGSBOARD_H__
#define __THINGSBOARD_H__

// #include "ledControl.h"
#include "../lib/ThingsBoard/ThingsBoard.h"

// define the thingsboard information
#define THINGSBOARD_SERVER  "app.coreiot.io"
#define THINGSBOARD_PORT    1883U

// define the token of the device
#define TOKEN_DEVICE        "w1d1cwh8qzkfbf5rfrvk"
#define MAX_MESSAGE_SIZE    1024U

extern ThingsBoard tb;

#endif
