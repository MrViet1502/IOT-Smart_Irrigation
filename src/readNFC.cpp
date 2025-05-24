#include "../include/readNFC.h"

bool success = false

void taskReadNFC(void* ptrParameter)
{
  Adafruit_PN532 nfc(SDA_PIN, SCL_PIN);
  nfc.begin();
  nfc.SAMConfig();

  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
  uint8_t uidLength;

  while (true)
  {
    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  
    if (success) 
    {
      Serial.println("Found an NFC card!");
      Serial.print("UID Value: ");
      for (uint8_t i=0; i < uidLength; i++) 
      {
          Serial.print(" 0x");Serial.print(uid[i], HEX);
      }
      Serial.println("\n \n");
    }
    vTaskDelay(2000);
  }
  
}