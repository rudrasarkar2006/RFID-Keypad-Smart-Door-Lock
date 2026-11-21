# 🔐 Dual-Authentication RFID & Keypad Smart Door Lock

![Arduino](https://img.shields.io/badge/Platform-Arduino%20UNO-00979D?style=for-the-badge&logo=arduino&logoColor=white)
![Language](https://img.shields.io/badge/Language-C%2B%2B-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)

An embedded mechatronics access-control system built on **Arduino UNO**. It combines **13.56 MHz RFID authentication** and a **4×4 Matrix Keypad** with custom frequency-modulated audio feedback, dual-LED status indicators, and an automated servo deadbolt mechanism.

---

## 📋 Table of Contents
- [Key Features](#-key-features)
- [Hardware Components](#-hardware-components)
- [Circuit Wiring & Pinout](#-circuit-wiring--pinout)
- [3D Hardware Layout](#-3d-hardware-layout)
- [Required Libraries](#-required-libraries)
- [Installation & Usage](#-installation--usage)
- [Customization](#-customization)

---

## ✨ Key Features

* **Dual-Mode Authentication:** Unlocks via an authorized RFID card/key fob **or** a 4-digit backup PIN (`#` to submit, `*` to clear).
* **16-Note Musical Keypad:** Maps every button (`0–9`, `A–D`, `*`, `#`) to a unique musical pitch (`1047 Hz` – `4435 Hz`) for instant tactile feedback.
* **Custom Sound Effects:**
  * **Unlock:** Ascending *Sci-Fi Laser Chirp* (`1800 Hz` $\rightarrow$ `3200 Hz`).
  * **Access Denied:** 3-note descending *Failure Warning Tone* (`1800 Hz` $\rightarrow$ `1400 Hz` $\rightarrow$ `950 Hz`).
  * **PIN Clear (`*`):** Crisp double-beep confirmation.
* **Active-LOW Thermal Protection:** Re-arms `BUZZER_PIN` to `HIGH` immediately after every tone, eliminating idle current draw and preventing buzzer overheating.
* **Automated Security Relock:** Rotates the servo deadbolt from `0°` (Locked) to `90°` (Unlocked) for **4 seconds** before automatically re-locking.
* **Zero-Waste Pin Architecture:** Utilizes analog ports (`A0`–`A4`) as digital I/O and PWM pins to run five peripherals cleanly on a single Arduino UNO.

---

## 🛠️ Hardware Components

| Component | Qty | Role in System |
| :--- | :---: | :--- |
| **Arduino UNO** | 1 | Central microcontroller & verification logic |
| **MFRC522 RFID Module** | 1 | 13.56 MHz contactless card/tag reader (SPI) |
| **4×4 Matrix Keypad** | 1 | Manual passcode input interface |
| **SG90 Micro Servo Motor** | 1 | Electromechanical deadbolt actuator (`0°`–`90°`) |
| **Piezo Buzzer Module** | 1 | Frequency-modulated audio feedback (`tone()`) |
| **Red & Blue 5mm LEDs** | 2 | Visual status indicators (Locked / Unlocked) |
| **220Ω Resistors** | 2 | Current-limiting protection for LEDs |
| **Breadboard & Jumper Wires** | — | Shared `+5V` and `GND` power distribution hub |

---

## 🔌 Circuit Wiring & Pinout

![Wiring Diagram](images/wiring_diagram.jpg)

### Pin Mapping Table

| Module | Module Pin | Connects To | Notes |
| :--- | :--- | :--- | :--- |
| **MFRC522 RFID** | `3.3V` | **Arduino `3.3V`** | **3.3V ONLY** (5V will damage the module) |
| | `GND` | **Breadboard `GND` Rail** | Common ground |
| | `RST` | **Arduino `D9`** | Reset line |
| | `SDA (SS)` | **Arduino `D10`** | SPI Slave Select |
| | `MOSI` | **Arduino `D11`** | SPI Master Out Slave In |
| | `MISO` | **Arduino `D12`** | SPI Master In Slave Out |
| | `SCK` | **Arduino `D13`** | SPI Serial Clock |
| | `IRQ` | *Unconnected* | Leave empty |
| **4×4 Keypad** | `R1, R2, R3, R4` | **Arduino `D2, D3, D4, D5`** | Pins 6–9 on 10-pin header |
| | `C1, C2, C3, C4` | **Arduino `D6, D7, D8, A0`** | Pins 2–5 on 10-pin header (`Pin 1` & `10` NC) |
| **SG90 Servo** | `Signal` (Orange) | **Arduino `A1`** | PWM control (`0°` locked / `90°` unlocked) |
| | `VCC` (Red) | **Breadboard `+5V` Rail** | 5V power |
| | `GND` (Brown) | **Breadboard `GND` Rail** | Common ground |
| **Buzzer Module** | `S` / `I/O` | **Arduino `A2`** | `tone()` output + Active-LOW idle |
| | `VCC` / `GND` | **Breadboard `+5V` / `GND`** | Shared power rails |
| **Blue LED** | Anode (`+`) | **Arduino `A3`** (via `220Ω`) | Cathode (`-`) to Breadboard `GND` |
| **Red LED** | Anode (`+`) | **Arduino `A4`** (via `220Ω`) | Cathode (`-`) to Breadboard `GND` |

---

## 🖥️ 3D Hardware Layout

![3D Circuit Preview](images/3d_circuit_preview.jpg)

---

## 📚 Required Libraries

Install these libraries via **Arduino IDE $\rightarrow$ Sketch $\rightarrow$ Include Library $\rightarrow$ Manage Libraries**:
1. **MFRC522** by *GithubCommunity*
2. **Keypad** by *Mark Stanley, Alexander Brevig*
3. **Servo** *(Built-in with Arduino IDE)*

---

## 🚀 Installation & Usage

1. **Clone the repository:**
   ```bash
   git clone [https://github.com/YOUR_USERNAME/RFID-Keypad-Smart-Door-Lock.git](https://github.com/YOUR_USERNAME/RFID-Keypad-Smart-Door-Lock.git)
