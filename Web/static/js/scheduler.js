// static/js/scheduler.js

toastr.options = {
  closeButton: true,
  progressBar: true,
  positionClass: "toast-top-right",
  timeOut: "3000"
};

const csrfToken = document.querySelector('meta[name="csrf-token"]').getAttribute("content");

function checkSession() {
  fetch("/api/check_session")
    .then(res => res.json())
    .then(data => {
      if (!data.logged_in) window.location.href = "/login";
    });
}

function addSchedule() {
  const time = document.getElementById("time").value;
  const command = document.getElementById("action").value;

  if (!time || !command) {
    toastr.error("⏰ Please fill all schedule fields!");
    return;
  }

  fetch("/api/schedule", {
    method: "POST",
    headers: {
      "Content-Type": "application/json",
      "X-CSRF-Token": csrfToken
    },
    body: JSON.stringify({ time, command })
  })
    .then(res => res.json())
    .then(result => {
      if (result.status === "added") {
        toastr.success(`📅 Scheduled ${command.toUpperCase()} at ${time}`);
        loadSchedules();
      } else {
        toastr.error("❌ Failed to add schedule");
      }
    });
}

function loadSchedules() {
  fetch("/api/schedule")
    .then(res => res.json())
    .then(data => {
      const list = document.getElementById("scheduleList");
      list.innerHTML = "";
      if (data.length === 0) {
        list.innerHTML = "<p class='text-muted'>No schedules set.</p>";
        return;
      }
      data.forEach((item, index) => {
        const div = document.createElement("div");
        div.className = "d-flex justify-content-between align-items-center border p-2 mb-1";
        div.innerHTML = `
          <span><strong>${item.time}</strong> → ${item.command.toUpperCase()}</span>
          <button class='btn btn-sm btn-danger' onclick='deleteSchedule(${index})'>Delete</button>
        `;
        list.appendChild(div);
      });
    });
}

function deleteSchedule(index) {
  fetch(`/api/schedule/${index}`, {
    method: "DELETE",
    headers: { "X-CSRF-Token": csrfToken }
  })
    .then(res => res.json())
    .then(result => {
      if (result.status === "deleted") {
        toastr.success("🗑️ Schedule deleted");
        loadSchedules();
      }
    });
}

function triggerOTA() {
  const url = document.getElementById("otaUrl").value.trim();
  if (!url.startsWith("http")) {
    toastr.error("❌ Invalid OTA URL!");
    return;
  }

  fetch("/api/control", {
    method: "POST",
    headers: {
      "Content-Type": "application/json",
      "X-CSRF-Token": csrfToken
    },
    body: JSON.stringify({ ota_url: url })
  })
    .then(res => res.json())
    .then(result => {
      if (result.status === "success") {
        toastr.success("✅ OTA command sent!");
      } else {
        toastr.error("❌ Failed to send OTA command");
      }
    })
    .catch(err => {
      console.error("Error sending OTA:", err);
      toastr.error("❌ OTA request error");
    });
}

window.onload = () => {
  checkSession();
  loadSchedules();

  document.getElementById("scheduleForm").addEventListener("submit", (e) => {
    e.preventDefault();
    addSchedule();
  });

  document.getElementById("otaBtn").addEventListener("click", triggerOTA);
};
