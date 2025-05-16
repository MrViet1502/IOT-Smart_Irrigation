from flask import Flask, jsonify, render_template, request, session, redirect, url_for
from flask_wtf.csrf import CSRFProtect, generate_csrf
from datetime import datetime, timedelta
import requests
import threading
import time
import csv
import os
import json
from apscheduler.schedulers.background import BackgroundScheduler

from threading import Thread
from datetime import datetime


SCHEDULE_FILE = "schedules.json"
schedules = []  # Each item: {"time": "HH:MM", "command": "on" or "off"}
app = Flask(__name__, template_folder="templates")
app.secret_key = "qwertyuiop"  # Đặt key bảo mật riêng
csrf = CSRFProtect(app)

# ----------- Cấu hình IoT ThingsBoard -------------
THINGSBOARD_URL = "https://app.coreiot.io"
DEVICE_ID = "1f5f2270-f990-11ef-a887-6d1a184f2bb5"
JWT_TOKEN = "eyJhbGciOiJIUzUxMiJ9.eyJzdWIiOiJuLnF1b2N2aWV0MTUwMkBnbWFpbC5jb20iLCJ1c2VySWQiOiI5NDMyNTg5MC1lZTcwLTExZWYtODdiNS0yMWJjY2Y3ZDI5ZDUiLCJzY29wZXMiOlsiVEVOQU5UX0FETUlOIl0sInNlc3Npb25JZCI6ImUwZWQ2YmYxLWE5ZmMtNDIzNS1iMzFlLTE5MjJkYWU0NDQxMiIsImV4cCI6MTc0NzQxNDAwMywiaXNzIjoiY29yZWlvdC5pbyIsImlhdCI6MTc0NzQwNTAwMywiZmlyc3ROYW1lIjoiVmnhu4d0IiwibGFzdE5hbWUiOiJOZ3V54buFbiBRdeG7kWMiLCJlbmFibGVkIjp0cnVlLCJpc1B1YmxpYyI6ZmFsc2UsInRlbmFudElkIjoiOTQyYTkwNjAtZWU3MC0xMWVmLTg3YjUtMjFiY2NmN2QyOWQ1IiwiY3VzdG9tZXJJZCI6IjEzODE0MDAwLTFkZDItMTFiMi04MDgwLTgwODA4MDgwODA4MCJ9.PZkU7UxJJF7tFi61dvERKNHDkHgDLuuEVaDaMZ49j3OjSH1dJmzxTIQuqRm7b1_u_sGOg0YqFuLIihn4SZUEAA"
HEADERS = {"X-Authorization": f"Bearer {JWT_TOKEN}"}

# ----------- Cấu hình lưu dữ liệu -------------
CSV_TEMP = "temperature_data.csv"
CSV_HUM = "humidity_data.csv"
CSV_GAS = "gas_data.csv"
MAX_ROWS = 40

# Inject CSRF token cho form
@app.context_processor
def inject_csrf_token():
    return {"csrf_token_value": generate_csrf()}

if os.path.exists(SCHEDULE_FILE):
    with open(SCHEDULE_FILE, "r") as f:
        schedules = json.load(f)
# ----------------------------------------
# Helper: Lưu dữ liệu vào CSV
def save_to_csv(file_path, timestamp, value):
    rows = []
    if os.path.exists(file_path):
        with open(file_path, "r") as f:
            rows = list(csv.reader(f))

    rows.append([timestamp, value])

    if len(rows) > MAX_ROWS:
        rows = rows[-MAX_ROWS:]

    with open(file_path, "w", newline="") as f:
        writer = csv.writer(f)
        writer.writerows(rows)

# ----------------------------------------
# LOGIN / SESSION
@app.route("/login", methods=["GET", "POST"])
def login():
    if request.method == "POST":
        username = request.form.get("username")
        password = request.form.get("password")

        if username == "admin" and password == "123456":
            session['user_id'] = username
            session.permanent = True
            app.permanent_session_lifetime = timedelta(minutes=30)
            return redirect(url_for("dashboard"))
        else:
            return render_template("login.html", error="Invalid username or password")

    return render_template("login.html")

@app.route("/logout")
def logout():
    session.clear()
    return redirect(url_for("login"))

@app.route("/api/check_session")
def check_session():
    return {"logged_in": 'user_id' in session}

# ----------------------------------------
# DASHBOARD + GIAO DIỆN
THRESHOLD_FILE = "thresholds.json"

@app.route("/api/thresholds", methods=["GET"])
def get_thresholds():
    try:
        if os.path.exists(THRESHOLD_FILE):
            with open(THRESHOLD_FILE, "r") as f:
                return jsonify(json.load(f))
    except:
        pass
    return jsonify({"temperature": None, "humidity": None, "gas": None})

@app.route("/api/thresholds", methods=["POST"])
@csrf.exempt
def save_thresholds():
    data = request.get_json()
    type_ = data.get("type")
    value = data.get("value")

    if not type_ or not isinstance(value, (float, int)):
        return jsonify({"error": "Invalid threshold"}), 400

    thresholds = {}
    if os.path.exists(THRESHOLD_FILE):
        with open(THRESHOLD_FILE, "r") as f:
            thresholds = json.load(f)

    thresholds[type_] = value

    with open(THRESHOLD_FILE, "w") as f:
        json.dump(thresholds, f)

    return jsonify({"status": "saved"})




@app.route("/")
def index():
    if 'user_id' in session:
        return redirect(url_for("dashboard"))
    return redirect(url_for("login"))

@app.route("/dashboard")
def dashboard():
    if 'user_id' in session:
        return render_template("dashboard.html")
    return redirect(url_for("login"))

@app.route("/tempChart")
def temp_chart():
    if 'user_id' in session:
        return render_template("temp_chart.html")
    return redirect(url_for("login"))

@app.route("/humChart")
def hum_chart():
    if 'user_id' in session:
        return render_template("hum_chart.html")
    return redirect(url_for("login"))

@app.route("/gasChart")
def gas_chart():
    if 'user_id' in session:
        return render_template("gas_chart.html")
    return redirect(url_for("login"))

# ----------------------------------------
# API realtime đọc data từ ThingsBoard
@app.route("/api/data")
def get_data():
    keys = "humidity,temperature,gas"
    url = f"{THINGSBOARD_URL}/api/plugins/telemetry/DEVICE/{DEVICE_ID}/values/timeseries?keys={keys}"

    try:
        response = requests.get(url, headers=HEADERS)
        response.raise_for_status()
        data = response.json()
        result = {}
        for key in data:
            if data[key]:
                latest = data[key][-1]
                value = latest["value"]
                ts = int(latest["ts"]) / 1000
                time_str = datetime.fromtimestamp(ts).strftime('%Y-%m-%d %H:%M:%S')
                result[key] = {"value": value, "timestamp": time_str}
        return jsonify(result)

    except requests.exceptions.RequestException as e:
        return jsonify({"error": f"ThingsBoard API error: {str(e)}"}), 500

# ----------------------------------------
# API lấy dữ liệu lịch sử từ CSV (Chart)
@app.route("/api/temp_history")
def get_temp_history():
    return load_csv_to_json(CSV_TEMP)

@app.route("/api/hum_history")
def get_hum_history():
    return load_csv_to_json(CSV_HUM)

@app.route("/api/gas_history")
def get_gas_history():
    return load_csv_to_json(CSV_GAS)

def load_csv_to_json(file_path):
    result = []
    try:
        with open(file_path, "r") as f:
            reader = csv.reader(f)
            for row in reader:
                result.append({"timestamp": row[0], "value": float(row[1])})
    except FileNotFoundError:
        pass

    return jsonify(result)

# ----------------------------------------
# API điều khiển LED
@app.route("/api/control", methods=["POST"])
@csrf.exempt
def control_device():
    payload = request.get_json()

    # Trường hợp gửi lệnh OTA update
    if "ota_url" in payload:
        ota_url = payload["ota_url"]
        url = f"{THINGSBOARD_URL}/api/plugins/telemetry/DEVICE/{DEVICE_ID}/attributes/SHARED_SCOPE"
        body = {"fw_url": ota_url}
        try:
            response = requests.post(url, headers=HEADERS, json=body, timeout=5)
            response.raise_for_status()
            return jsonify({"status": "success", "type": "ota"})
        except requests.exceptions.RequestException as e:
            return jsonify({"error": f"OTA request failed: {str(e)}"}), 500

    # Trường hợp gửi lệnh bật/tắt LED
    command = payload.get("command")
    if command not in ["on", "off"]:
        return jsonify({"error": "Invalid command"}), 400

    led_state = command == "on"
    url = f"{THINGSBOARD_URL}/api/plugins/telemetry/DEVICE/{DEVICE_ID}/attributes/SHARED_SCOPE"
    body = {"led": led_state}

    try:
        response = requests.post(url, headers=HEADERS, json=body, timeout=5)
        response.raise_for_status()
        return jsonify({"status": "success", "led": led_state})
    except requests.exceptions.RequestException as e:
        return jsonify({"error": f"LED request failed: {str(e)}"}), 500

# ----------------------------------------
# BACKGROUND COLLECTOR: lấy dữ liệu mỗi 10 giây ghi CSV
def collector_loop():
    while True:
        try:
            res = requests.get("http://localhost:5000/api/data")
            if res.status_code == 200:
                data = res.json()

                temp_value = data["temperature"]["value"]
                temp_time = data["temperature"]["timestamp"]
                save_to_csv(CSV_TEMP, temp_time, temp_value)

                hum_value = data["humidity"]["value"]
                hum_time = data["humidity"]["timestamp"]
                save_to_csv(CSV_HUM, hum_time, hum_value)

                gas_value = data["gas"]["value"]
                gas_time = data["gas"]["timestamp"]
                save_to_csv(CSV_GAS, gas_time, gas_value)

                print(f"Saved: Temp={temp_value}, Hum={hum_value}, Gas={gas_value}")
        except Exception as e:
            print("Collector error:", e)

        time.sleep(50)



# Add this route
@app.route("/scheduler")
def scheduler_page():
    if 'user_id' in session:
        return render_template("scheduler.html")
    return redirect(url_for("login"))

# Add these API endpoints
@app.route("/api/schedule", methods=["GET", "POST"])
@csrf.exempt
def manage_schedule():
    global schedules
    if request.method == "GET":
        return jsonify(schedules)

    data = request.get_json()
    time_str = data.get("time")
    command = data.get("command")

    if not time_str or command not in ["on", "off"]:
        return jsonify({"error": "Invalid input"}), 400

    schedules.append({"time": time_str, "command": command})
    with open(SCHEDULE_FILE, "w") as f:
        json.dump(schedules, f)
    return jsonify({"status": "added"})

@app.route("/api/schedule/<int:index>", methods=["DELETE"])
@csrf.exempt
def delete_schedule(index):
    global schedules
    if 0 <= index < len(schedules):
        schedules.pop(index)
        with open(SCHEDULE_FILE, "w") as f:
            json.dump(schedules, f)
        return jsonify({"status": "deleted"})
    return jsonify({"error": "Invalid index"}), 404

# Background thread to check schedules every minute
def run_scheduled_tasks():
    last_executed = set()
    while True:
        now = datetime.now().strftime("%H:%M")
        for entry in schedules:
            key = f"{now}-{entry['command']}"
            if entry["time"] == now and key not in last_executed:
                url = f"{THINGSBOARD_URL}/api/plugins/telemetry/DEVICE/{DEVICE_ID}/attributes/SHARED_SCOPE"
                body = {"led": entry["command"] == "on"}
                try:
                    requests.post(url, headers=HEADERS, json=body, timeout=5)
                    print(f"[Scheduler] Sent command: {entry['command']} at {now}")
                    last_executed.add(key)
                except Exception as e:
                    print("Scheduler send error:", e)
        time.sleep(60)



# ----------------------------------------
# START SERVER
if __name__ == "__main__":
    collector_thread = threading.Thread(target=collector_loop, daemon=True)
    schedule_thread = threading.Thread(target=run_scheduled_tasks, daemon=True)
    collector_thread.start()
    schedule_thread.start()
    app.run(debug=True)



# from flask import Flask, jsonify, render_template, request
# import requests
# import threading
# import time
# import csv
# import os
# from datetime import datetime

# app = Flask(__name__, template_folder="templates")

# THINGSBOARD_URL = "https://app.coreiot.io"
# DEVICE_ID = "1f5f2270-f990-11ef-a887-6d1a184f2bb5"
# JWT_TOKEN = "eyJhbGciOiJIUzUxMiJ9.eyJzdWIcQ"
# HEADERS = {"X-Authorization": f"Bearer {JWT_TOKEN}"}

# CSV_TEMP = "temperature_data.csv"
# CSV_HUM = "humidity_data.csv"
# CSV_GAS = "gas_data.csv"
# MAX_ROWS = 40

# def save_to_csv(file_path, timestamp, value):
#     rows = []
#     if os.path.exists(file_path):
#         with open(file_path, "r") as f:
#             rows = list(csv.reader(f))

#     rows.append([timestamp, value])

#     if len(rows) > MAX_ROWS:
#         rows = rows[-MAX_ROWS:]

#     with open(file_path, "w", newline="") as f:
#         writer = csv.writer(f)
#         writer.writerows(rows)

# @app.route("/dashboard")
# def dashboard():
#     return render_template("dashboard.html")

# @app.route("/tempChart")
# def temp_chart():
#     return render_template("temp_chart.html")

# @app.route("/humChart")
# def hum_chart():
#     return render_template("hum_chart.html")

# @app.route("/gasChart")
# def gas_chart():
#     return render_template("gas_chart.html")

# @app.route("/api/data")
# def get_data():
#     keys = "humidity,temperature,gas"
#     url = f"{THINGSBOARD_URL}/api/plugins/telemetry/DEVICE/{DEVICE_ID}/values/timeseries?keys={keys}"

#     response = requests.get(url, headers=HEADERS)
#     if response.status_code != 200:
#         return jsonify({"error": "ThingsBoard API error"}), 500

#     data = response.json()
#     result = {}
#     for key in data:
#         latest = data[key][-1]
#         value = latest["value"]
#         ts = int(latest["ts"]) / 1000
#         time_str = datetime.fromtimestamp(ts).strftime('%Y-%m-%d %H:%M:%S')
#         result[key] = {"value": value, "timestamp": time_str}

#     return jsonify(result)

# @app.route("/api/temp_history")
# def get_temp_history():
#     return load_csv_to_json(CSV_TEMP)

# @app.route("/api/hum_history")
# def get_hum_history():
#     return load_csv_to_json(CSV_HUM)

# @app.route("/api/gas_history")
# def get_gas_history():
#     return load_csv_to_json(CSV_GAS)

# def load_csv_to_json(file_path):
#     result = []
#     try:
#         with open(file_path, "r") as f:
#             reader = csv.reader(f)
#             for row in reader:
#                 result.append({"timestamp": row[0], "value": float(row[1])})
#     except FileNotFoundError:
#         pass

#     return jsonify(result)

# # -------------- BACKGROUND COLLECTOR ----------------
# def collector_loop():
#     while True:
#         try:
#             res = requests.get("http://localhost:5000/api/data")
#             if res.status_code == 200:
#                 data = res.json()

#                 temp_value = data["temperature"]["value"]
#                 temp_time = data["temperature"]["timestamp"]
#                 save_to_csv(CSV_TEMP, temp_time, temp_value)

#                 hum_value = data["humidity"]["value"]
#                 hum_time = data["humidity"]["timestamp"]
#                 save_to_csv(CSV_HUM, hum_time, hum_value)

#                 gas_value = data["gas"]["value"]
#                 gas_time = data["gas"]["timestamp"]
#                 save_to_csv(CSV_GAS, gas_time, gas_value)

#                 print(f"Saved: Temp={temp_value}, Hum={hum_value}, Gas={gas_value}")
#         except Exception as e:
#             print("Collector error:", e)

#         time.sleep(10)  # 10 seconds

# # -------------- MAIN START ----------------
# if __name__ == "__main__":
#     collector_thread = threading.Thread(target=collector_loop, daemon=True)
#     collector_thread.start()

#     app.run(debug=True)
