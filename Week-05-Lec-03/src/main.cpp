// PWM + Buzzer + Two LEDs (concurrent)
// LED pins: 18 and 19 (fade together)
// Buzzer pin: 27 (tones)
// ESP32: uses separate LEDC channels and a FreeRTOS task for LED fading

#include <Arduino.h>

// Pins
#define LED_PIN1      18
#define LED_PIN2      19
#define BUZZER_PIN    27

// LEDC channels
#define BUZZER_CH     0   // channel for buzzer (ledcWriteTone)
#define LED1_CH       1   // channel for LED on pin 18
#define LED2_CH       2   // channel for LED on pin 19

// Buzzer settings
#define BUZZER_RES    10      // 10-bit for buzzer (0..1023)
#define BUZZER_FREQ   2000    // base freq (ledcWriteTone will change it)

// LED settings
#define LED_FREQ      1000    // 1kHz PWM for LEDs
#define LED_RES       8       // 8-bit resolution (0..255)

// Task handle
TaskHandle_t ledTaskHandle = NULL;

void ledFadeTask(void *param) {
  // simple up/down fade for both LEDs in sync
  const int stepDelay = 10; // ms per brightness step
  int d = 0;
  int dir = 1; // 1 -> up, -1 -> down

  for (;;) {
    // write brightness to both LED channels
    ledcWrite(LED1_CH, d);
    ledcWrite(LED2_CH, d);

    // step
    d += dir;
    if (d >= 255) { d = 255; dir = -1; }
    if (d <= 0)   { d = 0;   dir = 1; }

    vTaskDelay(stepDelay / portTICK_PERIOD_MS);
  }
}

void setup() {
  // Serial for debug (optional)
  Serial.begin(115200);

  // Setup LED channels (LEDs)
  ledcSetup(LED1_CH, LED_FREQ, LED_RES);
  ledcAttachPin(LED_PIN1, LED1_CH);

  ledcSetup(LED2_CH, LED_FREQ, LED_RES);
  ledcAttachPin(LED_PIN2, LED2_CH);

  // Setup buzzer channel
  ledcSetup(BUZZER_CH, BUZZER_FREQ, BUZZER_RES);
  ledcAttachPin(BUZZER_PIN, BUZZER_CH);

  // Start LED fade task (runs concurrently)
  xTaskCreate(
    ledFadeTask,        // function
    "LED Fade Task",    // name
    2048,               // stack size
    NULL,               // param
    1,                  // priority
    &ledTaskHandle
  );

  // --- Initial buzzer demo (runs while LEDs fade) ---

  // 1) Simple beep pattern
  for (int i = 0; i < 3; i++) {
    ledcWriteTone(BUZZER_CH, 2000 + i * 400); // change tone
    delay(150);
    ledcWrite(BUZZER_CH, 0);                  // stop tone
    delay(150);
  }

  // 2) Frequency sweep (400Hz → 3kHz)
  for (int f = 400; f <= 3000; f += 100) {
    ledcWriteTone(BUZZER_CH, f);
    delay(20);
  }
  ledcWrite(BUZZER_CH, 0);
  delay(500);

  // 3) Short melody
  int melody[] = {262, 294, 330, 349, 392, 440, 494, 523};
  for (int i = 0; i < 8; i++) {
    ledcWriteTone(BUZZER_CH, melody[i]);
    delay(250);
  }
  ledcWrite(BUZZER_CH, 0); // stop buzzer
}

void loop() {
  // Repeat the buzzer sequence while the LED fade task runs
  // 1) Simple beep pattern
  for (int i = 0; i < 3; i++) {
    ledcWriteTone(BUZZER_CH, 2000 + i * 400);
    delay(150);
    ledcWrite(BUZZER_CH, 0);
    delay(150);
  }

  // 2) Frequency sweep (400Hz → 3kHz)
  for (int f = 400; f <= 3000; f += 100) {
    ledcWriteTone(BUZZER_CH, f);
    delay(20);
  }
  ledcWrite(BUZZER_CH, 0);
  delay(500);

  // 3) Short melody
  int melody[] = {262, 294, 330, 349, 392, 440, 494, 523};
  for (int i = 0; i < 8; i++) {
    ledcWriteTone(BUZZER_CH, melody[i]);
    delay(250);
  }
  ledcWrite(BUZZER_CH, 0); // stop buzzer

  delay(500); // small pause before repeating
}