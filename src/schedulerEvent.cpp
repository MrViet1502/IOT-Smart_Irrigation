#include "../include/schedulerEvent.h"
#include "../include/ledControl.h"
#include "../include/thingsboard.h"
#include <Update.h>

bool newFirmwareUpdateAvailable = false;
char* firmwareUrl = nullptr;

void processSharedAttributes(const Shared_Attribute_Data &data)
{
  for (auto it = data.begin(); it != data.end(); ++it) 
  {
    Serial.println(it->key().c_str());
    if (strcmp(it->key().c_str(), LED_STATE_ATTR) == 0) 
    {
      volatile bool ledState = it->value().as<bool>();
      digitalWrite(GPIO_LED, ledState);
      Serial.print("LED state is set to: ");
      Serial.println(ledState);
    }
    else if (strcmp(it->key().c_str(), FIRMWARE_UPDATE_ATTR) == 0) 
    {
      firmwareUrl = strdup(it->value().as<const char*>());
      newFirmwareUpdateAvailable = true;
      Serial.print("Firmware update URL: ");
      Serial.println(firmwareUrl);
      Serial.println("New firmware update available!");
    }
  }
}

bool performOTAUpdate(char* firmwareUpdateURL)
{
  WiFiClient client;
  HTTPClient http;

  Serial.print("Connecting to firmware URL: ");
  Serial.println(firmwareUpdateURL);

  if (!http.begin(client, firmwareUpdateURL)) {
    Serial.println("Failed to initialize HTTP connection.");
    return false;
  }

  int httpCode = http.GET();
  if (httpCode != 200) {
    Serial.print("HTTP GET failed with code: ");
    Serial.println(httpCode);
    http.end();
    return false;
  }

  int contentLength = http.getSize();
  if (contentLength <= 0) {
    Serial.println("Invalid content length.");
    http.end();
    return false;
  }

  bool canBegin = Update.begin(contentLength);
  if (!canBegin) {
    Serial.println("Not enough space to begin OTA update.");
    http.end();
    return false;
  }

  WiFiClient* stream = http.getStreamPtr();
  size_t written = 0;
  uint8_t buffer[128] = {0};

  while (http.connected() && written < contentLength) {
    size_t available = stream->available();
    if (available) {
      size_t bytesRead = stream->readBytes(buffer, min(available, sizeof(buffer)));
      written += Update.write(buffer, bytesRead);
      Serial.print(".");
    }
  }

  if (Update.end()) {
    if (Update.isFinished()) {
      Serial.println("\nOTA update completed successfully.");
      http.end();
      return true;
    } else {
      Serial.println("\nOTA update not finished.");
    }
  } else {
    Serial.print("\nOTA update failed. Error #: ");
    Serial.println(Update.getError());
  }

  http.end();
  return false;
}


void taskOTAFirmwareUpdate(void* ptrParameter)
{
  while (true)
  {
    if (newFirmwareUpdateAvailable && firmwareUrl != nullptr)
    {
      Serial.println("Starting OTA firmware update...");
      if (performOTAUpdate(firmwareUrl))
      {
        Serial.println("Firmware update successful. Restarting device...");
        ESP.restart(); // Restart the device after successful update
      }
      else
      {
        Serial.println("Firmware update failed.");
      }
      // Reset the flags after attempting the update
      newFirmwareUpdateAvailable = false;
      free(firmwareUrl);
      firmwareUrl = nullptr;
    }
    vTaskDelay(8000);
  }
}

void taskSchedulerEvent(void* ptrParameter)
{
  constexpr std::array<const char *, 1U> SHARED_ATTRIBUTES_LIST = {
    LED_STATE_ATTR,
  };
  const Shared_Attribute_Callback attributes_callback(&processSharedAttributes, SHARED_ATTRIBUTES_LIST.cbegin(), SHARED_ATTRIBUTES_LIST.cend());
  bool flagFirst = false;
  while   (true)
  {
    if (!tb.connected())
    {
      // connect to thingsboard
      if (!tb.connect(THINGSBOARD_SERVER, TOKEN_DEVICE, THINGSBOARD_PORT))
          Serial.println("Failed to connect");
      else
      {
        Serial.println("Connected to thingsboard");
      }

      // subscribe the share attributes
      // tb.Subscribe_Firmware_Update(callback);
      // tb.Start_Firmware_Update();
      // tb.Subscribe_Firmware_Update();
      // tb.firm

      if (!tb.Shared_Attributes_Subscribe(attributes_callback)) {
        Serial.println("Failed to subscribe for shared attribute updates");
      }
      else
      {
        Serial.println("Subscribe share attributes done");
        flagFirst = true;
      }
    }
    else
    {
      Serial.println("Connected to thingsboard");
      if (flagFirst== false)
      {
        flagFirst = true;
        tb.Shared_Attributes_Subscribe(attributes_callback);
      }
    }
    // if (!tb.Shared_Attributes_Subscribe(attributes_callback)) {
    //   Serial.println("Failed to subscribe for shared attribute updates");
    // }
    // else
    // {
    //   Serial.println("Subscribe share attributes done");
    //   flagFirst = true;
    // }
    vTaskDelay(5000);
    tb.loop();
  }
}