toastr.options = {
    "closeButton": true,
    "progressBar": true,
    "positionClass": "toast-bottom-right",
    "timeOut": "3000"
  }
const csrfToken = document.querySelector('meta[name="csrf-token"]')?.getAttribute("content");
  
function checkSession() {
    fetch("/api/check_session")
    .then(res => res.json())
    .then(data => {
        if (!data.logged_in) {
            window.location.href = "/login";
        }
    }).catch(err => {
        console.error("❌ Error checking session:", err);
    });
}

function fetchData() {
    fetch("/api/data", {
        headers: { "X-CSRF-Token": csrfToken }
    })
    .then(res => res.status === 401 ? window.location.href = "/login" : res.json())
    .then(data => {
        if (!data) return;
        document.getElementById("temp").innerText = data.temperature.value + " °C";
        document.getElementById("humid").innerText = data.humidity.value + " %";
        document.getElementById("gas").innerText = data.gas.value;
        document.getElementById("temp_time").innerText = data.temperature.timestamp;
        document.getElementById("humid_time").innerText = data.humidity.timestamp;
        document.getElementById("gas_time").innerText = data.gas.timestamp;
    }).catch(err => console.error("❌ Không thể tải dữ liệu:", err));
}

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
        if (result.status === "success") {
            toastr.success(`✅ Đã gửi lệnh thành công! Trạng thái LED: ${result.led ? "BẬT" : "TẮT"}`);
        } else {
            toastr.error(`❌ Gửi lệnh thất bại: ${result.error}`);
        }
    })
    .catch(err => {
        console.error("❌ Lỗi khi gửi lệnh:", err);
        toastr.error("❌ Lỗi khi gửi yêu cầu đến server");
    });
}

window.onload = () => {
    checkSession();
    fetchData();
    setInterval(fetchData, 5000);
};



