#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

// --- Pin configuration ---
#define DHTPIN 14        // DHT22 data pin
#define DHTTYPE DHT11   // Change to DHT22 if needed

#define SDA_PIN 21       // I2C SDA
#define SCL_PIN 22       // I2C SCL

// --- OLED setup ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// --- DHT sensor setup ---
DHT dht(DHTPIN, DHTTYPE);

// --- Setup function ---
void setup() {
  Serial.begin(115200);
  Serial.println("Hello, ESP32!");

  // Initialize I2C on custom pins
  Wire.begin(SDA_PIN, SCL_PIN);

  // Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    for (;;);
  }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Initializing...");
  display.display();

  // Initialize DHT sensor
  dht.begin();
  delay(1000);
}

// --- Main loop ---
void loop() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Check if read failed
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Error reading DHT22 sensor!");
    return;
  }

  // Print values on Serial Monitor
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C  |  Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  // Display on OLED
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Hello IoT");
  display.setCursor(0, 16);
  display.print("Temp: ");
  display.print(temperature);
  display.println(" C");
  display.setCursor(0, 32);
  display.print("Humidity: ");
  display.print(humidity);
  display.println(" %");
  display.display();

  delay(2000); // update every 2 seconds
}