#ifndef __READ_NFC_H__
#define __READ_NFC_H__

#include <Adafruit_PN532.h>

#define SDA_PIN GPIO_NUM_21
#define SCL_PIN GPIO_NUM_22
void taskReadNFC(void* ptrParameter);

extern bool success;

#endif