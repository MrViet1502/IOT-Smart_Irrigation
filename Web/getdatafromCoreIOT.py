
# import requests
# from datetime import datetime

# # 🔐 Dán JWT token bạn đã lấy được (login bằng email/password)
# JWT_TOKEN = "eyJhbGciOiJIUzUxMiJ9.eyJzdWIiOiJuLnF1b2N2aWV0MTUwMkBnbWFpbC5jb20iLCJ1c2VySWQiOiI5NDMyNTg5MC1lZTcwLTExZWYtODdiNS0yMWJjY2Y3ZDI5ZDUiLCJzY29wZXMiOlsiVEVOQU5UX0FETUlOIl0sInNlc3Npb25JZCI6IjZmODg5ZWZlLTUwMTMtNDBkYi1hM2JjLTRiMGZkZjNhYjdmOSIsImV4cCI6MTc0MzU2MDk5NywiaXNzIjoiY29yZWlvdC5pbyIsImlhdCI6MTc0MzU1MTk5NywiZmlyc3ROYW1lIjoiVmnhu4d0IiwibGFzdE5hbWUiOiJOZ3V54buFbiBRdeG7kWMiLCJlbmFibGVkIjp0cnVlLCJpc1B1YmxpYyI6ZmFsc2UsInRlbmFudElkIjoiOTQyYTkwNjAtZWU3MC0xMWVmLTg3YjUtMjFiY2NmN2QyOWQ1IiwiY3VzdG9tZXJJZCI6IjEzODE0MDAwLTFkZDItMTFiMi04MDgwLTgwODA4MDgwODA4MCJ9.7ohE1juY99-DT1PSziadLs9LGlbfF1opvoqpQGcykzt26ko3xv96z9PBbbSUFa009VaXrYlRH6YmopGGRHdUwA"  # 🔁 Thay bằng token thật của bạn

# # 🔎 Device ID bạn đã cung cấp
# DEVICE_ID = "1f5f2270-f990-11ef-a887-6d1a184f2bb5"

# # 🌐 Base URL của CoreIoT / ThingsBoard
# BASE_URL = "https://app.coreiot.io"

# # 🚀 Gọi API lấy dữ liệu telemetry
# url = f"{BASE_URL}/api/plugins/telemetry/DEVICE/{DEVICE_ID}/values/timeseries?keys=humidity"

# headers = {
#     "X-Authorization": f"Bearer {JWT_TOKEN}"
# }

# res = requests.get(url, headers=headers)

# if res.status_code != 200:
#     print(f"❌ Lỗi khi lấy dữ liệu telemetry: {res.status_code}")
#     print(res.text)
#     exit()

# data = res.json()

# if "humidity" in data:
#     latest = data["humidity"][-1]
#     value = latest["value"]
#     ts = int(latest["ts"]) / 1000
#     time_str = datetime.fromtimestamp(ts).strftime('%Y-%m-%d %H:%M:%S')

#     print(f"✅ Độ ẩm mới nhất: {value}%")
#     print(f"🕒 Gửi lúc: {time_str}")
# else:
#     print("⚠️ Không tìm thấy dữ liệu 'humidity'")

#  ++++++++++++++++ 


import requests
from datetime import datetime

# 🔐 JWT token bạn đã có
JWT_TOKEN = "eyJhbGciOiJIUzUxMiJ9.++"
DEVICE_ID = "1=="
BASE_URL = "https://app.coreiot.io"

# 🚀 Gọi API lấy nhiều telemetry keys cùng lúc
keys = "humidity,temperature,mq2_analog"
url = f"{BASE_URL}/api/plugins/telemetry/DEVICE/{DEVICE_ID}/values/timeseries?keys={keys}"

headers = {
    "X-Authorization": f"Bearer {JWT_TOKEN}"
}

res = requests.get(url, headers=headers)

if res.status_code != 200:
    print(f"❌ Lỗi khi lấy dữ liệu telemetry: {res.status_code}")
    print(res.text)
    exit()

data = res.json()

def print_telemetry(key):
    if key in data:
        latest = data[key][-1]
        value = latest["value"]
        ts = int(latest["ts"]) / 1000
        time_str = datetime.fromtimestamp(ts).strftime('%Y-%m-%d %H:%M:%S')
        print(f"🔹 {key.capitalize()}: {value} (⏰ {time_str})")
    else:
        print(f"⚠️ Không có dữ liệu '{key}'")

# 🔍 In ra từng loại dữ liệu
print_telemetry("humidity")
print_telemetry("temperature")
print_telemetry("mq2_analog")


