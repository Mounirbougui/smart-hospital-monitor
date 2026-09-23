# MedSafe Dashboard

Real-time web dashboard for monitoring hospital storage conditions.

## Features

- Live sensor data via MQTT over WebSocket
- 4 metric cards (Temperature, Humidity, Pressure, SOS)
- Interactive charts (Chart.js)
- AI medication quality analysis (Groq LLM)
- Medication status tracking

## Run locally

Just open `index.html` in a modern browser.

## Configuration

Click **⚙ Configure** in the dashboard to enter:
- HiveMQ WebSocket URL
- MQTT credentials
- Groq API key