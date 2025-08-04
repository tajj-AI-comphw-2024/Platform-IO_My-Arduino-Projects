// Libraries & Initialization
#include <Arduino.h>
#include <DHT.h> // DHT sensor library from Adafruit

// Pin Connections & Objects
// RGB LED Pins
const int blueRGBLED = 13;
const int greenRGBLED = 12;
const int redRGBLED = 11;

// Buzzer Pin
const int buzzerPin = 9;

// DHT22 Sensor Pin
const int DHTPIN = 10;
DHT dht(DHTPIN, DHT22); // Initialize DHT sensor

// Non-blocking alert state variables for buzzer and LED
bool alertActive = false;
unsigned long alertStartTime = 0;
unsigned long lastBuzzerTime = 0;
unsigned long lastRedFlashTime = 0;
int buzzerCount = 0;
bool buzzerOn = false;
bool redLedOn = false;

const unsigned long buzzerInterval = 400;      // Interval between buzzer pulses (ms)
const unsigned long buzzerPause = 5000;        // Pause after 3 buzzes (ms)
const unsigned long buzzerPulseDuration = 200; // Buzzer ON duration (ms)
const unsigned long redFlashInterval = 200;    // Red LED flash interval (ms)

// Start alert: buzzer and flashing red LED
void startTempAlert() {
  alertActive = true;
  alertStartTime = millis();
  lastBuzzerTime = millis();
  lastRedFlashTime = millis();
  buzzerCount = 0;
  buzzerOn = false;
  redLedOn = false;
  Serial.println("Temperature Alert!");
}

void setup() {
  // Initialize pin modes
  pinMode(redRGBLED, OUTPUT);
  pinMode(blueRGBLED, OUTPUT);
  pinMode(greenRGBLED, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  delay(1000);      // Allow sensor to power up
  dht.begin();      // Start DHT sensor
  Serial.begin(9600); // Start serial communication
}

unsigned long lastReadTime = 0;
const unsigned long readInterval = 2000; // Sensor read interval (ms)

void loop() {
  unsigned long currentTime = millis(); // Current time (ms)

  // Sensor reading logic
  if (currentTime - lastReadTime >= readInterval) {
    lastReadTime = currentTime;
    float humidity = dht.readHumidity();
    float tempC = dht.readTemperature();
    float tempF = dht.readTemperature(true);

    // Check for sensor errors
    if (isnan(humidity) || isnan(tempC) || isnan(tempF)) {
      Serial.println("Failed to read from DHT22! Check wiring and pull-up resistor.");
      return;
    }

    // LED and alert logic based on temperature
    if (tempC < 20) {
      digitalWrite(blueRGBLED, HIGH);   // Cold: blue
      digitalWrite(greenRGBLED, LOW);
      digitalWrite(redRGBLED, LOW);
      alertActive = false;
    } else if (tempC >= 20 && tempC < 30) {
      digitalWrite(blueRGBLED, LOW);
      digitalWrite(greenRGBLED, HIGH); // Comfortable: green
      digitalWrite(redRGBLED, LOW);
    } else {
      digitalWrite(blueRGBLED, LOW);
      digitalWrite(greenRGBLED, LOW);
      startTempAlert();                // Hot: start alert
    }

    // Print sensor readings to Serial
    float heatIndexC = dht.computeHeatIndex(tempC, humidity, false);
    float heatIndexF = dht.computeHeatIndex(tempF, humidity, true);
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.print("% | Temperatures: ");
    Serial.print(tempC);
    Serial.print("°C, ");
    Serial.print(tempF);
    Serial.print("°F, Heat Indexs: ");
    Serial.print(heatIndexC);
    Serial.print("°C, ");
    Serial.print(heatIndexF);
    Serial.println("°F");
  }

  // Non-blocking alert logic for buzzer and red LED
  if (alertActive) {
    unsigned long now = millis();
    // Flash red LED
    if (now - lastRedFlashTime >= redFlashInterval) {
      lastRedFlashTime = now;
      redLedOn = !redLedOn;
      digitalWrite(redRGBLED, redLedOn ? HIGH : LOW);
    }
    // Buzzer pulse logic
    if (buzzerCount < 3) {
      if (!buzzerOn && now - lastBuzzerTime >= buzzerInterval) {
        buzzerOn = true;
        lastBuzzerTime = now;
        digitalWrite(buzzerPin, HIGH);
      } else if (buzzerOn && now - lastBuzzerTime >= buzzerPulseDuration) {
        buzzerOn = false;
        lastBuzzerTime = now;
        digitalWrite(buzzerPin, LOW);
        buzzerCount++;
      }
    } else {
      // Pause before restarting buzzer sequence
      if (now - alertStartTime >= buzzerPause) {
        buzzerCount = 0;
        alertStartTime = now;
      }
    }
  } else {
    // Ensure buzzer and red LED are off when not alerting
    digitalWrite(buzzerPin, LOW);
    digitalWrite(redRGBLED, LOW);
  }
}