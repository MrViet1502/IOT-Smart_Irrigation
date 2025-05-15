// toastr.options = {
//     "closeButton": true,
//     "progressBar": true,
//     "positionClass": "toast-top-right",
//     "timeOut": "3000"
//   }
// const csrfToken = document.querySelector('meta[name="csrf-token"]')?.getAttribute("content");
  
// function checkSession() {
//     fetch("/api/check_session")
//     .then(res => res.json())
//     .then(data => {
//         if (!data.logged_in) {
//             window.location.href = "/login";
//         }
//     }).catch(err => {
//         console.error("❌ Error checking session:", err);
//     });
// }


// function fetchData() {
//   fetch("/api/data", {
//     headers: { "X-CSRF-Token": csrfToken }
//   })
//   .then(res => res.status === 401 ? window.location.href = "/login" : res.json())
//   .then(data => {
//     if (!data) return;

//     const temp = parseFloat(data.temperature.value);
//     const humid = parseFloat(data.humidity.value);
//     const gas = parseFloat(data.gas.value);

//     document.getElementById("temp").innerText = temp + " °C";
//     document.getElementById("humid").innerText = humid + " %";
//     document.getElementById("gas").innerText = gas;

//     document.getElementById("temp_time").innerText = data.temperature.timestamp;
//     document.getElementById("humid_time").innerText = data.humidity.timestamp;
//     document.getElementById("gas_time").innerText = data.gas.timestamp;

//     if (thresholds.temperature && temp < thresholds.temperature) {
//       toastr.warning(`🌡️ Temperature below threshold: ${temp}°C`);
//     }
//     if (thresholds.humidity && humid < thresholds.humidity) {
//       toastr.warning(`💧 Humidity below threshold: ${humid}%`);
//     }
//     if (thresholds.gas && gas < thresholds.gas) {
//       toastr.warning(`🔥 Gas level below threshold: ${gas}`);
//     }
//   }).catch(err => console.error("❌ Không thể tải dữ liệu:", err));
//   document.getElementById("threshold-temp-label").innerText = "Ngưỡng: " + thresholds.temperature + " °C";

// }

// function fetchThresholds() {
//     fetch("/api/thresholds")
//         .then(res => res.json())
//         .then(data => {
//             thresholds.temperature = parseFloat(data.temperature) || null;
//             thresholds.humidity = parseFloat(data.humidity) || null;
//             thresholds.gas = parseFloat(data.gas) || null;
//         });
// }


// function sendCommand(command) {
//     fetch("/api/control", {
//         method: "POST",
//         headers: {
//             "Content-Type": "application/json",
//             "X-CSRF-Token": csrfToken
//         },
//         body: JSON.stringify({ command })
//     })
//     .then(res => res.json())
//     .then(result => {
//         if (result.status === "success") {
//             toastr.success(`✅ Đã gửi lệnh thành công! Trạng thái LED: ${result.led ? "BẬT" : "TẮT"}`);
//         } else {
//             toastr.error(`❌ Gửi lệnh thất bại: ${result.error}`);
//         }
//     })
//     .catch(err => {
//         console.error("❌ Lỗi khi gửi lệnh:", err);
//         toastr.error("❌ Lỗi khi gửi yêu cầu đến server");
//     });
// }

// const thresholds = {
//   temperature: null,
//   humidity: null,
//   gas: null
// };
// function openThresholdModal(type) {
//   $('#thresholdType').val(type);
//   $('#thresholdInput').val(thresholds[type] || '');
//   $('#thresholdModal').modal('show');
// }

// function saveThreshold() {
//   const type = document.getElementById("thresholdType").value;
//   const value = parseFloat(document.getElementById("thresholdInput").value);
//   if (isNaN(value)) {
//     toastr.error("⚠️ Please enter a valid number!");
//     return;
//   }
//   thresholds[type] = value;
//   toastr.success(`✅ Set ${type} threshold to ${value}`);
//   $('#thresholdModal').modal('hide');
// }


// window.onload = () => {
//     checkSession();
//     fetchData();
//     setInterval(fetchData, 5000);
// };



// Toastr cấu hình mặc định
toastr.options = {
    closeButton: true,
    progressBar: true,
    positionClass: "toast-top-right",
    timeOut: 3000
};

// Lấy CSRF token từ meta
const csrfToken = document.querySelector('meta[name="csrf-token"]')?.getAttribute("content");

// Lưu ngưỡng hiện tại
const thresholds = {
    temperature: null,
    humidity: null,
    gas: null
};

// Kiểm tra phiên đăng nhập
function checkSession() {
    fetch("/api/check_session")
        .then(res => res.json())
        .then(data => {
            if (!data.logged_in) window.location.href = "/login";
        });
}

// Lấy ngưỡng đã lưu từ server
function fetchThresholds() {
    fetch("/api/thresholds")
        .then(res => res.json())
        .then(data => {
            thresholds.temperature = parseFloat(data.temperature) || null;
            thresholds.humidity = parseFloat(data.humidity) || null;
            thresholds.gas = parseFloat(data.gas) || null;

            // ✅ Cập nhật hiển thị badge
            document.getElementById("threshold-temperature-label").innerText = thresholds.temperature !== null
                ? `Ngưỡng: ${thresholds.temperature} °C`
                : "Ngưỡng: chưa đặt";

            document.getElementById("threshold-humidity-label").innerText = thresholds.humidity !== null
                ? `Ngưỡng: ${thresholds.humidity} %`
                : "Ngưỡng: chưa đặt";

            document.getElementById("threshold-gas-label").innerText = thresholds.gas !== null
                ? `Ngưỡng: ${thresholds.gas}`
                : "Ngưỡng: chưa đặt";
        });
}


// Lấy dữ liệu cảm biến từ CoreIoT
function fetchData() {
    fetch("/api/data", {
        headers: { "X-CSRF-Token": csrfToken }
    })
    .then(res => res.status === 401 ? window.location.href = "/login" : res.json())
    .then(data => {
        if (!data) return;

        const temp = parseFloat(data.temperature.value);
        const humid = parseFloat(data.humidity.value);
        const gas = parseFloat(data.gas.value);

        // Hiển thị giá trị
        document.getElementById("temp").innerText = `${temp} °C`;
        document.getElementById("humid").innerText = `${humid} %`;
        document.getElementById("gas").innerText = gas;

        document.getElementById("temp_time").innerText = data.temperature.timestamp;
        document.getElementById("humid_time").innerText = data.humidity.timestamp;
        document.getElementById("gas_time").innerText = data.gas.timestamp;

        // Cảnh báo nếu dưới ngưỡng
        if (thresholds.temperature && temp < thresholds.temperature)
            toastr.warning(`🌡️ Nhiệt độ thấp hơn ngưỡng: ${temp}°C`);
        if (thresholds.humidity && humid < thresholds.humidity)
            toastr.warning(`💧 Độ ẩm thấp hơn ngưỡng: ${humid}%`);
        if (thresholds.gas && gas < thresholds.gas)
            toastr.warning(`🔥 Chỉ số khí Gas thấp hơn ngưỡng: ${gas}`);
    })
    .catch(err => {
        console.error("❌ Lỗi khi lấy dữ liệu:", err);
        toastr.error("❌ Mất kết nối đến máy chủ");
    });
}

// Gửi lệnh điều khiển LED
function sendCommand(command) {
    fetch("/api/control", {
        method: "POST",
        headers: {
            "Content-Type": "application/json",
            "X-CSRF-Token": csrfToken
        },
        body: JSON.stringify({ command })
    })
    .then(res => res.json())
    .then(result => {
        if (result.status === "success")
            toastr.success(`✅ Đã gửi lệnh LED ${result.led ? "BẬT" : "TẮT"}`);
        else
            toastr.error(`❌ Lỗi: ${result.error}`);
    })
    .catch(err => toastr.error("❌ Không thể gửi lệnh điều khiển"));
}

// Mở modal đặt ngưỡng
function openThresholdModal(type) {
    document.getElementById("thresholdType").value = type;
    document.getElementById("thresholdInput").value = thresholds[type] || '';
    $('#thresholdModal').modal('show');
}

// Lưu ngưỡng sau khi nhập
function saveThreshold() {
    const type = document.getElementById("thresholdType").value;
    const value = parseFloat(document.getElementById("thresholdInput").value);

    if (isNaN(value)) {
        toastr.error("⚠️ Vui lòng nhập giá trị hợp lệ");
        return;
    }

    thresholds[type] = value;

    fetch("/api/thresholds", {
        method: "POST",
        headers: {
            "Content-Type": "application/json",
            "X-CSRF-Token": csrfToken
        },
        body: JSON.stringify({ type, value })
    })
    .then(res => res.json())
    .then(result => {
        if (result.status === "saved") {
            toastr.success(`✅ Ngưỡng ${type} được lưu: ${value}`);
            document.getElementById(`threshold-${type}-label`).innerText = `Ngưỡng: ${value} ${type === 'temperature' ? '°C' : type === 'humidity' ? '%' : ''}`;
            $('#thresholdModal').modal('hide');
        } else {
            toastr.error(" Không thể lưu ngưỡng");
        }
    });
}

// Khởi động khi load trang
window.onload = () => {
    checkSession();
    fetchThresholds();  // Load ngưỡng đã lưu
    fetchData();
    setInterval(fetchData, 5000);
};
