import json
import paho.mqtt.client as mqtt

MQTT_BROKER = "localhost"
MQTT_PORT = 1883
SENSOR_TOPIC = "sensor/data"
CONTROL_TOPIC = "pump/control"

callback_fn = None
client = mqtt.Client()

def on_message(client, userdata, msg):
    try:
        payload = json.loads(msg.payload.decode())
        if callback_fn:
            callback_fn(payload)
    except Exception as e:
        print("MQTT parse error:", e)

def mqtt_listen(callback):
    global callback_fn
    callback_fn = callback
    client.on_message = on_message
    client.connect(MQTT_BROKER, MQTT_PORT)
    client.subscribe(SENSOR_TOPIC)
    client.loop_forever()

def publish_control(action):
    client.publish(CONTROL_TOPIC, action)
