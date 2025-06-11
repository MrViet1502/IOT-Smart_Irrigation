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

// 1 UID hợp lệ (4 byte)
// const byte authorizedUID[4] = {0x7C, 0x1A, 0x11, 0x05}; // đăng kí thẻ trước đó

byte authorizedUID[4] = {0, 0, 0, 0};
bool rfidUIDValid = false;

void buzzerBeep(int count)
{
    for (int i = 0; i < count; i++)
    {
        digitalWrite(BUZZER_PIN, HIGH);
        vTaskDelay(100 / portTICK_PERIOD_MS);
        digitalWrite(BUZZER_PIN, LOW);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void readRFID(void *pvParameters)
{
    mfrc522.PCD_Init();
    Serial.println("Ready to scan...");

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

        // So sánh UID quẹt được với UID hợp lệ
        bool isAuthorized = true;
        for (byte i = 0; i < 4; i++)
        {
            if (mfrc522.uid.uidByte[i] != authorizedUID[i])
            {
                isAuthorized = false;
                break;
            }
        }

        if (isAuthorized && rfidUIDValid)
        {
            // Đúng thẻ được đăng ký
            if (digitalRead(PUMP_PIN) == LOW)
            {
                // Bơm đang tắt => bật bơm
                digitalWrite(PUMP_PIN, HIGH);
                Serial.println("Authorized card! PUMP ON");
            }
            else
            {
                // Bơm đang bật => tắt bơm
                digitalWrite(PUMP_PIN, LOW);
                Serial.println("Authorized card! PUMP OFF");
            }
            buzzerBeep(1); // 1 tiếng beep
        }
        else
        {
            // Sai thẻ
            Serial.println("Unauthorized card! Access denied.");
            buzzerBeep(3); // 3 tiếng beep
        }

        // Kết thúc giao tiếp với thẻ
        mfrc522.PICC_HaltA();

        // Tránh đọc liên tục
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
