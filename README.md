# Hệ thống Tự động Tưới Cây Thông Minh (Smart Irrigation System)

## Mô tả Dự án

**Mục tiêu:**  
Hệ thống tự động tưới cây thông minh sẽ sử dụng cảm biến độ ẩm đất và cảm biến nhiệt độ/độ ẩm không khí để tự động điều khiển việc tưới cây. Khi độ ẩm của đất xuống dưới mức quy định, hệ thống sẽ tự động kích hoạt máy bơm để tưới cây và gửi cảnh báo về cho người dùng. Dữ liệu về độ ẩm đất, nhiệt độ và độ ẩm không khí sẽ được thu thập lên server trung tâm là coreIOT và hiển thị lên web để người dùng có thể theo dõi và điều khiển từ xa.

### **Chức năng chính:**

- **Giám sát độ ẩm của đất và môi trường xung quanh (nhiệt độ, độ ẩm) trên server (Web)**
- **Tự động tưới cây khi độ ẩm đất xuống dưới ngưỡng tối thiểu.**
- **Gửi cảnh báo khi có vấn đề (ví dụ: độ ẩm quá thấp, hệ thống bị lỗi).**
- **Hiển thị dữ liệu trực quan và điều khiển thiết bị qua ứng dụng web.**
- **Tích hợp chức năng Scheduler lên lịch trình thời gian tự động hệ thống tưới cây được thiết lập trên Web người dùng**
- **Tích hợp chức năng OTA Update, cập nhật firmware từ xa cho ESP32 thông qua Web người dùng**
- **Quét thẻ RFID hợp lệ để tưới cây trực tiếp, khi không cần qua serve.**
- **Tích hợp Edge Computing như tự động bật máy bơm và gửi cảnh báo khi dữ liệu vượt ngưỡng cho phép, kiểm tra dữ liệu thu được từ cảm biến không có thay đổi nhiều thì không gửi lên server, tiết kiệm dung lượng**

## Phần cứng sử dụng

- **ESP32**
- **Cảm biến độ ẩm đất**
- **Cảm biến DHT20** (đo nhiệt độ và độ ẩm)
- **Máy bơm nước mini**
- **RFID - RC522**

## Công nghệ sử dụng

### **Embedded Systems Development:**

- **ESP32** sử dụng nền tảng **PlatformIO** trong **VS Code** để lập trình và thu thập dữ liệu, điều khiển các cảm biến và thiết bị.

### **Web App Development:**

- **Backend (Python với Flask):** Xử lý API, giao tiếp với ESP32 và lưu trữ dữ liệu.
- **Frontend (HTML, CSS, js):** Xây dựng giao diện người dùng để hiển thị các thông tin về độ ẩm đất, nhiệt độ và điều khiển hệ thống.
- **Database (CoreIOT):** Lưu trữ dữ liệu từ cảm biến để truy xuất và hiển thị dữ liệu theo thời gian thực.

### **Edge Computing:**

- **ESP32** sẽ xử lý dữ liệu cảm biến ngay tại biên (tại thiết bị), đưa ra quyết định tưới cây mà không cần truyền tải dữ liệu đến đám mây liên tục, giúp giảm độ trễ và tối ưu hóa việc tưới cây.

## Timeline thực hiện

### **Tuần 1-2:**

- Nghiên cứu phần cứng, lập trình cho **ESP32** để thu thập dữ liệu từ các cảm biến.
- Điều khiển thiết bị và kết nối với **MQTT** (có thể dùng **CoreIoT** để test trước).

### **Tuần 3-4:**

- Phát triển ứng dụng **Web** với **Backend (Python Flask)** và **Frontend (React.js)**.

### **Tuần 5-6:**

- Tích hợp các phần lại với nhau trong hệ thống và tối ưu hóa hệ thống.

### **Tuần 7:**

- Hoàn thiện hệ thống, viết báo cáo, chuẩn bị mô hình hệ thống.
