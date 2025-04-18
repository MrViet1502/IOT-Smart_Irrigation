import paho.mqtt.publish as publish
import json

data = {
    "soil_moisture": 45,
    "temperature": 35,
    "humidity": 75
}

publish.single(
    topic="sensor/data",
    payload=json.dumps(data),
    hostname="localhost",
    port=1883
)

print("Dữ liệu test đã gửi thành công!")
