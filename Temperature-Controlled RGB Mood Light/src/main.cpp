//Libaries & Intialization
#include <Arduino.h>
#include <DHT.h> // DHT sensor library from Adafruit

//Pin Connections & Objects
// RGB Connections
const int blueRGBLED = 13;
const int greenRGBLED = 12;
const int redRGBLED = 11;

//DHT11 Sensor
const int DHTPIN = 10;
DHT dht(DHTPIN, DHT22); // Initialize DHT sensor for normal 16mhz Arduino

void setup() {
  // put your setup code here, to run once:
  pinMode(redRGBLED, OUTPUT);
  pinMode(blueRGBLED, OUTPUT);
  pinMode(greenRGBLED, OUTPUT);
  delay(1000); // Allow sensor to power up
  dht.begin(); // Start the DHT sensor
  Serial.begin(9600); // Start the serial communication at 9600 baud rate
}

void loop() {
  delay(2000); // Wait for 2 seconds before the next reading
  float humidity = dht.readHumidity();
  float tempC = dht.readTemperature();
  float tempF = dht.readTemperature(true);  
  if (isnan(humidity) || isnan(tempC) || isnan(tempF)) {
    Serial.println("Failed to read from DHT11! Check wiring and pull-up resistor.");
    return;
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
