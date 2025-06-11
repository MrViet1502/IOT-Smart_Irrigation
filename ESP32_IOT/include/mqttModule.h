#ifndef MQTT_MODULE_H
#define MQTT_MODULE_H

void reconnectMQTT();
void MQTTask(void *pvParameters);
void callback(char *topic, byte *payload, unsigned int length);

#endif // MQTT_MODULE_H
