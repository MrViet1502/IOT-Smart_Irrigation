import pyrebase
from datetime import datetime
import hashlib

last_hash = None
firebase_config = {
    "apiKey": "AIzaSy======",
    "authDomain": "iot-esp32-hcmut.firebaseapp.com",
    "databaseURL": "https://iot-esp32-hcmut-default-rtdb.firebaseio.com",
    "projectId": "iot-esp32-hcmut",
    "storageBucket": "iot-esp32-hcmut.firebasestorage.app",
    "messagingSenderId": "713476===",
    "appId": "1:716013476369:web:43272bd3=====",
    "measurementId": "G-GFM==="
}

firebase = pyrebase.initialize_app(firebase_config)
db = firebase.database()

def log_sensor_data(data):
    global last_hash

    # Tạo hash từ nội dung data
    data_str = str(data)
    data_hash = hashlib.md5(data_str.encode()).hexdigest()

    if data_hash == last_hash:
        print("❌ Duplicate data detected – skip logging")
        return

    last_hash = data_hash

    from datetime import datetime
    timestamp = datetime.now().isoformat()
    entry = {
        "timestamp": timestamp,
        "soil_moisture": data.get("soil_moisture"),
        "temperature": data.get("temperature"),
        "humidity": data.get("humidity")
    }

    db.child("history").push(entry)
    print("✅ Logged to Firebase:", entry)
