-- Smart Hospital Monitor — PostgreSQL schema
-- Run this in Supabase SQL Editor

CREATE TABLE IF NOT EXISTS hospital_safety_logs (
    id          SERIAL PRIMARY KEY,
    device_id   VARCHAR(50)  NOT NULL,
    temperature FLOAT8       NOT NULL,
    humidity    FLOAT8       NOT NULL,
    pressure    FLOAT8       NOT NULL,
    ai_verdict  TEXT,
    created_at  TIMESTAMP    DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX IF NOT EXISTS idx_device_time
    ON hospital_safety_logs (device_id, created_at DESC);