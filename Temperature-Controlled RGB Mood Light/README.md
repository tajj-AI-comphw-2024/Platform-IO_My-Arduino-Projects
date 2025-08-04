# Temperature-Controlled RGB Mood Light

## Overview

This project uses an Arduino and a DHT22 sensor to create a temperature-controlled RGB mood light with buzzer alerts.

## Features

- RGB LED changes color based on temperature
- Buzzer alerts for high temperature
- Non-blocking logic for real-time sensor reading and alerting
- Serial output for temperature and humidity

## Hardware

- Arduino (Uno, Nano, etc.)
- DHT22 temperature/humidity sensor
- RGB LED (common cathode)
- Active buzzer
- Jumper wires

## Pin Connections

- Blue RGB LED: Pin 13
- Green RGB LED: Pin 12
- Red RGB LED: Pin 11
- Buzzer: Pin 9
- DHT22 Data: Pin 10

## Arduino Code

See `src/main.cpp` for the full code. Key logic:

- Reads temperature and humidity every 2 seconds
- Changes LED color based on temperature
- Activates buzzer and flashes red LED for high temperature
- Outputs readings to Serial

## How to Run

1. Upload `src/main.cpp` to your Arduino.
2. Connect hardware as described above.
3. Open the Serial Monitor in Arduino IDE to view real-time temperature and humidity data.

### Using Arduino IDE

To use this project in the Arduino IDE, copy the code from `src/main.cpp` and paste it into a new `.ino` file. The code structure is compatible and should work without modification.

## Example Serial Output

```
Humidity: 72.30% | Temperatures: 23.90°C, 75.02°F, Heat Indexs: 24.23°C, 75.62°F
```

## Portfolio Notes

- Demonstrates real-time embedded programming
- Includes data logging and visualization

---

Feel free to contact me for more details or collaboration!
