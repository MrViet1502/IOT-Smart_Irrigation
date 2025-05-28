#include "globalConfig.h"
#include <MFRC522.h>

#define SS_PIN 5
#define RST_PIN 26

// Kêt nối chân giữa Wemos esp32 với MFRC522
// GPIO05 -> SDA
// GPIO18 -> SCK
// GPIO19 -> MOSI
// GPIO23 -> MISO
// GPIO26 -> RST
// GND -> GND
// 3.3V -> VCC

MFRC522 mfrc522(SS_PIN, RST_PIN);

// Ví dụ 1 UID hợp lệ (4 byte)
const byte authorizedUID[4] = {0x7C, 0x1A, 0x11, 0x05};

void readRFID(void *pvParameters)
{

    mfrc522.PCD_Init();
    Serial.println("[RFID] Ready to scan...");

    for (;;)
    {
        if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial())
        {
            vTaskDelay(500 / portTICK_PERIOD_MS);
            continue;
        }

        // In ra UID quẹt được
        Serial.print("[RFID] UID: ");
        for (byte i = 0; i < mfrc522.uid.size; i++)
        {
            Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
            Serial.print(mfrc522.uid.uidByte[i], HEX);
        }
        Serial.println();

        // So sánh với UID hợp lệ
        bool isAuthorized = true;
        for (byte i = 0; i < 4; i++)
        {
            if (mfrc522.uid.uidByte[i] != authorizedUID[i])
            {
                isAuthorized = false;
                break;
            }
        }

        if (isAuthorized)
        {
            Serial.println(" Authorized card! LED ON");
            digitalWrite(LED_PIN, HIGH);
        }
        else
        {
            Serial.println(" Unauthorized card! LED OFF");
            digitalWrite(LED_PIN, LOW);
        }

        // Kết thúc giao tiếp với thẻ
        mfrc522.PICC_HaltA();

        // Tạm dừng để tránh lặp
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
