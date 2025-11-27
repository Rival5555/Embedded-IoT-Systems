/*
 * Complete ESP32 Webserver with Static IP
 * Controls Built-in LED (GPIO 2)
 */

#include <WiFi.h>

const char* ssid = "1255";
const char* password = "12345678";

// ----- Static IP Configuration Settings -----
// Choose an IP outside your router's normal DHCP range to avoid conflicts
IPAddress staticIP(192, 168, 122, 151); 
IPAddress gateway(192, 168, 122, 1);    // Your router's IP address
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);     // Google DNS, optional
// ------------------------------------------

WiFiServer server(80);
const int LED_PIN = 2;     // Built-in LED (GPIO 2)

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);   // LED off at start

  // Connect WiFi
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // 1. Configure the Static IP Address
  WiFi.config(staticIP, gateway, subnet, primaryDNS);
  
  // 2. Start connecting to WiFi
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");

  Serial.print("ESP32 Static IP Address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (!client) return;  // No client, exit

  Serial.println("New Client connected");
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);

  // ----- LED CONTROL -----
  if (request.indexOf("/LED=ON") != -1) {
    digitalWrite(LED_PIN, HIGH);
  }
  if (request.indexOf("/LED=OFF") != -1) {
    digitalWrite(LED_PIN, LOW);
  }

  // ----- RESPONSE PAGE -----
  String htmlPage =
    "<!DOCTYPE html><html>"
    "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"></head>"
    "<body><h1>ESP32 LED Control</h1>"
    "<p>Status: " + String(digitalRead(LED_PIN) == HIGH ? "ON" : "OFF") + "</p>"
    "<p><a href=\"/LED=ON\"><button style=\"background-color:#4CAF50; color: white; padding: 15px 32px; text-align: center; text-decoration: none; display: inline-block; font-size: 16px;\">LED ON</button></a></p>"
    "<p><a href=\"/LED=OFF\"><button style=\"background-color:#f44336; color: white; padding: 15px 32px; text-align: center; text-decoration: none; display: inline-block; font-size: 16px;\">LED OFF</button></a></p>"
    "</body></html>";

  // HTTP Headers
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close");
  client.println();
  client.println(htmlPage);

  delay(1);
  client.stop();
  Serial.println("Client disconnected");
}