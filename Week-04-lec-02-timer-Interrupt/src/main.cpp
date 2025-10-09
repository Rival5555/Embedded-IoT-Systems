#include <Arduino.h>

#define LED_PIN 2            // GPIO4 for LED
hw_timer_t *My_timer = nullptr;

// ---- Timer ISR ----
void IRAM_ATTR onTimer() {
  digitalWrite(LED_PIN, !digitalRead(LED_PIN));  // toggle LED
}

// ---- Setup ----
void setup() {
  pinMode(LED_PIN, OUTPUT);

  // timerBegin(timer number 0-3, prescaler, countUp)
  // 80 MHz / 80 = 1 MHz → tick = 1 µs
  My_timer = timerBegin(0, 80, true);

  // attach ISR to timer, edge-triggered
  timerAttachInterrupt(My_timer, &onTimer, true);

  // trigger every 1 000 000 µs = 1 s
  timerAlarmWrite(My_timer, 1000000, true);

  // enable alarm
  timerAlarmEnable(My_timer);
}

void loop() {
  // main loop free for other code
}