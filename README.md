# ServoLED Project

This project is designed for an ESP32 microcontroller to control a servo motor, a WS2812 LED ring, and respond to audio input. It also includes time-based automation and a web interface for configuration.

## Features

### 1. Audio Measurement
- Measures audio levels using an analog input (GPIO34).
- Calculates statistical values like the mean and standard deviation (`sigma`) over a specified time period.
- Uses the measured audio levels to control other components.

### 2. WS2812 LED Ring Control
- Dynamically adjusts the brightness of a WS2812 LED ring based on audio levels.
- Uses the `Adafruit_NeoPixel` library for LED control.

### 3. Servo Motor Control
- Controls a servo motor using PWM signals.
- Adjusts the servo motor's position based on the measured audio levels.

### 4. Time-Based Automation
- Synchronizes the current time using NTP (Network Time Protocol).
- Turns the audio and lights on or off based on user-defined "on" and "off" times.

### 5. Task Scheduling
- Uses a task scheduler (`Scheduler`) to manage periodic tasks like audio measurement and LED updates.

### 6. Wi-Fi and Captive Portal
- Connects to a Wi-Fi network.
- Sets up a captive portal for configuration.
- Includes a web server to handle HTTP requests for managing settings.

---

## Hardware Requirements
- **ESP32 Microcontroller**
- **WS2812 LED Ring**
- **Servo Motor**
- **Microphone or Audio Sensor** (connected to GPIO34 for analog input)

---

## Software Requirements
- **PlatformIO** (for building and uploading the code)
- **Adafruit_NeoPixel Library**
- **NTPClient Library**
- **Scheduler Library**

---

## Pin Configuration
- **GPIO34**: Analog input for audio measurement.
- **Servo Pin**: Configurable in the code (default: `D4`).
- **WS2812 LED Ring Pin**: Configurable in the code (default: `GPIO4`).

---

## Installation and Setup

### 1. Clone the Repository
```bash
git clone https://github.com/your-repo/servoLED.git
cd servoLED
