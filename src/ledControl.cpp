#include "../include/thingsboard.h"
#include "../include/ledControl.h"
#include "../lib/ThingsBoard/RPC_Response.h"
#include "../lib/ThingsBoard/RPC_Callback.h"
// #include "../lib/ThingsBoard/RPC_Data.h"
#include <array>
#include <Arduino.h>

RPC_Response controlLed(const RPC_Data &data) {
  Serial.println("Received Switch state");
  // Serial.println(String(data));
  int newState = int(data);
  Serial.println(newState);
  // Serial.print("Switch state change: ");
  // Serial.println(newState);

  digitalWrite(GPIO_LED, newState);
  // tb.sendAttributeData(LED_STATE_ATTR, digitalRead(GPIO_LED));
  return RPC_Response(LED_STATE_ATTR, newState);
}


// void processSetLedMode(const JsonVariantConst &data, JsonDocument &response) {
//   Serial.println("Received the set led state RPC method");

//   // Process data
//   int newState = data;

//   Serial.print("Mode to change: ");
//   Serial.println(newState);
//   StaticJsonDocument<1> response_doc;

//   digitalWrite(GPIO_LED, (uint8_t)(newState));


//   response_doc["newMode"] = newState;
//   // Returning current mode
//   response.set(response_doc);
// }

void taskSubscribeRPC(void* ptrParameter) {
  const std::array<RPC_Callback, 1U> callbacks = {
    RPC_Callback{LED_STATE_ATTR, controlLed}
  };
  int flagFirst  = false;
  while (true)
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

      // subscribe to RPC
      if (!tb.RPC_Subscribe(callbacks.cbegin(), callbacks.cend())) {
        Serial.println("Failed to subscribe for RPC");
      }
      else
      {
        Serial.println("Subscribe RPC done");
        flagFirst = true;
      }
      
    }
    else
    {
      Serial.println("Connected to thingsboad");
      if (flagFirst == false)
      {
        tb.RPC_Subscribe(callbacks.cbegin(), callbacks.cend());
        flagFirst = true;
      }
    }
    vTaskDelay(5000);
    tb.loop();
  }
  
}




