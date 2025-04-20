/*******************************************************************************
 * Project: Tilt-Triggered Alarm System
 * 
 * Description:
 *    This program implements a tilt-activated alarm system using an Arduino.
 *    When the device is tilted, it triggers an alarm through an active buzzer.
 *    The system uses a tilt ball switch sensor for motion detection and
 *    provides serial monitoring for debugging purposes.
 * 
 * Hardware Requirements:
 *    - Arduino Board
 *    - Active Buzzer (connected to pin 11)
 *    - Tilt Ball Switch Sensor (connected to pin 7)
 * 
 * Created: April 20, 2025
 * Author: Troy Johnson
 * 
 * Pin Configuration:
 *    - Digital Pin 11: Active Buzzer
 *    - Digital Pin 7:  Tilt Ball Switch
 * 
 * Behavior:
 *    - When level: Buzzer OFF
 *    - When tilted: Buzzer ON
 * 
 * Serial Communication:
 *    - Baud Rate: 9600
 *    - Outputs sensor state and buzzer status
 * 
 * IDE USED: Visual Studio Code with PlatformIO Extension
 * NOTE: Files via PlatformIO (C/CPP(C/C++ files)) are not compatible with Arduino IDE (ino files).
 ******************************************************************************/
#include <Arduino.h>

// Pin Definitions
#define activeBuzzer 11    // Active buzzer connected to digital pin 11
#define tiltBall 7         // Tilt ball switch sensor connected to digital pin 7

// Global Variables
int tiltSignal;           // Variable to store the tilt sensor reading

void setup() {
  // Initialize the active buzzer pin as output
  pinMode(activeBuzzer, OUTPUT);
  digitalWrite(activeBuzzer, LOW);    // Initially set buzzer to OFF
  
  // Initialize the tilt sensor pin as input with internal pull-up resistor
  pinMode(tiltBall, INPUT);
  digitalWrite(tiltBall, HIGH);       // Enable internal pull-up resistor
  
  // Initialize serial communication for debugging
  Serial.begin(9600);                 // Start serial at 9600 baud rate
}

void loop() {
  // Read the current state of the tilt sensor
  tiltSignal = digitalRead(tiltBall);
  
  // If tilt sensor is at rest (ball making contact, signal LOW)
  if(tiltSignal == 0){
    digitalWrite(activeBuzzer, LOW);     // Turn off the buzzer
    Serial.println("Buzzer Deactivated");
    Serial.println("Signal Val: "+tiltSignal);
    delay(1000);                       // Delay for 1 second to avoid rapid toggling
  }
  // If tilt sensor is triggered (ball broke contact, signal HIGH)
  else{
    digitalWrite(activeBuzzer, HIGH);    // Turn on the buzzer
    Serial.println("Buzzer Activated");
    Serial.println("Signal Val: "+tiltSignal);
    delay(1000);                       // Delay for 1 second to avoid rapid toggling
  }
}
