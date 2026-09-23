# Smart Hospital Monitor

**IoT + AI system for real-time pharmaceutical storage monitoring**

[![ESP32](https://img.shields.io/badge/ESP32-MakerBoard-blue?logo=espressif)](https://www.espressif.com/)
[![MQTT](https://img.shields.io/badge/MQTT-HiveMQ%20Cloud-purple?logo=mqtt)](https://www.hivemq.com/)
[![AI](https://img.shields.io/badge/AI-Groq%20LLaMA%203.3-orange)](https://groq.com/)
[![PostgreSQL](https://img.shields.io/badge/PostgreSQL-Supabase-336791?logo=postgresql)](https://supabase.com/)
[![License](https://img.shields.io/badge/license-MIT-green)](LICENSE)

An end-to-end IoT + Cloud + AI pipeline that monitors pharmaceutical storage conditions (temperature, humidity, pressure) in real time, detects anomalies, and automatically alerts medical staff via multiple channels to prevent medication spoilage.

---

## Overview

Pharmaceutical products (vaccines, insulin, chemotherapy agents) require strict environmental conditions. A brief exposure to wrong temperature or humidity can chemically alter medications and compromise patient safety.

**Smart Hospital Monitor** solves this by combining:

- 🔌 Embedded environmental sensors (ESP32 + BME280)
- 📡 Secure IoT communication (MQTT over TLS)
- 🧠 Cloud orchestration + LLM analysis (Fusion AI + Groq LLaMA 3.3)
- 💾 Relational persistence (PostgreSQL on Supabase)
- 📢 Multi-channel notifications (Slack, Gmail, Google Docs)

---

## Features

- **Real-time monitoring** — Temperature, humidity and pressure sampled every 300 ms
- **6-second averaging window** — Reduces noise and prevents false alerts
- **SOS emergency button** — Instant MQTT publish with edge detection (no waiting)
- **Local display & signalling** — TM1637 4-digit display, status LEDs, intermittent buzzer
- **AI-powered assessment** — Groq LLaMA 3.3 evaluates conditions against a medication inventory and classifies alert severity
- **Automated incident reports** — Google Docs reports generated automatically on every critical event
- **Multi-channel alerts** — Slack channel notifications and HTML emails with direct links to the incident report
- **Relational persistence** — Every event logged to PostgreSQL for auditing
- **Live web dashboard** — Chart.js visualization over MQTT/WebSocket for real-time monitoring

---

## Architecture

```
┌──────────────────────────────────────────┐
│ ESP32 Edge Node (MakerBoard)             │
│ ├─ Sensors : BME280 (T°, H%, P)          │
│ └─ UI/Alerts: TM1637, LEDs, Buzzer, SOS  │
└──────────────────┬───────────────────────┘
                   │ MQTT over TLS (8883)
                   ▼
          ┌──────────────────┐
          │   HiveMQ Cloud   │
          └────────┬─────────┘
                   │
                   ▼
      ┌────────────────────────────────┐
      │ Fusion AI Orchestration        │
      │ ├─ Groq LLM assessment         │
      │ ├─ PostgreSQL persistence      │
      │ └─ Google Sheets logging       │
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
| **Hardware** | ESP32 MakerBoard, BME280, TM1637 display, LEDs, buzzer, SOS buttons |
| **Firmware** | C++ / Arduino (Wire, PubSubClient, ArduinoJson, NextBME, NextTM1637) |
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

1. **Edge Sampling** — The ESP32 reads BME280 every 300 ms and accumulates samples.
2. **Local Processing** — Every 6 seconds, averages are computed and displayed on the TM1637 display.
3. **Secure Transmission** — Averaged packets are published over MQTT/TLS to HiveMQ. The physical SOS button bypasses the averaging window for an immediate emergency publish.
4. **Cloud Orchestration** — Fusion AI receives the MQTT payload and triggers the pipeline.
5. **AI Assessment & Storage** — A Groq LLaMA 3.3 model evaluates the environmental data against a medication inventory; all readings are persisted to PostgreSQL.
6. **Alert Escalation** — When an anomaly is detected, a Google Docs incident report is generated and notifications are dispatched via Slack and Gmail.
7. **Live Dashboard** — Staff can follow real-time charts from any browser via the WebSocket dashboard.

---

## Installation & Setup

### Prerequisites

- Arduino IDE with ESP32 board support
- Libraries: `PubSubClient`, `ArduinoJson`, `NextBME`, `NextTM1637`
- Accounts: HiveMQ Cloud, Supabase, Groq, Google Cloud (Docs, Sheets, Drive, Gmail APIs), Slack

### 1. Firmware

```bash
git clone https://github.com/Mounirbougui/smart-hospital-monitor.git
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
│   ├── rapport_technique.pdf
│   └── screenshots/
├── README.md
└── LICENSE
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

---

## License

Released under the **MIT License** — see [LICENSE](LICENSE) for details.

---

## Author

**Mounir Bougui**

- GitHub: [@Mounirbougui](https://github.com/Mounirbougui)
- LinkedIn: [Mounir Bougui](https://www.linkedin.com/in/mounir-bougui)
- Email: bouguimounir875@gmail.com

---

⭐ If you find this project useful, consider giving it a star.
