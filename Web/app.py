from flask import Flask, jsonify, render_template, request, session, redirect, url_for
from flask_wtf.csrf import CSRFProtect, generate_csrf
from datetime import datetime, timedelta
import requests
import threading
import time
import csv
import os

app = Flask(__name__, template_folder="templates")
app.secret_key = "qwertyuiop"  # Đặt key bảo mật riêng
csrf = CSRFProtect(app)

# ----------- Cấu hình IoT ThingsBoard -------------
THINGSBOARD_URL = "https://app.coreiot.io"
DEVICE_ID = "1f5f2270-f990-11ef-a887-6d1a184f2bb5"
JWT_TOKEN = "eyJhbGciOiJIUzUxMiJ9.eyJzdWIiOiJuLnF1b2N2aWV0MTUwMkBnbWFpbC5jb20iLCJ1c2VySWQiOiI5NDMyNTg5MC1lZTcwLTExZWYtODdiNS0yMWJjY2Y3ZDI5ZDUiLCJzY29wZXMiOlsiVEVOQU5UX0FETUlOIl0sInNlc3Npb25JZCI6ImQyMjMwMDQxLWJiNDktNDE3ZS1hMjRkLWE1Nzg4NGJjNDAzYSIsImV4cCI6MTc0NjczMDg1MCwiaXNzIjoiY29yZWlvdC5pbyIsImlhdCI6MTc0NjcyMTg1MCwiZmlyc3ROYW1lIjoiVmnhu4d0IiwibGFzdE5hbWUiOiJOZ3V54buFbiBRdeG7kWMiLCJlbmFibGVkIjp0cnVlLCJpc1B1YmxpYyI6ZmFsc2UsInRlbmFudElkIjoiOTQyYTkwNjAtZWU3MC0xMWVmLTg3YjUtMjFiY2NmN2QyOWQ1IiwiY3VzdG9tZXJJZCI6IjEzODE0MDAwLTFkZDItMTFiMi04MDgwLTgwODA4MDgwODA4MCJ9.lqvKDpv21dkRa7d-sAkKKaHIlAqf0Yj1tFt2HIB-2iQXvmdiHLGz_B5EZfccBG9RvpDh1VfLw-ISUf97zfe6Gw"
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
    command = payload.get("command")

    if command not in ["on", "off"]:
        return jsonify({"error": "Invalid command"}), 400

    led_state = True if command == "on" else False
    url = f"{THINGSBOARD_URL}/api/plugins/telemetry/DEVICE/{DEVICE_ID}/attributes/SHARED_SCOPE"
    body = {"led": led_state}

    try:
        response = requests.post(url, headers=HEADERS, json=body, timeout=5)
        response.raise_for_status()
        return jsonify({"status": "success", "led": led_state})
    except requests.exceptions.RequestException as e:
        return jsonify({"error": f"Request exception: {str(e)}"}), 500

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

        time.sleep(10)

# ----------------------------------------
# START SERVER
if __name__ == "__main__":
    collector_thread = threading.Thread(target=collector_loop, daemon=True)
    collector_thread.start()

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
# JWT_TOKEN = "eyJhbGciOiJIUzUxMiJ9.eyJzdWIiOiJuLnF1b2N2aWV0MTUwMkBnbWFpbC5jb20iLCJ1c2VySWQiOiI5NDMyNTg5MC1lZTcwLTExZWYtODdiNS0yMWJjY2Y3ZDI5ZDUiLCJzY29wZXMiOlsiVEVOQU5UX0FETUlOIl0sInNlc3Npb25JZCI6IjMyZGFiNjYzLWUxZGQtNDBhYy04NjllLTNhZmFmM2YzNGM3MCIsImV4cCI6MTc0NjcyMTgyMSwiaXNzIjoiY29yZWlvdC5pbyIsImlhdCI6MTc0NjcxMjgyMSwiZmlyc3ROYW1lIjoiVmnhu4d0IiwibGFzdE5hbWUiOiJOZ3V54buFbiBRdeG7kWMiLCJlbmFibGVkIjp0cnVlLCJpc1B1YmxpYyI6ZmFsc2UsInRlbmFudElkIjoiOTQyYTkwNjAtZWU3MC0xMWVmLTg3YjUtMjFiY2NmN2QyOWQ1IiwiY3VzdG9tZXJJZCI6IjEzODE0MDAwLTFkZDItMTFiMi04MDgwLTgwODA4MDgwODA4MCJ9.rxJciLSX7o51cxp4iGXyXCE6pg5Ouc8Z9aZdrbL_UaDV1keQmd6_rYYa1MJkhk5ZeZvbFKo8gTzsECdfaUWIcQ"
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
