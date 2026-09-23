# Fusion AI Workflow

Cloud orchestration pipeline for the Smart Hospital Monitor system.

## Pipeline

MQTT → Drive → Builder → Groq (LLM) → PostgreSQL → Google Sheets → Notifications

## Nodes

- **mqtt** — Receives sensor data from HiveMQ
- **drive** — Reads medication CSV from Google Drive
- **builder** — Parses data, prepares LLM prompt
- **groq** — LLM analysis (llama-3.3-70b-versatile)
- **postgres** — Persists to Supabase
- **formatter** — Formats for Google Sheets
- **Google Sheets** — Appends to Log_Data
- **If-Else** — Checks if alert needed
- **Format_Alert** — Builds alert text
- **Create + Docs_Formatter + Insert Content** — Google Docs report
- **Slack_text + Slack Action** — Slack notification
- **Gmail_text + Gmail** — Email notification

## Setup

1. Import `workflow.json` into Fusion AI
2. Configure secrets in Fusion AI → Settings → Secrets:
   - `MQTT_PASS`
   - `GROQ_API_KEY`
   - `GOOGLE_CLIENT_SECRET`
   - `GOOGLE_REFRESH_TOKEN`
   - `SLACK_BOT_TOKEN`
   - `SUPABASE_PASSWORD`