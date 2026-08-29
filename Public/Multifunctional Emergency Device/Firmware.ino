#include <HardwareSerial.h>
#include <TinyGPS++.h>

#define RXD2 17  // SIM900 TX -> ESP32 GPIO17 (RX2)
#define TXD2 16  // SIM900 RX -> ESP32 GPIO16 (TX2)
#define RXD1 26  // NEO-6M TX -> ESP32 GPIO26 (RX1)
#define TXD1 25  // NEO-6M RX -> ESP32 GPIO25 (TX1)

#define PUSHBUTTON1 12
#define PUSHBUTTON2 14
#define BUZZER 27   // Buzzer on GPIO25

#define HIGH_FREQ  1500   // High pitch frequency (Hz)
#define LOW_FREQ   1500    // Lower pitch frequency (Hz)
#define BEEP_DELAY 80     // Delay between tones (milliseconds)  
#define ALARM_TIME 5000   // Max alarm duration (5 seconds)
#define DEBOUNCE_TIME 150  // Button debounce time

bool inCall = false;
bool alarmActive = false;   // Track if the alarm is ON or OFF

String phoneNumber = "xxxx-xxx-xxxx";
String phoneNumber2 = "xxxx-xxx-xxxx";
String message = "Emergency Alert! User pressed the emergency button! Last location active:";

unsigned long lastButtonPress1 = 0;
unsigned long lastButtonPress2 = 0;

// Use predefined Serial1 & Serial2 for ESP32
TinyGPSPlus gps; // TinyGPS++ Object

void setup() {
  Serial.begin(115200);  // Serial Monitor
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);  // SIM900 (GSM)
  Serial1.begin(9600, SERIAL_8N1, RXD1, TXD1);  // NEO-6M (GPS)

  pinMode(PUSHBUTTON1, INPUT_PULLUP);
  pinMode(PUSHBUTTON2, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);
  
  digitalWrite(BUZZER, LOW);

  Serial.println("Initializing SIM900...");
  delay(1000);
  Serial2.println("AT");  // Test connection
  updateSerial();
}

void loop() {
  unsigned long currentMillis = millis();
  
  // Check SMS button press with debounce
  if (digitalRead(PUSHBUTTON1) == LOW && currentMillis - lastButtonPress1 > DEBOUNCE_TIME) {
    delay(DEBOUNCE_TIME);  // Additional debounce
    lastButtonPress1 = currentMillis;
    SendMessage();
  }

  // Check noise alarm button press with debounce
  if (digitalRead(PUSHBUTTON2) == LOW && currentMillis - lastButtonPress2 > DEBOUNCE_TIME) {
    delay(DEBOUNCE_TIME);  // Additional debounce
    lastButtonPress2 = currentMillis;
    NoiseAlarm();
  }

  // Read GPS Data
  while (Serial1.available()) {
    gps.encode(Serial1.read());
  }
}

void SendMessage() {
  Serial2.println("AT+CMGF=1");  // Set SMS mode
  updateSerial();

  Serial.println("Checking for call status...");
  unsigned long startTime = millis();
  while (millis() - startTime < 5000) {
    if (Serial2.available()) {
      String response = Serial2.readString();
      Serial.println("Response: " + response);
      if (response.indexOf("RING") != -1) {
        Serial.println("Incoming call detected! Answering...");
        Serial2.println("ATA");
        updateSerial();
        inCall = true;
        return;
      }
    }
  }

  if (inCall) {
    Serial.println("Ending active call...");
    Serial2.println("ATH");
    updateSerial();
    inCall = false;
    return;
  }
 
  Serial.println("No incoming call detected. Sending SMS...");

  String gpsMessage = message + getGPSData();  // Append GPS coordinates
  sendSMS(phoneNumber, gpsMessage);
  delay(3000);
  sendSMS(phoneNumber2, gpsMessage);
  delay(5000);

  Serial2.println("ATD" + phoneNumber2 + ";");  // Set SMS mode
  updateSerial();
  Serial.println("Calling " + phoneNumber2 + "...");
  delay(100000);
  Serial2.println("ATH");

}

void sendSMS(String number, String msg) {
  Serial2.print("AT+CMGS=\"");
  Serial2.print(number);
  Serial2.println("\"");
  updateSerial();

  Serial2.print(msg);
  Serial2.write(26);
  updateSerial();
  Serial.println("SMS Sent to " + number);
}

void NoiseAlarm() {
  alarmActive = !alarmActive;  // Toggle alarm state

  if (alarmActive) {
    Serial.println("Emergency Alarm Activated!");

    unsigned long startTime = millis();

    // Ensure buzzer oscillates properly
    for (int i = 0; i < (ALARM_TIME / (2 * BEEP_DELAY)); i++) {
      tone(BUZZER, HIGH_FREQ);
      delay(BEEP_DELAY);
      tone(BUZZER, LOW_FREQ);
      delay(BEEP_DELAY);

      if (digitalRead(PUSHBUTTON2) == LOW) { // Stop if button is pressed again
        delay(DEBOUNCE_TIME);
        alarmActive = false;
        break;
      }
    }

    Serial.println("Emergency Alarm Stopped!");
    noTone(BUZZER);
  } else {
    Serial.println("Buzzer Manually Stopped!");
    noTone(BUZZER);
  }
}

void updateSerial() {
  delay(500);
  while (Serial.available()) {
    Serial2.write(Serial.read());
  }
  while (Serial2.available()) {
    Serial.write(Serial2.read());
  }
}

// Function to Get GPS Coordinates
String getGPSData() {
  String gpsData = "";
  if (gps.location.isValid()) {
    gpsData = "\nLatitude, Longitude: " + String(gps.location.lat(), 6) + ", "+ String(gps.location.lng(), 6);
  } else {
    gpsData = "\nGPS Signal Not Available!";
  }
  return gpsData;
}
