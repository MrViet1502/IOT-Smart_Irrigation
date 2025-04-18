
from flask import Flask, jsonify, render_template, request
import requests
from datetime import datetime
import os

app = Flask(__name__, template_folder="templates")


THINGSBOARD_URL = "https://app.coreiot.io"
DEVICE_ID = "1f5f==" # key id device
JWT_TOKEN = "eyJhbGciOiJIUzUxMiJ9.===="  #key token
HEADERS = {"X-Authorization": f"Bearer {JWT_TOKEN}"}

@app.route("/api/data")
def get_latest_data():
    keys = "humidity,temperature,mq2_analog"
    url = f"{THINGSBOARD_URL}/api/plugins/telemetry/DEVICE/{DEVICE_ID}/values/timeseries?keys={keys}"

    response = requests.get(url, headers=HEADERS)
    if response.status_code != 200:
        return jsonify({"error": "ThingsBoard API error"}), 500

    data = response.json()
    result = {}
    for key in data:
        latest = data[key][-1]
        value = latest["value"]
        ts = int(latest["ts"]) / 1000
        time_str = datetime.fromtimestamp(ts).strftime('%Y-%m-%d %H:%M:%S')
        result[key] = {"value": value, "timestamp": time_str}
    return jsonify(result)


@app.route("/api/control", methods=["POST"])
def control_device():
    payload = request.get_json()
    command = payload.get("command")  # "on" hoặc "off"

    if command not in ["on", "off"]:
        return jsonify({"error": "Invalid command"}), 400

    led_state = True if command == "on" else False

    url = f"{THINGSBOARD_URL}/api/plugins/telemetry/DEVICE/{DEVICE_ID}/attributes/SHARED_SCOPE"
    body = {
        "led": led_state
    }

    try:
        response = requests.post(url, headers=HEADERS, json=body, timeout=5)
        if response.status_code == 200:
            return jsonify({"status": "success", "led": led_state})
        else:
            return jsonify({"error": "Failed to send shared attribute", "code": response.status_code}), 500
    except requests.exceptions.RequestException as e:
        return jsonify({"error": f"Request exception: {str(e)}"}), 500

@app.route("/")
def index():
    return render_template("index.html")

if __name__ == "__main__":
    app.run(debug=True)



# from flask import Flask, render_template, jsonify, request
# import threading
# import json
# import paho.mqtt.client as mqtt
# import logging

# # Cấu hình logging (giúp bạn debug)
# logging.basicConfig(level=logging.INFO)

# app = Flask(__name__)
# sensor_data = {"soil_moisture": 0, "temperature": 0, "humidity": 0}

# # MQTT cấu hình
# MQTT_BROKER = "localhost"
# MQTT_PORT = 1883
# MQTT_TOPIC_SUB = "sensor/data"
# MQTT_TOPIC_PUB = "led/control"

# # Khởi tạo client dùng cho lắng nghe dữ liệu
# mqtt_client = mqtt.Client(protocol=mqtt.MQTTv311)


# # Callback khi nhận dữ liệu từ MQTT
# def on_message(client, userdata, msg):
#     global sensor_data
#     try:
#         payload = json.loads(msg.payload.decode())
#         sensor_data.update(payload)
#         logging.info(f"Received: {sensor_data}")
#     except Exception as e:
#         logging.warning(f"Failed to parse MQTT message: {e}")


# # Luồng riêng để lắng nghe MQTT
# def mqtt_listen():
#     mqtt_client.on_message = on_message
#     try:
#         mqtt_client.connect(MQTT_BROKER, MQTT_PORT)
#         mqtt_client.subscribe(MQTT_TOPIC_SUB)
#         mqtt_client.loop_forever()
#     except Exception as e:
#         logging.error(f"MQTT connect/listen failed: {e}")


# # Giao diện hiển thị chính
# @app.route('/')
# def index():
#     return render_template('index.html', data=sensor_data)


# # API lấy dữ liệu JSON (gọi từ frontend)
# @app.route('/data')
# def data():
#     return jsonify(sensor_data)


# # API gửi lệnh điều khiển từ web đến ESP32 qua MQTT
# @app.route('/control', methods=['GET'])
# def control():
#     action = request.args.get('action')
#     if action not in ["on", "off"]:
#         return jsonify({"status": "error", "message": "Invalid action"}), 400

#     try:
#         mqtt_client.publish(MQTT_TOPIC_PUB, action)
#         logging.info(f"Published control: {action}")
#         return jsonify({"status": "ok", "action": action})
#     except Exception as e:
#         logging.error(f"Failed to publish control command: {e}")
#         return jsonify({"status": "error", "message": str(e)}), 500


# if __name__ == '__main__':
#     mqtt_thread = threading.Thread(target=mqtt_listen, daemon=True)
#     mqtt_thread.start()
#     app.run(debug=True, port=5000)


# from flask import Flask, render_template, request, jsonify
# import threading
# from mqtt_handler import mqtt_listen, publish_control
# from firebase_handler import log_sensor_data

# app = Flask(__name__)
# sensor_data = {"soil_moisture": 0}

# def update_data(data):
#     sensor_data.update(data)
#     log_sensor_data(data)

# mqtt_thread = threading.Thread(target=mqtt_listen, args=(update_data,), daemon=True)
# mqtt_thread.start()

# @app.route('/')
# def index():
#     return render_template('index.html', data=sensor_data)

# @app.route('/data')
# def data():
#     return jsonify(sensor_data)

# @app.route('/control')
# def control():
#     action = request.args.get('action')
#     if action in ['on', 'off']:
#         publish_control(action)
#         return jsonify({"status": "ok", "action": action})
#     return jsonify({"status": "error"}), 400

# if __name__ == '__main__':
#     app.run(debug=True, use_reloader=False)

# Using firebase

# ================================

