let soilChart, tempChart, humidChart;

function setupCharts() {
    const soilCtx = document.getElementById('soilChart').getContext('2d');
    const tempCtx = document.getElementById('tempChart').getContext('2d');
    const humidCtx = document.getElementById('humidChart').getContext('2d');

    soilChart = createLineChart(soilCtx, 'Độ ẩm đất (%)', 'green');
    tempChart = createLineChart(tempCtx, 'Nhiệt độ (°C)', 'orange');
    humidChart = createLineChart(humidCtx, 'Độ ẩm không khí (%)', 'blue');
}

function createLineChart(ctx, label, color) {
    return new Chart(ctx, {
        type: 'line',
        data: {
            labels: [],
            datasets: [{
                label: label,
                data: [],
                borderWidth: 2,
                borderColor: color,
                fill: false
            }]
        },
        options: {
            responsive: true,
            scales: {
                x: { title: { display: true, text: 'Thời gian' } },
                y: { title: { display: true, text: label }, beginAtZero: true }
            }
        }
    });
}

function updateUI(data) {
    document.getElementById("soil").innerText = data.soil_moisture + " %";
    document.getElementById("temp").innerText = data.temperature + " °C";
    document.getElementById("humid").innerText = data.humidity + " %";

    const now = new Date().toLocaleTimeString();
    addData(soilChart, now, data.soil_moisture);
    addData(tempChart, now, data.temperature);
    addData(humidChart, now, data.humidity);
}

function updateSensorData() {
    fetch('/data')
        .then(response => {
            if (!response.ok) throw new Error("Flask không phản hồi");
            return response.json();
        })
        .then(data => {
            updateUI(data);
        })
        .catch(err => {
            console.warn("🛑 Flask error:", err);
            fetchFromFirebase();
        });
}

function fetchFromFirebase() {
    const firebaseURL = "https://iot-esp32-hcmut-default-rtdb.firebaseio.com/history.json?orderBy=\"$key\"&limitToLast=1";
    fetch(firebaseURL)
        .then(res => res.json())
        .then(data => {
            const keys = Object.keys(data);
            const latest = data[keys[0]];
            updateUI(latest);
        });
}

setInterval(updateSensorData, 3000);
window.onload = () => {
    setupCharts();
    updateSensorData();
};

function sendControl(action) {
    fetch(`/control?action=${action}`)
      .then(res => res.json())
      .then(data => alert("Đã gửi: " + data.action));
}
