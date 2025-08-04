//Libaries & Intialization
#include <Arduino.h>
#include <DHT.h> // DHT sensor library from Adafruit

//Pin Connections & Objects
// RGB Connections
const int blueRGBLED = 13;
const int greenRGBLED = 12;
const int redRGBLED = 11;

// Active Buzzer Connections
const int buzzerPin = 9; // Buzzer pin

//DHT22 Sensor
const int DHTPIN = 10;
DHT dht(DHTPIN, DHT22); // Initialize DHT sensor for normal 16mhz Arduino

// Non-blocking alert state variables
bool alertActive = false;
unsigned long alertStartTime = 0;
unsigned long lastBuzzerTime = 0;
unsigned long lastRedFlashTime = 0;
int buzzerCount = 0;
bool buzzerOn = false;
bool redLedOn = false;

const unsigned long buzzerInterval = 400; // ms between buzzer pulses
const unsigned long buzzerPause = 5000;   // ms pause after 3 buzzes
const unsigned long buzzerPulseDuration = 200; // ms buzzer ON duration
const unsigned long redFlashInterval = 200; // ms for red LED flash

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
  // put your setup code here, to run once:
  pinMode(redRGBLED, OUTPUT);
  pinMode(blueRGBLED, OUTPUT);
  pinMode(greenRGBLED, OUTPUT);
  pinMode(buzzerPin, OUTPUT); // Set buzzer pin as output
  delay(1000); // Allow sensor to power up
  dht.begin(); // Start the DHT sensor
  Serial.begin(9600); // Start the serial communication at 9600 baud rate
}

unsigned long lastReadTime = 0;
const unsigned long readInterval = 2000; // Read every 2 seconds

void loop() {
  unsigned long currentTime = millis(); // Get the current time in milliseconds
  // Check if it's time to read the sensor
  // If the current time minus the last read time is greater than or equal to the read interval
  // then read the sensor and update the last read time
  if (currentTime - lastReadTime >= readInterval) {
    lastReadTime = currentTime;
    float humidity = dht.readHumidity();
    float tempC = dht.readTemperature();
    float tempF = dht.readTemperature(true);  
    if (isnan(humidity) || isnan(tempC) || isnan(tempF)) {
      Serial.println("Failed to read from DHT22! Check wiring and pull-up resistor.");
      return;
    }
    if (tempC < 20) {
      digitalWrite(blueRGBLED, HIGH);
      digitalWrite(greenRGBLED, LOW);
      digitalWrite(redRGBLED, LOW);
      alertActive = false;
    } else if (tempC >= 20 && tempC < 30) {
      digitalWrite(blueRGBLED, LOW);
      digitalWrite(greenRGBLED, HIGH);
      digitalWrite(redRGBLED, LOW);
    } else {
      digitalWrite(blueRGBLED, LOW);
      digitalWrite(greenRGBLED, LOW);
      startTempAlert();
    }
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
  // Non-blocking alert logic
  if (alertActive) {
    unsigned long now = millis();
    // Red LED flashing
    if (now - lastRedFlashTime >= redFlashInterval) {
      lastRedFlashTime = now;
      redLedOn = !redLedOn;
      digitalWrite(redRGBLED, redLedOn ? HIGH : LOW);
    }
    // Buzzer logic
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
      // Wait for pause before restarting buzzer sequence
      if (now - alertStartTime >= buzzerPause) {
        buzzerCount = 0;
        alertStartTime = now;
      }
    }
  } else {
    // If not alert, make sure buzzer and red LED are off
    digitalWrite(buzzerPin, LOW);
    digitalWrite(redRGBLED, LOW);
  }
}