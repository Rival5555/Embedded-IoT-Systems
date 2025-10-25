// ============================================================================
// Project Title : ESP32 Smart LED Controller with OLED Display & Buzzer
// Name          : HASSAN AL
// RegNo  #      : 23-NTU-CS-1255
//
// Description:
// This firmware controls 3 LEDs and a buzzer using an ESP32. 
// It demonstrates multiple lighting patterns, button-based interactions,
// and OLED feedback for visual monitoring.
//
// ▶ Mode Button → Switches between LED effects:
//     1. OFF → 2. ALTERNATE → 3. ALL ON → 4. SMOOTH PWM FADE
// ▶ Action Button → Short press toggles all LEDs; long press activates buzzer
// ▶ Boot Button  → Resets everything back to OFF state
//
// Components Used:
//   - ESP32 NodeMCU (DevKit v1)
//   - 3 LEDs (Red, Green, Yellow)
//   - 2 Push Buttons (Mode, Action)
//   - 1 Piezo Buzzer
//   - 0.96" I2C OLED Display (SSD1306)
// ============================================================================

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <math.h>

// ----------------------- Pin Mapping -----------------------
#define BUZZER_PIN           27
#define MODE_BUTTON_PIN      13
#define ACTION_BUTTON_PIN    12
#define BOOT_BUTTON_PIN      0    // Built-in boot/reset button

#define LED_RED_PIN          19
#define LED_GREEN_PIN        18
#define LED_YELLOW_PIN       5

// ----------------------- PWM Configuration -----------------
#define PWM_RESOLUTION_BITS  8
#define LED_PWM_FREQUENCY    5000
#define BUZZER_PWM_CHANNEL   0
#define LED_RED_CHANNEL      1
#define LED_GREEN_CHANNEL    2
#define LED_YELLOW_CHANNEL   3

// ----------------------- OLED Setup ------------------------
#define OLED_SCREEN_WIDTH    128
#define OLED_SCREEN_HEIGHT   64
#define OLED_I2C_ADDRESS     0x3C
Adafruit_SSD1306 oledDisplay(OLED_SCREEN_WIDTH, OLED_SCREEN_HEIGHT, &Wire, -1);

// ----------------------- Mode Management -------------------
enum LightingMode {
  MODE_OFF = 0,
  MODE_ALTERNATE,
  MODE_ALL_ON,
  MODE_FADE,
  TOTAL_MODES
};

LightingMode activeLightingMode = MODE_OFF;

// ----------------------- Timing Variables ------------------
unsigned long ledBlinkTimer = 0;
const unsigned long LED_BLINK_INTERVAL = 400;
bool isBlinkOn = false;

unsigned long ledFadeTimer = 0;
unsigned long buttonPressStartTime = 0;
bool isButtonPressed = false;
bool isLongPressHandled = false;

unsigned long lastModeButtonPressTime = 0;
unsigned long lastBootButtonPressTime = 0;
const unsigned long BUTTON_DEBOUNCE_DELAY = 50;

// Manual LED toggle state (used after short press)
bool isManualControlEnabled = false;
bool manualLEDState = false;

// ============================================================================
// OLED Utility — Print text on 2 lines (header + main info)
// ============================================================================
void displayOLEDMessage(const char* headerText, const char* mainMessage) {
  oledDisplay.clearDisplay();
  oledDisplay.setTextSize(1);
  oledDisplay.setTextColor(SSD1306_WHITE);
  oledDisplay.setCursor(0, 0);
  oledDisplay.println(headerText);

  oledDisplay.setTextSize(2);
  oledDisplay.setCursor(0, 20);
  oledDisplay.println(mainMessage);
  oledDisplay.display();
}

// ============================================================================
// Change LED Mode — Called when MODE button is pressed
// ============================================================================
void updateLightingMode(LightingMode newMode) {
  activeLightingMode = newMode;
  isManualControlEnabled = false;  // disable manual LED control if new mode selected

  switch (newMode) {
    case MODE_OFF:
      displayOLEDMessage("Mode:", "All OFF");
      ledcWrite(LED_RED_CHANNEL, 0);
      ledcWrite(LED_GREEN_CHANNEL, 0);
      ledcWrite(LED_YELLOW_CHANNEL, 0);
      break;

    case MODE_ALTERNATE:
      displayOLEDMessage("Mode:", "Alternate");
      ledBlinkTimer = millis();
      isBlinkOn = false;
      break;

    case MODE_ALL_ON:
      displayOLEDMessage("Mode:", "All ON");
      ledcWrite(LED_RED_CHANNEL, 255);
      ledcWrite(LED_GREEN_CHANNEL, 255);
      ledcWrite(LED_YELLOW_CHANNEL, 255);
      break;

    case MODE_FADE:
      displayOLEDMessage("Mode:", "PWM Fade");
      ledFadeTimer = millis();
      break;
  }
}

// ============================================================================
// Buzzer Utility — Simple tone function
// ============================================================================
void playBuzzerTone(unsigned int toneFrequency, unsigned long toneDuration) {
  ledcWriteTone(BUZZER_PWM_CHANNEL, toneFrequency);
  delay(toneDuration);
  ledcWriteTone(BUZZER_PWM_CHANNEL, 0);
}

// ============================================================================
// SETUP — Initialization Code
// ============================================================================
void setup() {
  Serial.begin(115200);

  // Configure input pins with internal pull-ups
  pinMode(MODE_BUTTON_PIN, INPUT_PULLUP);
  pinMode(ACTION_BUTTON_PIN, INPUT_PULLUP);
  pinMode(BOOT_BUTTON_PIN, INPUT_PULLUP);

  // Configure LED and buzzer pins for PWM output
  ledcSetup(BUZZER_PWM_CHANNEL, 2000, PWM_RESOLUTION_BITS);
  ledcAttachPin(BUZZER_PIN, BUZZER_PWM_CHANNEL);

  ledcSetup(LED_RED_CHANNEL, LED_PWM_FREQUENCY, PWM_RESOLUTION_BITS);
  ledcAttachPin(LED_RED_PIN, LED_RED_CHANNEL);

  ledcSetup(LED_GREEN_CHANNEL, LED_PWM_FREQUENCY, PWM_RESOLUTION_BITS);
  ledcAttachPin(LED_GREEN_PIN, LED_GREEN_CHANNEL);

  ledcSetup(LED_YELLOW_CHANNEL, LED_PWM_FREQUENCY, PWM_RESOLUTION_BITS);
  ledcAttachPin(LED_YELLOW_PIN, LED_YELLOW_CHANNEL);

  // Initialize OLED Display
  Wire.begin(21, 22);
  if (!oledDisplay.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDRESS)) {
    while (true) delay(100); // stop execution if OLED not detected
  }

  displayOLEDMessage("System:", "Ready");
  updateLightingMode(MODE_OFF);
}

// ============================================================================
// LOOP — Core Logic
// ============================================================================
void loop() {
  unsigned long currentMillis = millis();

  // -------------------- MODE BUTTON --------------------
  if (digitalRead(MODE_BUTTON_PIN) == LOW) {
    if (currentMillis - lastModeButtonPressTime > BUTTON_DEBOUNCE_DELAY) {
      activeLightingMode = (LightingMode)((activeLightingMode + 1) % TOTAL_MODES);
      updateLightingMode(activeLightingMode);
      while (digitalRead(MODE_BUTTON_PIN) == LOW) delay(10);
      lastModeButtonPressTime = millis();
    }
  }

  // -------------------- BOOT BUTTON --------------------
  if (digitalRead(BOOT_BUTTON_PIN) == LOW) {
    if (currentMillis - lastBootButtonPressTime > BUTTON_DEBOUNCE_DELAY) {
      updateLightingMode(MODE_OFF);
      displayOLEDMessage("System:", "Reset (BOOT)");
      while (digitalRead(BOOT_BUTTON_PIN) == LOW) delay(10);
      lastBootButtonPressTime = millis();
    }
  }

  // -------------------- ACTION BUTTON --------------------
  int actionButtonState = digitalRead(ACTION_BUTTON_PIN);

  // Detect initial press
  if (actionButtonState == LOW && !isButtonPressed) {
    isButtonPressed = true;
    buttonPressStartTime = currentMillis;
    isLongPressHandled = false;
  }

  // Handle long press (buzzer)
  if (actionButtonState == LOW && isButtonPressed && !isLongPressHandled) {
    if (currentMillis - buttonPressStartTime >= 1500) { // 1.5 sec = long press
      displayOLEDMessage("Action:", "Long Press");
      playBuzzerTone(2500, 300);
      isLongPressHandled = true;
    }
  }

  // Handle release
  if (actionButtonState == HIGH && isButtonPressed) {
    unsigned long pressDuration = currentMillis - buttonPressStartTime;
    isButtonPressed = false;

    // If short press, toggle LEDs manually
    if (!isLongPressHandled && pressDuration < 1500) {
      isManualControlEnabled = true;
      manualLEDState = !manualLEDState;

      if (manualLEDState) {
        ledcWrite(LED_RED_CHANNEL, 255);
        ledcWrite(LED_GREEN_CHANNEL, 255);
        ledcWrite(LED_YELLOW_CHANNEL, 255);
        displayOLEDMessage("Action:", "Short: ON");
      } else {
        ledcWrite(LED_RED_CHANNEL, 0);
        ledcWrite(LED_GREEN_CHANNEL, 0);
        ledcWrite(LED_YELLOW_CHANNEL, 0);
        displayOLEDMessage("Action:", "Short: OFF");
      }
    }
  }

  // -------------------- LED MODE BEHAVIOR --------------------
  if (!isManualControlEnabled) {
    switch (activeLightingMode) {

      case MODE_ALTERNATE:
        if (currentMillis - ledBlinkTimer >= LED_BLINK_INTERVAL) {
          ledBlinkTimer = currentMillis;
          isBlinkOn = !isBlinkOn;
          static int currentLEDIndex = 0;
          if (isBlinkOn) {
            currentLEDIndex = (currentLEDIndex + 1) % 3;
            ledcWrite(LED_RED_CHANNEL,   (currentLEDIndex == 0) ? 255 : 0);
            ledcWrite(LED_GREEN_CHANNEL, (currentLEDIndex == 1) ? 255 : 0);
            ledcWrite(LED_YELLOW_CHANNEL,(currentLEDIndex == 2) ? 255 : 0);
          } else {
            ledcWrite(LED_RED_CHANNEL, 0);
            ledcWrite(LED_GREEN_CHANNEL, 0);
            ledcWrite(LED_YELLOW_CHANNEL, 0);
          }
        }
        break;

      case MODE_FADE: {
        unsigned long elapsedTime = (currentMillis - ledFadeTimer) % 2000;
        float timeFraction = elapsedTime / 2000.0f;
        float redValue   = (sin(2 * PI * timeFraction) + 1.0f) / 2.0f;
        float greenValue = (sin(2 * PI * (timeFraction + 1.0 / 3.0)) + 1.0f) / 2.0f;
        float yellowValue= (sin(2 * PI * (timeFraction + 2.0 / 3.0)) + 1.0f) / 2.0f;

        ledcWrite(LED_RED_CHANNEL,   (int)(redValue * 255));
        ledcWrite(LED_GREEN_CHANNEL, (int)(greenValue * 255));
        ledcWrite(LED_YELLOW_CHANNEL,(int)(yellowValue * 255));
        break;
      }

      default:
        break;
    }
  }

  delay(8); // short delay for stability
}
