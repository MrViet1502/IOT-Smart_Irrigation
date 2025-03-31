#include "../include/schedulerEvent.h"
#include "../include/ledControl.h"
#include "../include/thingsboard.h"


void processSharedAttributes(const Shared_Attribute_Data &data)
{
  for (auto it = data.begin(); it != data.end(); ++it) {
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
  
  while (true)
  {
    if (!tb.Shared_Attributes_Subscribe(attributes_callback)) {
      Serial.println("Failed to subscribe for shared attribute updates");
    }
    vTaskDelay(2000);
  }
}