# ESP32 Emergency Alert System with GSM & GPS

An IoT-based emergency alert system designed as a thesis project. The system uses an **ESP32** microcontroller integrated with a **SIM900 GSM module** and a **NEO-6M GPS module** to send distress SMS alerts with real-time location data and trigger local audio alarms when buttons are pressed.

## 🚀 Features
* **Dual-Recipient SMS Alerts:** Sends emergency SMS messages to two pre-configured numbers with precise GPS coordinates.
* **Emergency Voice Call:** Automatically dials an emergency contact after sending SMS alerts.
* **Real-time GPS Tracking:** Integrates the TinyGPS++ library to parse live latitude and longitude coordinates.
* **Local Sound Panic Alarm:** Features a loud buzzer alarm that can be activated and deactivated with a hardware push button.
* **Smart Call Answering:** Automatically handles incoming calls during emergency routines.

## 🛠️ Hardware Components & Pin Mapping

### Microcontroller
* **ESP32 DevKit V1**

### Module Connections

| Component | Component Pin | ESP32 GPIO Pin | Description |
| :--- | :--- | :--- | :--- |
| **SIM900 GSM** | TX | **GPIO 17 (RX2)** | Serial Data Receive |
| **SIM900 GSM** | RX | **GPIO 16 (TX2)** | Serial Data Transmit |
| **NEO-6M GPS** | TX | **GPIO 26 (RX1)** | Serial Data Receive |
| **NEO-6M GPS** | RX | **GPIO 25 (TX1)** | Serial Data Transmit |

### Peripherals

| Component | ESP32 GPIO Pin | Connection Type | Description |
| :--- | :--- | :--- | :--- |
| **Push Button 1** | **GPIO 12** | INPUT_PULLUP | Activates SMS / Call Emergency Sequence |
| **Push Button 2** | **GPIO 14** | INPUT_PULLUP | Toggles Panic Noise Buzzer Alarm |
| **Buzzer** | **GPIO 27** | OUTPUT | High/Low frequency oscillation alarm |

---

## ⚙️ Configuration Before Uploading
Open the primary `.ino` file and update the placeholders with your active phone numbers:

```cpp
String phoneNumber = "xxxx-xxx-xxxx";  // First emergency contact
String phoneNumber2 = "xxxx-xxx-xxxx"; // Second emergency contact & Voice call recipient
```

---

## 📦 Required Libraries
Ensure you have the following library installed in your Arduino IDE before compiling:
* **TinyGPS++** by Mikal Hart (Available via the Arduino Library Manager)

## 🔧 Installation & Setup
1. Clone this repository or download the source code.
2. Open the project file in **Arduino IDE**.
3. Select your **ESP32 Dev Module** board and the correct COM Port.
4. Ensure your hardware is wired exactly as described in the pin mapping table.
5. Provide an external power source for the SIM900 module if required (it can draw high current spikes during transmission).
6. Upload the code and open the **Serial Monitor** at `115200` baud rate to inspect system logs.

## 📜 License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
