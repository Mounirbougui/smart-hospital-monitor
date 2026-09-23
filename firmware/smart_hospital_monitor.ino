#include <Wire.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <NextBME.h>
#include <NextTM1637.h>
#include "secrets.h"

const char* MQTT_TOPIC    = "hospital/room1/sensors";
const char* DEVICE_ID     = "MakerBoard_01";

#define PIN_SDA       21
#define PIN_SCL       22
#define PIN_TM_CLK    18
#define PIN_TM_DIO     5
#define PIN_BUZZER    26
#define PIN_LED1      33
#define PIN_LED2      32
#define PIN_BTN_A     35
#define PIN_BTN_B     34

#define TEMP_HIGH     33.0f
#define TEMP_LOW      15.0f
#define HUM_HIGH      80.0f
#define HUM_LOW       20.0f

#define BTN_PRESSED_STATE  LOW

#define WINDOW_MS       6000   // normal publish every 6s
#define SAMPLE_EVERY_MS  300   // sample every 300ms (~20 samples/window)
#define BEEP_ON_MS       200
#define BEEP_OFF_MS     1800

NextBME          bmeSensor;
NextTM1637       tmDisplay(PIN_TM_CLK, PIN_TM_DIO);
WiFiClientSecure wifiClient;
PubSubClient     mqttClient(wifiClient);

float         sumTemp     = 0, sumHum = 0, sumPress = 0;
int           sampleCount = 0;
bool          sosDetected = false;

float         avgTemp  = 0, avgHum = 0, avgPress = 0;
bool          avgSos   = false, avgAlert = false;

unsigned long windowStart  = 0;
unsigned long lastSample   = 0;
unsigned long lastBeepTime = 0;
bool          beepState    = false;
bool          lastBtnState = false;  // for edge detection on SOS

// ═══════════════════════════════════════════════════════════
void setup() {
  pinMode(PIN_BUZZER, OUTPUT);
  digitalWrite(PIN_BUZZER, LOW);
  Serial.begin(115200);
  delay(200);
  Serial.println("\n=== Smart Hospital Monitor FINAL ===");

  pinMode(PIN_LED1, OUTPUT);
  pinMode(PIN_LED2, OUTPUT);
  pinMode(PIN_BTN_A, INPUT);
  pinMode(PIN_BTN_B, INPUT);
  digitalWrite(PIN_LED1, LOW);
  digitalWrite(PIN_LED2, LOW);

  tmDisplay.begin();
  tmDisplay.setBrightness(5);
  tmDisplay.showNumber(8888, false);
  delay(1000);
  tmDisplay.clear();

  Wire.begin(PIN_SDA, PIN_SCL);
  if (!bmeSensor.begin(0x77) && !bmeSensor.begin(0x76)) {
    Serial.println("[ERROR] BME280 not found!");
    tmDisplay.showNumber(9999, false);
    while (1) delay(1000);
  }
  Serial.println("[OK] BME280 ready");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  WiFi.setSleep(false);
  Serial.printf("[WiFi] Connecting to %s", WIFI_SSID);
  for (int i = 0; i < 30 && WiFi.status() != WL_CONNECTED; i++) {
    delay(500); Serial.print(".");
  }
  if (WiFi.status() == WL_CONNECTED)
    Serial.printf("\n[WiFi] Connected! IP: %s\n", WiFi.localIP().toString().c_str());
  else
    Serial.println("\n[WiFi] FAILED");

  wifiClient.setInsecure();
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  reconnectMQTT();

  windowStart = millis();
  lastSample  = millis();
  Serial.println("[OK] Setup complete.");
}

// ═══════════════════════════════════════════════════════════
void loop() {
  if (!mqttClient.connected()) reconnectMQTT();
  mqttClient.loop();

  unsigned long now = millis();

  // ── Read buttons every loop (10ms) for fast SOS detection ──
  bool btnA    = (digitalRead(PIN_BTN_A) == BTN_PRESSED_STATE);
  bool btnB    = (digitalRead(PIN_BTN_B) == BTN_PRESSED_STATE);
  bool btnNow  = btnA || btnB;
  bool newPress = btnNow && !lastBtnState;  // rising edge only
  lastBtnState  = btnNow;

  // ── If button just pressed → publish IMMEDIATELY with SOS=true ──
  if (newPress) {
    Serial.println("\n[SOS] Button pressed — publishing immediately!");
    // Take a fresh single reading for the emergency publish
    float t = bmeSensor.readTemperature();
    float h = bmeSensor.readHumidity();
    float p = bmeSensor.readPressure();
    if (!isnan(t)) avgTemp  = t;
    if (!isnan(h)) avgHum   = h;
    if (!isnan(p)) avgPress = p;
    avgSos   = true;
    avgAlert = true;
    publishData();
  }

  // ── Latch SOS for current window ──
  if (btnNow) sosDetected = true;

  // ── Sample every 300ms ──
  if (now - lastSample >= SAMPLE_EVERY_MS) {
    lastSample = now;

    float t = bmeSensor.readTemperature();
    float h = bmeSensor.readHumidity();
    float p = bmeSensor.readPressure();  // NextBME returns hPa directly

    if (!isnan(t) && !isnan(h) && !isnan(p)) {
      sumTemp  += t;
      sumHum   += h;
      sumPress += p;
      sampleCount++;
    }
  }

  // ── End of 6-second window → publish averaged data ──
  if (now - windowStart >= WINDOW_MS) {
    if (sampleCount > 0) {
      avgTemp  = sumTemp  / sampleCount;
      avgHum   = sumHum   / sampleCount;
      avgPress = sumPress / sampleCount;
    }

    avgSos   = sosDetected;
    avgAlert = (avgTemp > TEMP_HIGH) || (avgTemp < TEMP_LOW) ||
               (avgHum  > HUM_HIGH)  || (avgHum  < HUM_LOW)  || avgSos;

    // Display: 28.77 x 100 = 2877 → shows 28:77
    int displayTemp = (int)(avgTemp * 100 + 0.5f);
    tmDisplay.showNumber(displayTemp, false);
    tmDisplay.setColon(true);

    digitalWrite(PIN_LED1, HIGH);
    digitalWrite(PIN_LED2, avgAlert ? HIGH : LOW);

    Serial.printf("\n[6s AVG | %d samples]\n", sampleCount);
    Serial.printf("  Temp  : %.2f C\n",   avgTemp);
    Serial.printf("  Hum   : %.2f %%\n",  avgHum);
    Serial.printf("  Press : %.2f hPa\n", avgPress);
    Serial.printf("  SOS   : %s\n",       avgSos   ? "TRUE" : "false");
    Serial.printf("  Alert : %s\n\n",     avgAlert ? "TRUE" : "false");

    publishData();

    // Reset window
    sumTemp     = 0;
    sumHum      = 0;
    sumPress    = 0;
    sampleCount = 0;
    sosDetected = false;
    windowStart = millis();
  }

  // ── Buzzer ──
  if (avgAlert) handleBuzzer();
  else {
    digitalWrite(PIN_BUZZER, LOW);
    beepState    = false;
    lastBeepTime = 0;
  }

  delay(10);
}

// ═══════════════════════════════════════════════════════════
void publishData() {
  StaticJsonDocument<256> doc;
  doc["device_id"]   = DEVICE_ID;
  doc["temperature"] = round(avgTemp  * 100.0f) / 100.0f;
  doc["humidity"]    = round(avgHum   * 100.0f) / 100.0f;
  doc["pressure"]    = round(avgPress * 100.0f) / 100.0f;
  doc["alert"]       = avgAlert;
  doc["sos"]         = avgSos;
  doc["timestamp"]   = millis();

  char buf[256];
  serializeJson(doc, buf);

  if (mqttClient.publish(MQTT_TOPIC, buf))
    Serial.println("[MQTT] Published: " + String(buf));
  else
    Serial.printf("[MQTT] FAILED (state=%d)\n", mqttClient.state());
}

// ═══════════════════════════════════════════════════════════
void handleBuzzer() {
  unsigned long now = millis();
  if (!beepState) {
    if (now - lastBeepTime >= BEEP_OFF_MS) {
      digitalWrite(PIN_BUZZER, HIGH);
      beepState    = true;
      lastBeepTime = now;
    }
  } else {
    if (now - lastBeepTime >= BEEP_ON_MS) {
      digitalWrite(PIN_BUZZER, LOW);
      beepState    = false;
      lastBeepTime = now;
    }
  }
}

// ═══════════════════════════════════════════════════════════
void reconnectMQTT() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[WiFi] Reconnecting...");
    WiFi.disconnect();
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    int t = 0;
    while (WiFi.status() != WL_CONNECTED && t < 20) {
      delay(500); Serial.print("."); t++;
    }
    if (WiFi.status() == WL_CONNECTED)
      Serial.println("\n[WiFi] Reconnected: " + WiFi.localIP().toString());
    else { Serial.println("\n[WiFi] Failed."); return; }
  }
  int tries = 0;
  while (!mqttClient.connected() && tries < 5) {
    String cid = "ESP32_" + String(DEVICE_ID) + "_" + String(random(0xffff), HEX);
    Serial.print("[MQTT] Connecting...");
    if (mqttClient.connect(cid.c_str(), MQTT_USER, MQTT_PASS))
      Serial.println(" OK!");
    else {
      Serial.printf(" failed (state=%d), retry in 3s\n", mqttClient.state());
      delay(3000); tries++;
    }
  }
}
