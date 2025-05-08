function createChart(config) {
    const ctx = document.getElementById(config.canvasId).getContext('2d');
    const chart = new Chart(ctx, {
        type: 'line',
        data: {
            labels: [],
            datasets: [{
                label: config.label,
                data: [],
                backgroundColor: 'rgba(52, 160, 233, 0.3)',
                borderColor: 'rgba(54, 162, 235, 1)',
                borderWidth: 2,
                fill: true,
                tension: 0.3,
                pointRadius: 3
            }]
        },
        options: {
            responsive: true,
            scales: {
                x: { title: { display: true, text: "Time" } },
                y: { title: { display: true, text: config.label }, beginAtZero: false }
            }
        }
    });

    function fetchData() {
        fetch(config.apiUrl)
            .then(response => response.json())
            .then(data => {
                const labels = data.map(item => item.timestamp);
                const values = data.map(item => item.value);

                chart.data.labels = labels;
                chart.data.datasets[0].data = values;
                chart.update();
            });
    }

    fetchData();
    setInterval(fetchData, config.interval);
}
