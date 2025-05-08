// chart_config.js
const chartConfigs = {
    temperature: {
        canvasId: "temperatureChart",
        apiUrl: "/api/temp_history",
        label: "Temperature (°C)",
        unit: "°C",
        interval: 5000
    },
    humidity: {
        canvasId: "humidityChart",
        apiUrl: "/api/hum_history",
        label: "Humidity (%)",
        unit: "%",
        interval: 5000
    },
    gas: {
        canvasId: "gasChart",
        apiUrl: "/api/gas_history",
        label: "Gas (%)",
        unit: "%",
        interval: 10000
    }
};
