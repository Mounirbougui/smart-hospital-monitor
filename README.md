# AiVital Track — Smart Hospital & Patient Monitor

**End-to-end IoT + Cloud + AI system for real-time biometric and environmental monitoring in healthcare facilities.**

[![ESP32](https://img.shields.io/badge/ESP32-MakerBoard-blue?logo=espressif)](https://www.espressif.com/)
[![MQTT](https://img.shields.io/badge/MQTT-HiveMQ%20Cloud-purple?logo=mqtt)](https://www.hivemq.com/)
[![AI](https://img.shields.io/badge/AI-Groq%20LLaMA%203.3-orange)](https://groq.com/)
[![PostgreSQL](https://img.shields.io/badge/PostgreSQL-Supabase-336791?logo=postgresql)](https://supabase.com/)
[![License](https://img.shields.io/badge/license-MIT-green)](LICENSE)

---

## Overview

**AiVital Track** is an edge-to-cloud monitoring platform that continuously tracks patient vitals and environmental conditions in hospital rooms, detects anomalies in real time, and orchestrates intelligent multi-channel alerts to medical staff.

The system combines embedded biometric sensors, secure IoT communication, cloud orchestration, LLM-based medical assessment, and relational persistence — all wired into a single automated pipeline.

---

## Features

- **Patient Vitals Monitoring** — Heart rate and SpO₂ via MAX30100; contactless body temperature via MLX90614.
- **Environmental & Motion Tracking** — Room temperature, humidity and pressure via BME280; fall and movement detection via MPU6050.
- **Interactive Edge Interface** — Live readings on an I²C LCD, with a 4×4 keypad for staff authentication and manual triggers.
- **Local Alerting** — Hardware SOS button, warning LEDs, and an active buzzer for immediate on-site signalling.
- **Real-time Web Dashboard** — Chart.js visualisation over MQTT/WebSocket, accessible from any nurse station browser.
- **AI-Powered Assessment** — Groq LLaMA 3.3 evaluates combined telemetry against medical baselines and classifies alert severity.
- **Automated Incident Reports** — Google Docs reports generated automatically on every critical event.
- **Multi-Channel Notifications** — Slack messages and HTML emails with direct links to the incident report.
- **Relational Persistence** — Every event logged to PostgreSQL for auditing and later analysis.

---

## Architecture

```
┌──────────────────────────────────────────────┐
│  ESP32 Edge Node (MakerBoard)                │
│  ├─ Vitals        : MAX30100, MLX90614       │
│  ├─ Environment   : BME280                   │
│  ├─ Motion        : MPU6050                  │
│  └─ UI & Alerts   : I²C LCD, Keypad, LEDs,   │
│                     Buzzer, SOS Button       │
└──────────────────────┬───────────────────────┘
                       │ MQTT over TLS (8883)
                       ▼
              ┌──────────────────┐
              │   HiveMQ Cloud   │
              └────────┬─────────┘
                       │
                       ▼
        ┌────────────────────────────────┐
        │   Fusion AI Orchestration      │
        │   ├─ Groq LLM analysis         │
        │   ├─ PostgreSQL persistence    │
        │   └─ Google Sheets logging     │
        └────────┬───────────────────────┘
                 │
     ┌───────────┼───────────┬──────────────┐
     ▼           ▼           ▼              ▼
 ┌────────┐ ┌────────┐ ┌───────────┐ ┌───────────┐
 │ Slack  │ │ Gmail  │ │ GoogleDoc │ │ Dashboard │
 └────────┘ └────────┘ └───────────┘ └───────────┘
```

---

## Tech Stack

| Layer | Technology |
|-------|------------|
| **Hardware** | ESP32 MakerBoard, MAX30100, MLX90614, MPU6050, BME280, I²C LCD, 4×4 Keypad, LEDs, Buzzer |
| **Firmware** | C++ / Arduino (Wire, PubSubClient, ArduinoJson) |
| **Protocols** | MQTT over TLS/SSL (port 8883), I²C |
| **Broker** | HiveMQ Cloud |
| **Orchestration** | Fusion AI (visual workflow) |
| **AI / LLM** | Groq API — `llama-3.3-70b-versatile` |
| **Database** | PostgreSQL (Supabase) |
| **Notifications** | Slack API, Gmail API, Google Docs API |
| **Frontend** | HTML5, CSS3, Chart.js, MQTT.js |

---

## Screenshots

> Add images to `docs/screenshots/` using the filenames below.

| Preview | File |
|---------|------|
| Live Dashboard | `docs/screenshots/dashboard.png` |
| Slack Alert | `docs/screenshots/slack-alert.png` |
| Gmail Notification | `docs/screenshots/gmail-alert.png` |
| Google Docs Report | `docs/screenshots/gdocs-report.png` |
| Supabase Database | `docs/screenshots/supabase.png` |
| Fusion AI Workflow | `docs/screenshots/fusion-ai-workflow.png` |

---

## How It Works

1. **Edge Sampling** — The ESP32 polls the I²C bus continuously, collecting data from MAX30100, MLX90614, MPU6050 and BME280.
2. **Local Processing** — Readings are averaged over a sliding window to filter noise and displayed live on the I²C LCD.
3. **Secure Transmission** — Averaged packets are published over MQTT/TLS to HiveMQ. The physical SOS button bypasses the averaging window and publishes immediately.
4. **Cloud Orchestration** — Fusion AI receives the MQTT payload and triggers the pipeline.
5. **AI Assessment & Storage** — A Groq LLaMA 3.3 model evaluates the telemetry against medical baselines; all readings are persisted to PostgreSQL.
6. **Alert Escalation** — When an anomaly is detected, a Google Docs incident report is generated and notifications are dispatched via Slack and Gmail.
7. **Live Dashboard** — Staff can follow real-time charts from any browser via the WebSocket dashboard.

---

## Installation & Setup

### Prerequisites

- Arduino IDE with ESP32 board support
- Libraries: `PubSubClient`, `ArduinoJson`, `Adafruit_MAX30100`, `Adafruit_MLX90614`, `Adafruit_MPU6050`, `Adafruit_BME280`, `LiquidCrystal_I2C`, `Keypad`
- Accounts: HiveMQ Cloud, Supabase, Groq, Google Cloud (Docs, Sheets, Drive, Gmail APIs), Slack

### 1. Firmware

```bash
git clone https://github.com/mounirbougui/smart-hospital-monitor.git
cd smart-hospital-monitor/firmware
cp secrets.h.example secrets.h
```

Edit `secrets.h` with your Wi-Fi and MQTT credentials. This file is **gitignored** and never leaves your machine.

Open `smart_hospital_monitor.ino` in Arduino IDE, select the ESP32 board, and upload.

### 2. Cloud Workflow (Fusion AI)

1. Create a new workflow in Fusion AI.
2. Import `fusion-ai/workflow.json`.
3. Register secrets in Fusion AI → Settings → Secrets:
   - `MQTT_PASS`
   - `GROQ_API_KEY`
   - `GOOGLE_CLIENT_SECRET`
   - `GOOGLE_REFRESH_TOKEN`
   - `SLACK_BOT_TOKEN`
   - `SUPABASE_PASSWORD`
4. Deploy the workflow.

### 3. Database

Run `database/schema.sql` in the Supabase SQL Editor.

### 4. Dashboard

Open `dashboard/index.html` in a modern browser, click **Configure**, and enter the HiveMQ WebSocket URL and credentials.

---

## Project Structure

```
smart-hospital-monitor/
├── firmware/               # ESP32 C++ source code
│   ├── smart_hospital_monitor.ino
│   ├── secrets.h.example   # Credentials template
│   └── secrets.h           # Local only (gitignored)
├── fusion-ai/              # Cloud orchestration
│   ├── workflow.json
│   └── README.md
├── dashboard/              # Real-time frontend
│   ├── index.html
│   └── README.md
├── database/               # PostgreSQL schema
│   └── schema.sql
├── docs/                   # Documentation & screenshots
│   └── screenshots/
├── README.md
├── LICENSE
├── .gitignore
└── .env.example
```

---

## Test Results

| Test | Scenario | Outcome |
|------|----------|---------|
| 1 | Nominal regime | All readings within range, `alert = false` |
| 2 | Temperature threshold exceeded | LEDs + buzzer triggered, full notification pipeline fired |
| 3 | SOS button pressed | Immediate MQTT publish, incident report generated, Slack + Gmail notified |

**Database:** 451 records logged  
**HiveMQ console:** 14 messages received on `hospital/room1/sensors`

---

## Roadmap

- [ ] X.509 certificate authentication for MQTT
- [ ] Multi-room deployment with centralised aggregation
- [ ] SMS alerts via Twilio
- [ ] Predictive ML model for thermal drift anticipation
- [ ] Mobile companion app (React Native)

---

## License

Released under the **MIT License** — see [LICENSE](LICENSE) for details.

---

## Author

**Mounir Bougui**

- GitHub: [@mounirbougui](https://github.com/mounirbougui)
- LinkedIn: [Mounir Bougui](https://linkedin.com/in/mounir-bougui)
- Email: bouguimounir875@gmail.com

---

⭐ If you find this project useful, consider giving it a star.
