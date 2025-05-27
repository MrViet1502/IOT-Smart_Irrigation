#ifndef __THINGSBOARD_H__
#define __THINGSBOARD_H__

// #include "ledControl.h"
#include "../lib/ThingsBoard/ThingsBoard.h"

// define the thingsboard information
#define THINGSBOARD_SERVER "app.coreiot.io"
#define THINGSBOARD_PORT 1883U

// define the token of the device
#define TOKEN_DEVICE "gB69jhkhOWD1wEYj6mm7"
#define MAX_MESSAGE_SIZE 1024U

extern ThingsBoard tb;

#endif
