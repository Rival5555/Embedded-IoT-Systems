#include <WiFi.h>
#include <WebServer.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DHT.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Pins
#define DHTPIN 23
#define DHTTYPE DHT11
#define BUTTON_PIN 5   // Button to GND, use INPUT_PULLUP

DHT dht(DHTPIN, DHTTYPE);

// WiFi credentials
const char* ssid     = "NTU FSD";
const char* password = "";

WebServer server(80);

// Last measured values
float lastTemp = NAN;
float lastHum  = NAN;

bool lastButtonState = HIGH;

// --- Helper: read DHT and update globals ---
void readDHTValues() {
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // Celsius

  if (!isnan(h) && !isnan(t)) {
    lastHum  = h;
    lastTemp = t;
    Serial.print("Temp: ");
    Serial.print(t);
    Serial.print(" *C, Humidity: ");
    Serial.print(h);
    Serial.println(" %");
  } else {
    Serial.println("Failed to read from DHT!");
  }
}

// --- Helper: show values on OLED ---
void showOnOLED() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  if (isnan(lastTemp) || isnan(lastHum)) {
    display.setCursor(0, 0);
    display.println("DHT Error!");
  } else {
    display.setCursor(0, 0);
    display.println("DHT22 Readings");

    display.setCursor(0, 20);
    display.print("Temp: ");
    display.print(lastTemp, 1);
    display.println(" C");

    display.setCursor(0, 40);
    display.print("Hum:  ");
    display.print(lastHum, 1);
    display.println(" %");
  }

  display.display();
}

// --- Web handler ---
void handleRoot() {
  // Option A: use last measured values
  // Option B: take fresh reading here, uncomment if you want:
  // readDHTValues();

  String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<meta http-equiv='refresh' content='5'>";
  html += "<title>ESP32 DHT Monitor</title></head><body>";
  html += "<h2>ESP32 DHT22 Readings</h2>";

  if (isnan(lastTemp) || isnan(lastHum)) {
    html += "<p><b>No valid data yet.</b><br>Press the button to take a reading.</p>";
  } else {
    html += "<p><b>Temperature:</b> ";
    html += String(lastTemp, 1);
    html += " &deg;C</p>";

    html += "<p><b>Humidity:</b> ";
    html += String(lastHum, 1);
    html += " %</p>";
  }

  html += "<hr><p>Press the physical button to update readings on OLED and here.</p>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // OLED init
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED not found!");
    while (1);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Booting...");
  display.display();

  dht.begin();

  // WiFi connect
  WiFi.begin(ssid, password);
  display.setCursor(0, 16);
  display.println("WiFi Connecting...");
  display.display();

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("WiFi Connected");
  display.setCursor(0, 16);
  display.print("IP: ");
  display.println(WiFi.localIP());
  display.setCursor(0, 32);
  display.println("Press button");
  display.setCursor(0, 44);
  display.println("to read DHT");
  display.display();

  // Web server
  server.on("/", handleRoot);
  server.begin();
}

void loop() {
  server.handleClient();

  bool currentButtonState = digitalRead(BUTTON_PIN);

  // Detect falling edge (HIGH -> LOW)
  if (lastButtonState == HIGH && currentButtonState == LOW) {
    // Small debounce delay
    delay(50);
    if (digitalRead(BUTTON_PIN) == LOW) {
      Serial.println("Button pressed: reading DHT + updating OLED");
      readDHTValues();
      showOnOLED();
    }
  }

  lastButtonState = currentButtonState;
}
