#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- Network Settings ---
const char* ssid = "Wokwi-GUEST";
const char* password = "";
const char* mqtt_server = "192.168.55.1"; // Your Laptop IP

// --- Pin Definitions (Based on your diagram.json) ---
#define DHTPIN 23        // DHT Data pin connected to GPIO 23
#define DHTTYPE DHT22
#define BUTTON_PIN 5     // Button connected to GPIO 5

// --- OLED Display Settings ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// --- Objects ---
WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  
  // 1. Initialize Sensors & Pins
  dht.begin();
  pinMode(BUTTON_PIN, INPUT_PULLUP); // Using Internal Pullup since button goes to GND

  // 2. Initialize OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Initializing...");
  display.display();

  // 3. Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("WiFi Connected");

  // 4. Connect to MQTT
  client.setServer(mqtt_server, 1883);
}

void loop() {
  // --- Reconnect if lost ---
  if (!client.connected()) {
    while (!client.connected()) {
      Serial.println("Connecting to MQTT...");
      if (client.connect("ESP32Client")) {
        Serial.println("Connected to Broker");
      } else {
        delay(5000);
      }
    }
  }
  client.loop();

  // --- Read Sensors ---
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  int btnState = digitalRead(BUTTON_PIN); // LOW = Pressed, HIGH = Released

  // Check valid reading
  if (isnan(temp) || isnan(hum)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // --- Display on Serial Monitor ---
  Serial.print("Temp: "); Serial.print(temp); Serial.print("C  ");
  Serial.print("Hum: "); Serial.print(hum); Serial.print("%  ");
  Serial.print("Btn: "); Serial.println(btnState == LOW ? "PRESSED" : "OPEN");

  // --- Display on OLED ---
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.println("IoT Dashboard");
  display.drawLine(0, 10, 128, 10, WHITE); // Draw line
  
  display.setCursor(0, 20);
  display.print("Temp: "); display.print(temp); display.println(" C");
  
  display.setCursor(0, 35);
  display.print("Hum:  "); display.print(hum); display.println(" %");

  if(btnState == LOW) {
    display.setCursor(0, 50);
    display.println("STATUS: ALERT!");
    client.publish("home/lab1/alert", "Button Pressed!");
  }

  display.display();

  // --- Send to MQTT ---
  char tempString[8];
  dtostrf(temp, 1, 2, tempString);
  client.publish("home/lab1/temp", tempString);
  char humString[8];
  dtostrf(hum, 1, 2, humString);
  client.publish("home/lab1/humidity", humString);

  delay(2000); // Wait 2s
}