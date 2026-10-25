#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_ADS1X15.h>
#include "esp_task_wdt.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
Adafruit_ADS1115 ads;

// ---------- Pins ----------
const int VOLT1_PIN  = 35;
const int VOLT2_PIN  = 32;
const int VOLT3_PIN  = 33;
const int RELAY_PIN  = 25;
const int BUTTON_PIN = 26;
const int LED_RED    = 27;
const int LED_YELLOW = 14;
const int LED_GREEN  = 12;

// ---------- Channels ----------
const bool CH_ENABLED[3] = { true, true, true };
const bool VOLT_ENABLED  = true;

// ---------- Per-pump setting (change this for each pump) ----------
const float PUMP_FLA      = 10.0;   // full-load amps of this pump
const float MAX_CURRENT   = PUMP_FLA * 1.5;  // trip at 150 % FLA
const float IMBALANCE_PCT = 25.0;   // phase current imbalance trip
const float PHASE_LOSS_V  = 80.0;   // below this counts as phase loss

const unsigned long TRIP_DELAY_MS = 500;
const float MIN_VOLTAGE = 150.0;
const float MAX_VOLTAGE = 280.0;

// ---------- Calibration ----------
const float BURDEN        = 33.0;
const float CT_RATIO      = 2000.0;
const int   CT_TURNS      = 1;
const float CURRENT_CAL   = 1.0;
const float NOISE_FLOOR_A = 0.15;
const float VOLTAGE_SCALE = 100.0;  // calibrate later
const float START_THRESH  = 0.5;

const unsigned long STARTUP_MS = 5000;
const unsigned long LOG_INTERVAL_MS = 2000;

int nSamples = 60;

bool tripped     = false;
bool startupDone = true;
unsigned long startTime = 0;
unsigned long overSince = 0;
unsigned long lastLog   = 0;
bool motorWasOff = true;

enum TripCause {
  NONE = 0,
  OVERCURRENT,
  UNDERVOLT,
  OVERVOLT,
  PHASE_LOSS,
  IMBALANCE
};
TripCause tripReason = NONE;

float histI[3][3] = {{0}};
int   histICnt[3] = {0};
float histV[3][3] = {{0}};
int   histVCnt[3] = {0};

float median3(float a, float b, float c) {
  if (a > b) { float t = a; a = b; b = t; }
  if (b > c) { float t = b; b = c; c = t; }
  if (a > b) { float t = a; a = b; b = t; }
  return b;
}

void logEvent(const char* msg) {
  Serial.printf("[%lu] %s\n", millis(), msg);
}

void setup() {
  Serial.begin(115200);
  delay(100);
  logEvent("Boot");

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);

  // fail-safe: keep contactor open until init succeeds
  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_GREEN, LOW);

  Wire.begin(21, 22);
  Wire.setClock(400000);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED not found");
    while (1) { delay(1000); }
  }

  if (!ads.begin(0x48)) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("ADS1115 Error");
    display.display();
    while (1) { delay(1000); }
  }

  ads.setGain(GAIN_ONE);
  ads.setDataRate(RATE_ADS1115_860SPS);

  int enabled = 0;
  for (int ch = 0; ch < 3; ch++) if (CH_ENABLED[ch]) enabled++;
  if (enabled < 1) enabled = 1;
  nSamples = 180 / enabled;
  if (nSamples < 40) nSamples = 40;

  // init ok → enable protection and arm watchdog
  esp_task_wdt_init(8, true);          // 8 s timeout, panic reboot
  esp_task_wdt_add(NULL);

  digitalWrite(RELAY_PIN, HIGH);
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, HIGH);

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 18);
  display.println("Protection Ready");
  display.display();
  delay(600);

  logEvent("Init OK, protection armed");
}

void loop() {
  esp_task_wdt_reset();

  // ----- Current RMS -----
  float sum[3] = {0}, sumSq[3] = {0};

  for (int i = 0; i < nSamples; i++) {
    for (int ch = 0; ch < 3; ch++) {
      if (!CH_ENABLED[ch]) continue;
      float v = ads.computeVolts(ads.readADC_SingleEnded(ch));
      sum[ch] += v;
      sumSq[ch] += v * v;
    }
  }

  float amps[3] = {0};

  for (int ch = 0; ch < 3; ch++) {
    if (!CH_ENABLED[ch]) continue;

    float bias = sum[ch] / nSamples;
    float var = sumSq[ch] / nSamples - bias * bias;
    if (var < 0) var = 0;
    float irms = sqrt(var) / BURDEN * CT_RATIO * CURRENT_CAL / CT_TURNS;

    if (histICnt[ch] < 3) {
      for (int k = 0; k < 3; k++) histI[ch][k] = irms;
      histICnt[ch] = 3;
    } else {
      histI[ch][0] = histI[ch][1];
      histI[ch][1] = histI[ch][2];
      histI[ch][2] = irms;
    }
    float med = median3(histI[ch][0], histI[ch][1], histI[ch][2]);
    amps[ch] = (med < NOISE_FLOOR_A) ? 0.0 : med;
  }

  float maxA = 0, sumA = 0;
  int liveCh = 0;
  for (int ch = 0; ch < 3; ch++) {
    if (!CH_ENABLED[ch]) continue;
    if (amps[ch] > maxA) maxA = amps[ch];
    sumA += amps[ch];
    liveCh++;
  }
  float avgA = (liveCh > 0) ? sumA / liveCh : 0;

  // motor start detection
  if (motorWasOff && maxA > START_THRESH) {
    startTime = millis();
    startupDone = false;
    motorWasOff = false;
    logEvent("Motor start detected, startup window open");
  }
  if (maxA < NOISE_FLOOR_A) motorWasOff = true;

  if (!startupDone && (millis() - startTime >= STARTUP_MS)) {
    startupDone = true;
    logEvent("Startup window closed");
  }

  // ----- Voltage (bias removed + median filter) -----
  float rawV[3] = {0};
  if (VOLT_ENABLED) {
    float s[3] = {0}, q[3] = {0};
    const int nv = 200;
    for (int i = 0; i < nv; i++) {
      float x1 = (analogRead(VOLT1_PIN) / 4095.0f) * 3.3f;
      float x2 = (analogRead(VOLT2_PIN) / 4095.0f) * 3.3f;
      float x3 = (analogRead(VOLT3_PIN) / 4095.0f) * 3.3f;
      s[0] += x1; s[1] += x2; s[2] += x3;
      q[0] += x1 * x1; q[1] += x2 * x2; q[2] += x3 * x3;
      delayMicroseconds(400);
    }
    for (int p = 0; p < 3; p++) {
      float m = s[p] / nv;
      float v = q[p] / nv - m * m;
      if (v < 0) v = 0;
      rawV[p] = sqrt(v) * VOLTAGE_SCALE;
    }
  }

  float volt[3] = {0};
  for (int p = 0; p < 3; p++) {
    if (histVCnt[p] < 3) {
      for (int k = 0; k < 3; k++) histV[p][k] = rawV[p];
      histVCnt[p] = 3;
    } else {
      histV[p][0] = histV[p][1];
      histV[p][1] = histV[p][2];
      histV[p][2] = rawV[p];
    }
    volt[p] = median3(histV[p][0], histV[p][1], histV[p][2]);
  }

  // ----- Protection logic -----
  bool fault = false;
  bool over  = false;
  TripCause reason = NONE;

  if (startupDone) {
    for (int ch = 0; ch < 3; ch++) {
      if (CH_ENABLED[ch] && amps[ch] > MAX_CURRENT) {
        over = true;
        reason = OVERCURRENT;
      }
    }
  }

  if (over) {
    if (overSince == 0) overSince = millis();
    if (millis() - overSince >= TRIP_DELAY_MS) fault = true;
  } else {
    overSince = 0;
  }

  if (VOLT_ENABLED && !fault) {
    // under / over voltage
    for (int p = 0; p < 3; p++) {
      if (volt[p] < MIN_VOLTAGE) { fault = true; reason = UNDERVOLT; }
      if (volt[p] > MAX_VOLTAGE) { fault = true; reason = OVERVOLT; }
    }

    // phase loss (one phase collapsed while others present)
    int good = 0;
    for (int p = 0; p < 3; p++) if (volt[p] > PHASE_LOSS_V) good++;
    if (good >= 1 && good < 3) {
      fault = true;
      reason = PHASE_LOSS;
    }

    // current imbalance (only when motor is running)
    if (startupDone && avgA > 1.0) {
      float maxDev = 0;
      for (int ch = 0; ch < 3; ch++) {
        if (!CH_ENABLED[ch]) continue;
        float dev = fabs(amps[ch] - avgA) / avgA * 100.0f;
        if (dev > maxDev) maxDev = dev;
      }
      if (maxDev > IMBALANCE_PCT) {
        fault = true;
        reason = IMBALANCE;
      }
    }
  }

  if (fault && !tripped) {
    tripped = true;
    tripReason = reason;
    digitalWrite(RELAY_PIN, LOW);
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_YELLOW, LOW);

    const char* r = "UNKNOWN";
    if (reason == OVERCURRENT) r = "OVERCURRENT";
    else if (reason == UNDERVOLT) r = "UNDERVOLT";
    else if (reason == OVERVOLT) r = "OVERVOLT";
    else if (reason == PHASE_LOSS) r = "PHASE_LOSS";
    else if (reason == IMBALANCE) r = "IMBALANCE";
    char buf[64];
    snprintf(buf, sizeof(buf), "TRIP: %s  I=%.1f/%.1f/%.1f  V=%.0f/%.0f/%.0f",
             r, amps[0], amps[1], amps[2], volt[0], volt[1], volt[2]);
    logEvent(buf);
  }

  if (!tripped) {
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_YELLOW, (over && startupDone) ? HIGH : LOW);
  }

  // Reset — clear trip only
  if (tripped && digitalRead(BUTTON_PIN) == LOW) {
    delay(40);
    if (digitalRead(BUTTON_PIN) == LOW) {
      tripped = false;
      tripReason = NONE;
      overSince = 0;
      digitalWrite(RELAY_PIN, HIGH);
      digitalWrite(LED_RED, LOW);
      digitalWrite(LED_YELLOW, LOW);
      digitalWrite(LED_GREEN, HIGH);
      logEvent("Manual reset");
    }
  }

  // periodic status log
  if (millis() - lastLog >= LOG_INTERVAL_MS) {
    lastLog = millis();
    Serial.printf("[%lu] I: %.2f %.2f %.2f A | V: %.0f %.0f %.0f | %s\n",
                  millis(),
                  amps[0], amps[1], amps[2],
                  volt[0], volt[1], volt[2],
                  tripped ? "TRIPPED" : (startupDone ? "OK" : "STARTING"));
  }

  // ----- OLED -----
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  if (tripped) {
    display.print("TRIP: ");
    if (tripReason == OVERCURRENT) display.println("OVERCURRENT");
    else if (tripReason == UNDERVOLT) display.println("UNDERVOLT");
    else if (tripReason == OVERVOLT) display.println("OVERVOLT");
    else if (tripReason == PHASE_LOSS) display.println("PHASE LOSS");
    else if (tripReason == IMBALANCE) display.println("IMBALANCE");
    else display.println("FAULT");
  } else if (!startupDone) {
    display.println("STATUS: STARTING");
  } else {
    display.println("STATUS: OK");
  }
  display.println("---------------------");

  for (int ch = 0; ch < 3; ch++) {
    display.printf("L%d: ", ch + 1);
    if (CH_ENABLED[ch]) display.printf("%.2f A\n", amps[ch]);
    else display.println("--");
  }

  if (VOLT_ENABLED) {
    display.printf("V: %.0f %.0f %.0f\n", volt[0], volt[1], volt[2]);
  } else {
    display.println("V: --");
  }

  if (tripped) {
    display.printf("Limit %.1fA  Reset\n", MAX_CURRENT);
  } else {
    display.printf("Limit %.1f A\n", MAX_CURRENT);
  }

  display.display();
}