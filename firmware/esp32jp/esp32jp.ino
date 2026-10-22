#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_ADS1X15.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
Adafruit_ADS1115 ads;

// ---------- Pins (match your breadboard wiring) ----------
const int VOLT1_PIN  = 35;
const int VOLT2_PIN  = 32;
const int VOLT3_PIN  = 33;
const int RELAY_PIN  = 25;
const int BUTTON_PIN = 26;
const int LED_RED    = 27;
const int LED_YELLOW = 14;
const int LED_GREEN  = 12;   // was 13 — that was why green never lit

// ---------- Channels ----------
// All three CTs installed
const bool CH_ENABLED[3] = { true, true, true };

// Voltage modules (ZMPT101B) on GPIO 35, 32, 33
const bool VOLT_ENABLED = true;

// ---------- Limits ----------
const float MAX_CURRENT = 30.0;
const unsigned long TRIP_DELAY_MS = 500;
const float MIN_VOLTAGE = 150.0;   // widen or narrow after calibration
const float MAX_VOLTAGE = 280.0;

// ---------- Calibration ----------
const float BURDEN        = 33.0;
const float CT_RATIO      = 2000.0;   // 100A / 0.05A
const int   CT_TURNS      = 1;
const float CURRENT_CAL   = 1.0;
const float NOISE_FLOOR_A = 0.15;
const float VOLTAGE_SCALE = 100.0;    // temporary until you calibrate

// ---------- Timing ----------
const unsigned long STARTUP_MS = 5000;

int nSamples = 60;

bool tripped     = false;
bool startupDone = false;
unsigned long startTime = 0;
unsigned long overSince = 0;

float hist[3][3] = {{0,0,0},{0,0,0},{0,0,0}};
int   histCount[3] = {0, 0, 0};

float median3(float a, float b, float c) {
  if (a > b) { float t = a; a = b; b = t; }
  if (b > c) { float t = b; b = c; c = t; }
  if (a > b) { float t = a; a = b; b = t; }
  return b;
}

void setup() {
  Serial.begin(115200);

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);

  // HIGH = allow contactor (COM + NO)
  digitalWrite(RELAY_PIN, HIGH);
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_GREEN, HIGH);

  Wire.begin(21, 22);
  Wire.setClock(400000);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED not found");
    while (1);
  }

  if (!ads.begin(0x48)) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("ADS1115 Error");
    display.display();
    while (1);
  }

  ads.setGain(GAIN_ONE);
  ads.setDataRate(RATE_ADS1115_860SPS);

  int enabled = 0;
  for (int ch = 0; ch < 3; ch++) if (CH_ENABLED[ch]) enabled++;
  if (enabled < 1) enabled = 1;
  nSamples = 180 / enabled;
  if (nSamples < 40) nSamples = 40;

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 18);
  display.println("Protection Ready");
  display.display();
  delay(800);

  startTime = millis();
}

void loop() {

  if (!startupDone && (millis() - startTime >= STARTUP_MS)) {
    startupDone = true;
  }

  // ----- Current RMS (bias removed) -----
  float sum[3]   = {0, 0, 0};
  float sumSq[3] = {0, 0, 0};

  for (int i = 0; i < nSamples; i++) {
    for (int ch = 0; ch < 3; ch++) {
      if (!CH_ENABLED[ch]) continue;
      float v = ads.computeVolts(ads.readADC_SingleEnded(ch));
      sum[ch]   += v;
      sumSq[ch] += v * v;
    }
  }

  float amps[3] = {0, 0, 0};
  float bias[3] = {0, 0, 0};

  for (int ch = 0; ch < 3; ch++) {
    if (!CH_ENABLED[ch]) continue;

    bias[ch] = sum[ch] / nSamples;
    float var = sumSq[ch] / nSamples - bias[ch] * bias[ch];
    if (var < 0) var = 0;
    float irms = sqrt(var) / BURDEN * CT_RATIO * CURRENT_CAL / CT_TURNS;

    if (histCount[ch] < 3) {
      for (int k = 0; k < 3; k++) hist[ch][k] = irms;
      histCount[ch] = 3;
    } else {
      hist[ch][0] = hist[ch][1];
      hist[ch][1] = hist[ch][2];
      hist[ch][2] = irms;
    }

    float med = median3(hist[ch][0], hist[ch][1], hist[ch][2]);
    amps[ch] = (med < NOISE_FLOOR_A) ? 0.0 : med;
  }

  // ----- Voltage (simple multi-sample) -----
  float volt1 = 0, volt2 = 0, volt3 = 0;
  if (VOLT_ENABLED) {
    float vs1 = 0, vs2 = 0, vs3 = 0;
    const int nv = 40;
    for (int i = 0; i < nv; i++) {
      float x1 = (analogRead(VOLT1_PIN) / 4095.0) * 3.3;
      float x2 = (analogRead(VOLT2_PIN) / 4095.0) * 3.3;
      float x3 = (analogRead(VOLT3_PIN) / 4095.0) * 3.3;
      vs1 += x1 * x1;
      vs2 += x2 * x2;
      vs3 += x3 * x3;
      delayMicroseconds(400);
    }
    // rough scale — calibrate later against a real meter
    volt1 = sqrt(vs1 / nv) * VOLTAGE_SCALE;
    volt2 = sqrt(vs2 / nv) * VOLTAGE_SCALE;
    volt3 = sqrt(vs3 / nv) * VOLTAGE_SCALE;
  }

  // ----- Protection -----
  bool fault = false;
  bool over  = false;

  if (startupDone) {
    for (int ch = 0; ch < 3; ch++) {
      if (CH_ENABLED[ch] && amps[ch] > MAX_CURRENT) over = true;
    }
  }

  if (over) {
    if (overSince == 0) overSince = millis();
    if (millis() - overSince >= TRIP_DELAY_MS) fault = true;
  } else {
    overSince = 0;
  }

  if (VOLT_ENABLED) {
    if (volt1 < MIN_VOLTAGE || volt1 > MAX_VOLTAGE ||
        volt2 < MIN_VOLTAGE || volt2 > MAX_VOLTAGE ||
        volt3 < MIN_VOLTAGE || volt3 > MAX_VOLTAGE) {
      fault = true;
    }
  }

  if (fault && !tripped) {
    tripped = true;
    digitalWrite(RELAY_PIN, LOW);
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_YELLOW, LOW);
  }

  // LEDs when not tripped
  if (!tripped) {
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_YELLOW, (over && startupDone) ? HIGH : LOW);
  }

  // Reset
  if (tripped && digitalRead(BUTTON_PIN) == LOW) {
    delay(40);
    if (digitalRead(BUTTON_PIN) == LOW) {
      tripped = false;
      overSince = 0;
      digitalWrite(RELAY_PIN, HIGH);
      digitalWrite(LED_RED, LOW);
      digitalWrite(LED_YELLOW, LOW);
      digitalWrite(LED_GREEN, HIGH);
      startTime = millis();
      startupDone = false;
    }
  }

  // ----- OLED (your layout) -----
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  if (tripped) {
    display.println("STATUS: TRIPPED");
  } else if (!startupDone) {
    display.println("STATUS: STARTING");
  } else {
    display.println("STATUS: OK");
  }
  display.println("---------------------");

  for (int ch = 0; ch < 3; ch++) {
    display.printf("L%d: ", ch + 1);
    if (CH_ENABLED[ch]) {
      display.printf("%.2f A\n", amps[ch]);
    } else {
      display.println("--");
    }
  }

  if (VOLT_ENABLED) {
    display.printf("V: %.0f %.0f %.0f\n", volt1, volt2, volt3);
  } else {
    display.println("V: --");
  }

  if (tripped) {
    display.printf("Limit %.0fA  Press Reset\n", MAX_CURRENT);
  } else {
    display.printf("Limit %.0f A\n", MAX_CURRENT);
  }

  display.display();
}