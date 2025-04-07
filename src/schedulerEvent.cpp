#include "../include/schedulerEvent.h"
#include "../include/ledControl.h"
#include "../include/thingsboard.h"


void processSharedAttributes(const Shared_Attribute_Data &data)
{
  Serial.println("Binh bo");
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