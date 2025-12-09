#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

// --- Configuration ---
#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 64    // OLED display height, in pixels
#define OLED_ADDR 0x3C      // I2C address for 128x64 display (0x3C or 0x3D)

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The '-1' means we are not using a reset pin, which is typical for I2C.
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Pins and Sensor Type
#define DHTPIN 23           // GPIO pin the DHT data line is connected to
#define DHTTYPE DHT22       // Specify the sensor model
#define BUTTON_PIN 5        // Button connected between this pin and GND

// Initialize DHT sensor object
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);

  // Set the button pin as an input with internal pull-up resistor enabled
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // --- OLED Init ---
  // You must check your specific OLED's I2C address (usually 0x3C or 0x3D)
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("OLED not found or wrong address!");
    while (1); // Stop execution if OLED is not initialized
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("System Ready...");
  display.println("Press button to read...");
  display.display();

  dht.begin();
}

void loop() {
  // Check if the button is pressed (LOW because of INPUT_PULLUP)
  if (digitalRead(BUTTON_PIN) == LOW) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Reading DHT...");
    display.display();

    delay(50); // Small debounce delay

    // Reading temperature or humidity takes about 250 milliseconds!
    float h = dht.readHumidity();
    float t = dht.readTemperature(); // Celsius

    display.clearDisplay();

    if (isnan(t) || isnan(h)) {
      Serial.println("Failed to read from DHT sensor!");
      display.setCursor(0, 0);
      display.println("DHT Error!");
    } else {
      Serial.print("Temperature: ");
      Serial.print(t);
      Serial.print(" C, Humidity: ");
      Serial.print(h);
      Serial.println(" %");

      display.setCursor(0, 0);
      display.println("DHT22 Readings");
      display.setCursor(0, 20);
      display.print("Temp: ");
      display.print(t);
      display.println(" C");
      display.setCursor(0, 40);
      display.print("Hum:  ");
      display.print(h);
      display.println(" %");
    }

    display.display();

    // Wait for the button to be released before proceeding
    while (digitalRead(BUTTON_PIN) == LOW) {
      delay(10);
    }
    delay(500); // Wait a short period after release before allowing a new read
  }
  
  // This delay prevents the loop from consuming too much power by constantly checking the button
  delay(10);
}