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
    soil: {
        canvasId: "soilChart",
        apiUrl: "/api/soil_history",
        label: "Soil (%)",
        unit: "%",
        interval: 10000
    }
};
