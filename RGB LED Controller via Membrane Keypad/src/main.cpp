/*
 * File: main.cpp
 * Author: Troy Johnson
 * Date Created: April 26, 2025
 * Description: This program controls an RGB LED using a membrane keypad. 
 *              It includes various modes such as static RGB, brightness adjustment, 
 *              flashing patterns, random colors, and custom color input.
 * 
 * Key Features:
 * - Static RGB Mode (A): Select predefined colors using the keypad (1-8).
 * - Random Color Mode (B): Automatically generates random RGB colors.
 * - Color Cycle Mode (C): Smooth transition through color spectrum with adjustable speed.
 * - Custom Color Mode (D): Input specific RGB values (0-255) for each color channel.
 * - Brightness Adjustment: Increase/decrease LED intensity in 25% increments.
 * - Flashing Patterns: Cycle through LED colors at fixed intervals.
 * - State Preservation: Maintains LED state across mode changes.
 * - Reset Function: Hold * key for 2 seconds to reset to standby mode.
 * 
 * Controls:
 * - A-D: Mode selection
 * - 0-9: Mode-specific functions
 * - *: Reset to standby (long press)
 * - #: Confirm value in custom color mode
 * 
 * Future If Revisting Plans:
 * - Implement EEPROM (Electrical Erasable Read-Only Memory) memory to save last selected mode and colors. Most likely to use external EEPROM.
 * - Add serial control option for keyboard input.
 * - Expand custom patterns and effects library.
 * - Add sound reactivity mode.
 * 
 * Last Updated: May 26, 2025
 */

/*
 * Change Log:
 * - April 26, 2025: Initial version created. Documentation planned and completed.
 * - April 27, 2025: Implemented staticRGB() and standby() functions.
 * - April 28, 2025: Added brightness adjustment menu.
 * - May 1, 2025: Improved staticFlash() to use non-blocking timing.
 * - May 26, 2025: Major update:
 *   - Fixed standby() function double press issue
 *   - Implemented randomColor() function
 *   - Added colorCycle() with speed control
 *   - Added customColor() with RGB value input
 *   - Implemented long-press reset functionality
 *   - Added state preservation in staticRGB mode
 *   - Improved error handling and user feedback
 */

#include <Arduino.h>
#include <Keypad.h>

// RGB LED Pins & Values
#define redTLED 13
#define greenTLED 12
#define blueTLED 11
int redVal = 0, greenVal = 0, blueVal = 0;  // Initialize to off state



//Intializing the keypad
char customKey;
const byte ROWS = 4;
const byte COLS = 4; 
char hexaKeys[ROWS][COLS]={
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}

};

byte rowPins[ROWS] = {23, 25, 27, 29};
byte colPins[COLS] = {31, 33, 35, 37};

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

struct LEDState {
    bool red;
    bool green;
    bool blue;
}; 
LEDState currentState = {false, false, false};

/*----------------------------------------------------------------------------------------------*/
void standby() {
    static unsigned long previousMillis = 0;
    static int currentPhase = 0; // 0=initial flash, 1=delay, 2=fade in, 3=fade out
    static int flashCount = 0;
    static int fadeValue = 0;
    static bool shouldExit = false;
    const unsigned long FLASH_INTERVAL = 250;
    const unsigned long FADE_INTERVAL = 10;
    
    while (!shouldExit) {
        customKey = customKeypad.getKey();
        if (customKey != NO_KEY) {
            Serial.println("Standby Cancelled");
            shouldExit = true;
            break;
        }
        
        unsigned long currentMillis = millis();
        
        switch(currentPhase) {
            case 0: // Initial flash sequence
                if (currentMillis - previousMillis >= FLASH_INTERVAL) {
                    previousMillis = currentMillis;
                    if ((flashCount % 2) == 0) {
                        digitalWrite(redTLED, HIGH);
                        digitalWrite(greenTLED, HIGH);
                        digitalWrite(blueTLED, HIGH);
                    } else {
                        digitalWrite(redTLED, LOW);
                        digitalWrite(greenTLED, LOW);
                        digitalWrite(blueTLED, LOW);
                    }
                    flashCount++;
                    if (flashCount >= 6) { // 3 complete cycles
                        currentPhase = 1;
                        previousMillis = currentMillis; // Reset timer for delay
                    }
                }
                break;
                
            case 1: // Delay phase
                if (currentMillis - previousMillis >= 1000) {
                    currentPhase = 2;
                    previousMillis = currentMillis;
                }
                break;
                
            case 2: // Fade in
                if (currentMillis - previousMillis >= FADE_INTERVAL) {
                    previousMillis = currentMillis;
                    analogWrite(redTLED, fadeValue);
                    analogWrite(greenTLED, fadeValue);
                    analogWrite(blueTLED, fadeValue);
                    fadeValue++;
                    if (fadeValue > 255) {
                        currentPhase = 3;
                        fadeValue = 255;
                    }
                }
                break;
                
            case 3: // Fade out
                if (currentMillis - previousMillis >= FADE_INTERVAL) {
                    previousMillis = currentMillis;
                    analogWrite(redTLED, fadeValue);
                    analogWrite(greenTLED, fadeValue);
                    analogWrite(blueTLED, fadeValue);
                    fadeValue--;
                    if (fadeValue < 0) {
                        currentPhase = 2;
                        fadeValue = 0;
                    }
                }
                break;
        }
    }
}

void errorFlash() {
    static unsigned long previousMillis = 0;
    static int flashCount = 0;
    static bool ledState = false;
    
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= 250) {
        previousMillis = currentMillis;
        ledState = !ledState;
        digitalWrite(redTLED, ledState);
        if (!ledState) flashCount++;
        
        if (flashCount >= 3) {
            digitalWrite(redTLED, LOW);
            flashCount = 0;
            return;
        }
    }
}

void callMode(){
  if(customKey == 'A') {
    Serial.println("Static RGB Mode Selected");
    staticRGB();
  } else if (customKey == 'B') {
    Serial.println("Brightness Mode Selected");
    brightness();
  } else if (customKey == 'C') {
    Serial.println("Static Flash Mode Selected");
    staticFlash();
  } else if (customKey == 'D') {
    Serial.println("Exiting to Main Menu");
    return; // Exit to main menu
  } else {
    Serial.println("Invalid Input. Please try again.");
    errorFlash(); // Flash red LED for invalid input
  }
  
}
/*----------------------------------------------------------------------------------------------*/
void brightness(){
    Serial.println(F("Brightness Key Pressed"));
    Serial.println(F("Press 1. to Increase Brightness"));
    Serial.println(F("Press 2. to Decrease Brightness"));
    Serial.println(F("Press 3. to Exit Brightness Menu"));
    Serial.println(F("Modes (A-D) will also exit menu"));
    
    const int STEP = 64;  // 25% of 255
    float ratios[3] = {0}; // Store color ratios
    
    // Calculate initial color ratios if any color is non-zero
    int maxVal = max(max(redVal, greenVal), blueVal);
    if (maxVal > 0) {
        ratios[0] = (float)redVal / maxVal;
        ratios[1] = (float)greenVal / maxVal;
        ratios[2] = (float)blueVal / maxVal;
    }
    
    while (true) {
        customKey = customKeypad.getKey();
        if (!customKey) continue;
        
        switch(customKey) {
            case '1': {
                Serial.println(F("Increasing Brightness"));
                int newMax = min(255, maxVal + STEP);
                if (newMax != maxVal) {
                    maxVal = newMax;
                    // Apply ratios to maintain color balance
                    redVal = round(ratios[0] * maxVal);
                    greenVal = round(ratios[1] * maxVal);
                    blueVal = round(ratios[2] * maxVal);
                    applyLEDState(currentState);
                }
                break;
            }
            case '2': {
                Serial.println(F("Decreasing Brightness"));
                int newMax = max(0, maxVal - STEP);
                if (newMax != maxVal) {
                    maxVal = newMax;
                    // Apply ratios to maintain color balance
                    redVal = round(ratios[0] * maxVal);
                    greenVal = round(ratios[1] * maxVal);
                    blueVal = round(ratios[2] * maxVal);
                    applyLEDState(currentState);
                }
                break;
            }
            case '3':
            case 'A':
            case 'B':
            case 'C':
            case 'D':
                Serial.println(F("Exiting Brightness Menu"));
                return;
            default:
                Serial.println(F("Invalid Input"));
                errorFlash();
        }
    }
}
/*----------------------------------------------------------------------------------------------*/
void staticFlash() {
    // Pre-defined states stored in PROGMEM to save dynamic memory
    static const uint8_t PROGMEM states[][3] = {
        {1, 0, 0}, // Red
        {0, 1, 0}, // Green
        {0, 0, 1}, // Blue
        {1, 1, 0}, // Yellow
        {0, 1, 1}, // Cyan
        {1, 0, 1}, // Magenta
        {1, 1, 1}  // White
    };
    
    static unsigned long previousMillis = 0;
    static uint8_t state = 0;
    const unsigned long interval = 1000;

    while (true) {
        customKey = customKeypad.getKey();
        if (customKey != NO_KEY) {
            Serial.println(F("Exiting Static Flash Mode"));
            break;
        }

        unsigned long currentMillis = millis();
        if (currentMillis < previousMillis) {
            previousMillis = currentMillis; // Handle overflow
        }
        
        if (currentMillis - previousMillis >= interval) {
            previousMillis = currentMillis;
            
            // Apply the current state
            digitalWrite(redTLED, pgm_read_byte(&states[state][0]));
            digitalWrite(greenTLED, pgm_read_byte(&states[state][1]));
            digitalWrite(blueTLED, pgm_read_byte(&states[state][2]));
            
            state = (state + 1) % 7;
        }
    }
}

void printStaticRGBMenu() {
    Serial.println("Static RGB Menu Options:");
    Serial.println("1. Red LED On");
    Serial.println("2. Green LED On");
    Serial.println("3. Blue LED On");
    Serial.println("4. Yellow LED On");
    Serial.println("5. Cyan LED On");
    Serial.println("6. Magenta LED On");
    Serial.println("7. White LED On");
    Serial.println("8. LED Off");
    Serial.println("9. Static Flash Mode");
    Serial.println("0. Brightness Menu");
    Serial.println("A. Show Menu");
    Serial.println("B/C/D. Exit to Main Menu");
}

void applyLEDState(LEDState state) {
    // Apply LED states with brightness preservation
    if (state.red) {
        analogWrite(redTLED, redVal);
    } else {
        analogWrite(redTLED, 0);
    }
    if (state.green) {
        analogWrite(greenTLED, greenVal);
    } else {
        analogWrite(greenTLED, 0);
    }
    if (state.blue) {
        analogWrite(blueTLED, blueVal);
    } else {
        analogWrite(blueTLED, 0);
    }
}
void staticRGB() {
    printStaticRGBMenu();
    while (true) {
        customKey = customKeypad.getKey();
        
        if (customKey) {
            LEDState previousState = currentState; // Store current state before changes
            
            switch (customKey) {
                case 'A':
                    printStaticRGBMenu();
                    break;
                case '1':
                    Serial.println("Red LED On");
                    currentState = {true, false, false};
                    break;
                case '2':
                    Serial.println("Green LED On");
                    currentState = {false, true, false};
                    break;
                case '3':
                    Serial.println("Blue LED On");
                    currentState = {false, false, true};
                    break;
                case '4':
                    Serial.println("Yellow LED On");
                    currentState = {true, true, false};
                    break;
                case '5':
                    Serial.println("Cyan LED On");
                    currentState = {false, true, true};
                    break;
                case '6':
                    Serial.println("Magenta LED On");
                    currentState = {true, false, true};
                    break;
                case '7':
                    Serial.println("White LED On");
                    currentState = {true, true, true};
                    break;
                case '8':
                    Serial.println("LED Off");
                    currentState = {false, false, false};
                    break;
                case '9':
                    staticFlash();
                    Serial.println("Back to Static RGB Mode");
                    break;
                case '0':
                    brightness();
                    break;
                case 'B':
                case 'C':
                case 'D':
                    Serial.println("Exiting Static RGB Mode");
                    return;
                default:
                    Serial.println("Invalid Input. Please try again.");
                    errorFlash();
                    currentState = previousState; // Restore previous state
            }
            
            // Apply the current state
            applyLEDState(currentState);
        }
        customKey = NO_KEY;
    }
}
/*----------------------------------------------------------------------------------------------*/
void randomColor() {
    Serial.println("Random Color Mode");
    Serial.println("Press any key to exit");
    
    unsigned long previousMillis = 0;
    const unsigned long interval = 2000; // Change color every 2 seconds
    
    while (true) {
        customKey = customKeypad.getKey();
        if (customKey != NO_KEY) {
            Serial.println("Exiting Random Color Mode");
            break;
        }
        
        unsigned long currentMillis = millis();
        if (currentMillis - previousMillis >= interval) {
            previousMillis = currentMillis;
            
            // Generate random RGB values
            redVal = random(256);
            greenVal = random(256);
            blueVal = random(256);
            
            analogWrite(redTLED, redVal);
            analogWrite(greenTLED, greenVal);
            analogWrite(blueTLED, blueVal);
        }
    }
}
/*----------------------------------------------------------------------------------------------*/
void colorCycle() {
    Serial.println(F("Color Cycle Mode"));
    Serial.println(F("Press 1-9 to adjust speed"));
    Serial.println(F("Press any other key to exit"));
    
    static unsigned long previousMillis = 0;
    static unsigned long interval = 100; // Default speed
    static float hue = 0;
    const float HUE_STEP = 0.5; // Smaller step for smoother transitions
    const float GAMMA = 2.2; // Gamma correction factor
    
    while (true) {
        customKey = customKeypad.getKey();
        if (customKey) {
            if (customKey >= '1' && customKey <= '9') {
                interval = (10 - (customKey - '0')) * 25; // Finer speed control
                Serial.print(F("Speed set to: "));
                Serial.println(interval);
            } else {
                Serial.println(F("Exiting Color Cycle Mode"));
                break;
            }
        }
        
        unsigned long currentMillis = millis();
        if (currentMillis - previousMillis >= interval) {
            previousMillis = currentMillis;
            
            // Update hue with smoother step
            hue += HUE_STEP;
            if (hue >= 360) hue = 0;
            
            // Improved HSV to RGB conversion
            float h = hue / 60.0f;
            float s = 1.0f; // Full saturation
            float v = 1.0f; // Full value/brightness
            
            float c = v * s;
            float x = c * (1 - abs(fmod(h, 2.0f) - 1));
            float m = v - c;
            
            float r, g, b;
            if (h < 1) {
                r = c; g = x; b = 0;
            } else if (h < 2) {
                r = x; g = c; b = 0;
            } else if (h < 3) {
                r = 0; g = c; b = x;
            } else if (h < 4) {
                r = 0; g = x; b = c;
            } else if (h < 5) {
                r = x; g = 0; b = c;
            } else {
                r = c; g = 0; b = x;
            }
            
            // Apply gamma correction and scale to 0-255
            redVal = round(pow((r + m), 1.0/GAMMA) * 255);
            greenVal = round(pow((g + m), 1.0/GAMMA) * 255);
            blueVal = round(pow((b + m), 1.0/GAMMA) * 255);
            
            analogWrite(redTLED, redVal);
            analogWrite(greenTLED, greenVal);
            analogWrite(blueTLED, blueVal);
        }
    }
    // Reset the hue for next time
    hue = 0;
}
/*----------------------------------------------------------------------------------------------*/
void customColor() {
    int values[3] = {0, 0, 0}; // Store RGB values
    int currentColor = 0; // 0=Red, 1=Green, 2=Blue
    int currentValue = 0;
    
    Serial.println("Custom Color Mode");
    Serial.println("Enter values (0-255) for each color");
    Serial.println("Use # to confirm each value");
    Serial.println("Press * to cancel");
    
    while (true) {
        customKey = customKeypad.getKey();
        
        if (customKey) {
            if (customKey == '*') {
                Serial.println("Cancelled custom color input");
                return;
            }
            
            if (customKey == '#') {
                values[currentColor] = currentValue;
                Serial.print("Set ");
                switch(currentColor) {
                    case 0: Serial.print("Red"); break;
                    case 1: Serial.print("Green"); break;
                    case 2: Serial.print("Blue"); break;
                }
                Serial.print(" to: ");
                Serial.println(currentValue);
                
                currentColor++;
                currentValue = 0;
                
                if (currentColor > 2) {
                    redVal = values[0];
                    greenVal = values[1];
                    blueVal = values[2];
                    
                    analogWrite(redTLED, redVal);
                    analogWrite(greenTLED, greenVal);
                    analogWrite(blueTLED, blueVal);
                    
                    Serial.println("Custom color applied!");
                    return;
                }
                
                Serial.print("Enter value for ");
                switch(currentColor) {
                    case 0: Serial.println("Red:"); break;
                    case 1: Serial.println("Green:"); break;
                    case 2: Serial.println("Blue:"); break;
                }
            }
            else if (customKey >= '0' && customKey <= '9') {
                int digit = customKey - '0';
                currentValue = currentValue * 10 + digit;
                if (currentValue > 255) currentValue = 255;
                Serial.println(currentValue);
            }
        }
    }
}
/*----------------------------------------------------------------------------------------------*/
void setup() {
  pinMode(redTLED, OUTPUT);
  pinMode(greenTLED, OUTPUT);
  pinMode(blueTLED, OUTPUT);

  Serial.begin(9600);
  //Flashes LED as a solid white color so we know it's wired correctly
  standby();
  digitalWrite(redTLED, HIGH);
  digitalWrite(greenTLED, HIGH);
  digitalWrite(blueTLED, HIGH);
}

void loop() {
    static unsigned long keyPressStartTime = 0;
    static bool keyWasPressed = false;
    static bool resetTriggered = false;
    const unsigned long HOLD_DURATION = 2000; // 2 seconds for reset
    const unsigned long CHECK_INTERVAL = 50;  // Check every 50ms

    customKey = customKeypad.getKey();
    unsigned long currentMillis = millis();

    // Handle long press of * for reset
    if (customKey == '*') {
        if (!keyWasPressed) {
            keyWasPressed = true;
            keyPressStartTime = currentMillis;
            resetTriggered = false;
        }
        else if (!resetTriggered && (currentMillis - keyPressStartTime) >= HOLD_DURATION) {
            resetTriggered = true;
            Serial.println("Resetting to standby mode...");
            digitalWrite(redTLED, LOW);
            digitalWrite(greenTLED, LOW);
            digitalWrite(blueTLED, LOW);
            standby();
            keyWasPressed = false;
            return;
        }
    }
    else {
        keyWasPressed = false;
        resetTriggered = false;
    }    // Handle normal key presses
    if (customKey) {
        Serial.print(F("Input: "));
        Serial.println(customKey);
        
        switch(customKey) {
            case 'A':
                Serial.println(F("Static RGB Mode Selected"));
                staticRGB();
                break;
            case 'B':
                Serial.println(F("Random Color Mode Selected"));
                randomColor();
                break;
            case 'C':
                Serial.println(F("Color Cycle Mode Selected"));
                colorCycle();
                break;
            case 'D':
                Serial.println(F("Custom Color Mode Selected"));
                customColor();
                break;
            default:
                if (customKey != '*') { // Ignore * as it's used for reset
                    Serial.println(F("Invalid Input. Please select a mode (A-D)"));
                    errorFlash();
                }
                break;
        }
    }
}
