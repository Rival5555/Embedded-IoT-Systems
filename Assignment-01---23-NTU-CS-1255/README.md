# ESP32 Smart LED Controller with OLED Display & Buzzer

## 📋 Project Overview

This project implements an intelligent LED control system using ESP32 with multiple lighting modes, interactive button controls, visual OLED feedback, and audio buzzer alerts.

**Developer:** HASSAN AL  
**Registration:** 23-NTU-CS-1255

---

## 🎯 Features

- **4 LED Control Modes:**

  - OFF - All LEDs disabled
  - ALTERNATE - Sequential LED blinking pattern
  - ALL ON - All LEDs at maximum brightness
  - PWM FADE - Smooth sinusoidal fade effect

- **Interactive Controls:**

  - Mode Button: Cycle through lighting modes
  - Action Button: Short press toggles LEDs, long press activates buzzer
  - Boot Button: Emergency reset to OFF state

- **Visual Feedback:** Real-time status display on 0.96" OLED screen
- **Audio Alerts:** Buzzer activation on long press detection

---

## 🔗 Wokwi Simulation

**[▶️ Run Live Simulation on Wokwi](https://wokwi.com/)**

_Note: Replace with your actual Wokwi project link after uploading the simulation_

---

## 🔌 Pin Configuration

### LED Connections

| Component  | ESP32 Pin | PWM Channel |
| ---------- | --------- | ----------- |
| Red LED    | GPIO 19   | Channel 1   |
| Green LED  | GPIO 18   | Channel 2   |
| Yellow LED | GPIO 5    | Channel 3   |

### Button Connections

| Button        | ESP32 Pin | Configuration           |
| ------------- | --------- | ----------------------- |
| Mode Button   | GPIO 13   | INPUT_PULLUP            |
| Action Button | GPIO 12   | INPUT_PULLUP            |
| Boot Button   | GPIO 0    | INPUT_PULLUP (Built-in) |

### Buzzer Connection

| Component    | ESP32 Pin | PWM Channel |
| ------------ | --------- | ----------- |
| Piezo Buzzer | GPIO 27   | Channel 0   |

### OLED Display (I2C)

| OLED Pin | ESP32 Pin | Protocol    |
| -------- | --------- | ----------- |
| SDA      | GPIO 21   | I2C Data    |
| SCL      | GPIO 22   | I2C Clock   |
| Address  | 0x3C      | I2C Address |

---

## 🛠️ Hardware Requirements

- **ESP32 Development Board** (NodeMCU DevKit v1 or equivalent)
- **3× LEDs** (Red, Green, Yellow) with appropriate resistors (220Ω recommended)
- **2× Push Buttons** (Tactile switches)
- **1× Piezo Buzzer** (Active or Passive)
- **1× 0.96" OLED Display** (SSD1306, I2C interface)
- **Breadboard and Jumper Wires**
- **USB Cable** for programming

---

## 📚 Required Libraries

Install the following libraries via Arduino IDE Library Manager:

```
- Adafruit GFX Library
- Adafruit SSD1306
- Wire (Built-in)
```

---

## 🚀 How to Use

### Mode Button Operations

1. **Press once** → Cycle to next mode (OFF → ALTERNATE → ALL ON → FADE → OFF...)
2. OLED displays current mode immediately

### Action Button Operations

1. **Short Press (<1.5s)** → Toggle all LEDs ON/OFF manually
2. **Long Press (≥1.5s)** → Activate buzzer tone (2500Hz, 300ms)

### Boot Button Operation

1. **Press** → Emergency reset to OFF mode

### Lighting Modes Explained

**MODE_OFF**

- All LEDs turned off
- System in standby state

**MODE_ALTERNATE**

- LEDs blink sequentially (Red → Green → Yellow)
- 400ms interval between transitions
- Each LED turns on individually while others remain off

**MODE_ALL_ON**

- All three LEDs at full brightness (255/255)
- Constant illumination

**MODE_FADE**

- Smooth PWM fade using sinusoidal waves
- Each LED follows phase-shifted sine pattern
- Creates flowing color transition effect
- 2-second cycle period

---

## 📸 Project Screenshots

### Circuit Diagram

![Circuit Wiring](screenshots/Alloff.jpg)
_Complete wiring schematic showing all connections_

### OLED Display States

![OLED Mode OFF](screenshots/Alloff.jpg)
_Display showing OFF mode_

![OLED Mode Alternate](screenshots/Alternate.jpg)
_Display showing ALTERNATE mode_

![OLED Mode All ON](screenshots/All_on.jpg)
_Display showing ALL ON mode_

![OLED Mode Fade](screenshots/PWM_Fade.jpg)
_Display showing PWM FADE mode_

### Button Actions

![Action Short Press](screenshots/Short_press.jpg)
_Short press toggle feedback_

![Action Long Press](screenshots/Long_press.mp4)
_Long press buzzer activation_

---

## ⚙️ Technical Specifications

**PWM Configuration:**

- Resolution: 8-bit (0-255)
- LED Frequency: 5000 Hz
- Buzzer Frequency: 2000 Hz (base), 2500 Hz (tone)

**Timing Parameters:**

- LED Blink Interval: 400ms
- Button Debounce: 50ms
- Long Press Threshold: 1500ms
- Loop Delay: 8ms

**OLED Specifications:**

- Resolution: 128×64 pixels
- Driver: SSD1306
- Interface: I2C

---

## 🔧 Troubleshooting

**OLED Not Displaying:**

- Check I2C connections (SDA/SCL)
- Verify I2C address is 0x3C
- Ensure proper power supply

**LEDs Not Working:**

- Verify PWM channel assignments
- Check LED polarity
- Confirm resistor values

**Buttons Not Responding:**

- Check pull-up configuration
- Verify debounce timing
- Test button continuity

**Buzzer Silent:**

- Check PWM channel 0 assignment
- Verify buzzer polarity
- Test with different frequencies

---

## 📝 Code Structure

```
setup()
├── Pin Configuration
├── PWM Channel Setup
├── OLED Initialization
└── Initial Mode Setting

loop()
├── Mode Button Handler
├── Boot Button Handler
├── Action Button Handler
│   ├── Short Press Detection
│   └── Long Press Detection
└── LED Pattern Execution
    ├── Alternate Blink Logic
    ├── Fade Calculation
    └── Manual Override
```

---

## 📄 License

This project is created for educational purposes as part of academic coursework.

---

## 👤 Author

**HASSAN AL**  
Registration: 23-NTU-CS-1255  
Project: ESP32 Smart LED Controller

---

## 🙏 Acknowledgments

- Adafruit for GFX and SSD1306 libraries
- ESP32 Arduino Core developers
- Wokwi for simulation platform

---

_Last Updated: October 2025_
