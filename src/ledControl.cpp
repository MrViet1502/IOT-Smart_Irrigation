#include "../include/thingsboard.h"
#include "../include/ledControl.h"
#include "../lib/ThingsBoard/RPC_Response.h"
#include "../lib/ThingsBoard/RPC_Callback.h"
// #include "../lib/ThingsBoard/RPC_Data.h"
#include <array>
#include <Arduino.h>

RPC_Response controlLed(const RPC_Data &data) {
  Serial.println("Received Switch state");
  bool newState = data;
  Serial.print("Switch state change: ");
  Serial.println(newState);
  digitalWrite(GPIO_LED, newState);
  tb.sendAttributeData(LED_STATE_ATTR, digitalRead(GPIO_LED));
  return RPC_Response(LED_STATE_ATTR, newState);
}


void taskSubscribeRPC(void* ptrParameter) {
  const std::array<RPC_Callback, 1U> callbacks = {
    RPC_Callback{LED_STATE_ATTR, controlLed}
  };

  while (true)
  {
    if (!tb.RPC_Subscribe(callbacks.cbegin(), callbacks.cend())) {
      Serial.println("Failed to subscribe for RPC");
      return;
    }
    Serial.println("Subscribe done");
    vTaskDelay(2000);
  }
  
}




