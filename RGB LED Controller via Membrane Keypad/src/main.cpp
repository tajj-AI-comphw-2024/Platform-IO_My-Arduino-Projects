/*
 * File: main.cpp
 * Author: Troy Johnson
 * Date Created: April 26, 2025
 * Description: This program controls an RGB LED using a membrane keypad. 
 *              It includes various modes such as static RGB, brightness adjustment, 
 *              and flashing patterns.
 * 
 * Key Features:
 * - Static RGB Mode: Select predefined colors using the keypad.
 * - Brightness Adjustment: Increase or decrease LED brightness.
 * - Flashing Patterns: Cycle through LED colors at fixed intervals.
 * - Custom Modes: Define custom RGB values for unique colors.
 * 
 * Future Plans:
 * - Implement a memory feature to save the last selected mode.
 * - Simplify the code for better readability and maintainability.
 * - Reset function to restore the LED to its default state (Standby), when the user presses and holds the #/* key.
 * - Grant user the ability to choose using their keyboard or keypad if they dont have a microcontoller.
 * - LED will flash red when input is invalid.
 * 
 * Last Updated: May 1, 2025
 */

/*
 * Change Log:
 * - April 26, 2025: Initial version created. Documnetation planned and completed.
 * - April 27, 2025: Implemented staticRGB() and standby() functions.
 * - April 28, 2025: Added brightness adjustment menu.
 * - May 1, 2025: Improved staticFlash() to use non-blocking timing.
 */

#include <Arduino.h>
#include <Keypad.h>

// RGB LED Pins & Values
#define redTLED 13
#define greenTLED 12
#define blueTLED 11
int redVal, greenVal, blueVal;



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

/*----------------------------------------------------------------------------------------------*/
void fadeLEDs(bool fadeIn) {
  if (fadeIn) {
    for (int i = 0; i <= 255; i++) {
      customKey = customKeypad.getKey();
      analogWrite(redTLED, i);
      analogWrite(greenTLED, i);
      analogWrite(blueTLED, i);
      delay(10);
      if (customKey != NO_KEY) {
        Serial.println("Standby Cancelled");
        break;
      }
    }
  } else {
    for (int i = 255; i >= 0; i--) {
      customKey = customKeypad.getKey();
      analogWrite(redTLED, i);
      analogWrite(greenTLED, i);
      analogWrite(blueTLED, i);
      delay(10);
      if (customKey != NO_KEY) {
        Serial.println("Standby Cancelled");
        break;
      }
    }
  }
}

void standby() {
  for (int i = 3; i > 0; i--) {
    digitalWrite(redTLED, HIGH);
    digitalWrite(greenTLED, HIGH);
    digitalWrite(blueTLED, HIGH);
    delay(250);
    digitalWrite(redTLED, LOW);
    digitalWrite(greenTLED, LOW);
    digitalWrite(blueTLED, LOW);
    delay(250);
  }

  delay(1000);

  while (customKey == NO_KEY) {
    fadeLEDs(true);  // Fade in
    fadeLEDs(false); // Fade out
  }
}
/*----------------------------------------------------------------------------------------------*/

void brightness(char key){
  Serial.println("Brightness Key Pressed");
  Serial.println("Press 1. to Increase Brightness");
  Serial.println("Press 2. to Decrease Brightness");
  Serial.println("Press 3. to Exit Brightness Menu");
  Serial.println("Clicking Modes (A,B,C, or D) will also allow you leave the menu and return to the main menu");
}
/*----------------------------------------------------------------------------------------------*/
void staticFlash(){
  unsigned long previousMillis = 0; // Tracks the last time the LED state was updated
  const unsigned long interval = 1000; // Interval for each LED state in milliseconds
  int state = 0; // Tracks the current LED state

  while (true) {
    customKey = customKeypad.getKey();
    if (customKey != NO_KEY) {
      // Exit the function if a key is pressed
      Serial.println("Exiting Static Flash Mode");
      break;
    }

    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;

      // Cycle through LED states
      switch (state) {
        case 0: // Red LED
          digitalWrite(redTLED, HIGH);
          digitalWrite(greenTLED, LOW);
          digitalWrite(blueTLED, LOW);
          break;
        case 1: // Green LED
          digitalWrite(redTLED, LOW);
          digitalWrite(greenTLED, HIGH);
          digitalWrite(blueTLED, LOW);
          break;
        case 2: // Blue LED
          digitalWrite(redTLED, LOW);
          digitalWrite(greenTLED, LOW);
          digitalWrite(blueTLED, HIGH);
          break;
        case 3: // Yellow LED
          digitalWrite(redTLED, HIGH);
          digitalWrite(greenTLED, HIGH);
          digitalWrite(blueTLED, LOW);
          break;
        case 4: // Cyan LED
          digitalWrite(redTLED, LOW);
          digitalWrite(greenTLED, HIGH);
          digitalWrite(blueTLED, HIGH);
          break;
        case 5: // Magenta LED
          digitalWrite(redTLED, HIGH);
          digitalWrite(greenTLED, LOW);
          digitalWrite(blueTLED, HIGH);
          break;
        case 6: // White LED
          digitalWrite(redTLED, HIGH);
          digitalWrite(greenTLED, HIGH);
          digitalWrite(blueTLED, HIGH);
          break;
      }

      // Move to the next state
      state = (state + 1) % 7; // Cycle through 0 to 6
    }
  }
}

void staticRGB() {
  while (true) {
    customKey = customKeypad.getKey();
    if(customKey == 'A') {
      // Future notes: Will print in the form of a grid to represent the keypad layout
      Serial.println("Static RGB Key Pressed\nMenu Options:");
      Serial.println("If you want to know the prompts, press A again");
      Serial.println("Press 1. to Red LED On");
      Serial.println("Press 2. to Green LED On");
      Serial.println("Press 3. to Blue LED On");
      Serial.println("Press 4. to Yellow LED On");
      Serial.println("Press 5. to Cyan LED On");
      Serial.println("Press 6. to Magenta LED On");
      Serial.println("Press 7. to White LED On");
      Serial.println("Press 8. to LED Off");
      Serial.println("Press 9. to Static Flash Mode");
      Serial.println("Press 0. to Brightness Menu");
      Serial.println("Press Any Mode to Exit Static RGB Menu");
    }else if (customKey == '1') {
      Serial.println("Red LED On");
      digitalWrite(redTLED, HIGH);
      digitalWrite(greenTLED, LOW);
      digitalWrite(blueTLED, LOW);
    } else if (customKey == '2') {
      Serial.println("Green LED On");
      digitalWrite(redTLED, LOW);
      digitalWrite(greenTLED, HIGH);
      digitalWrite(blueTLED, LOW);
    } else if (customKey == '3') {
      Serial.println("Blue LED On");
      digitalWrite(redTLED, LOW);
      digitalWrite(greenTLED, LOW);
      digitalWrite(blueTLED, HIGH);
    } else if (customKey == '4') {
      Serial.println("Yellow LED On");
      digitalWrite(redTLED, HIGH);
      digitalWrite(greenTLED, HIGH);
      digitalWrite(blueTLED, LOW);
    } else if (customKey == '5') {
      Serial.println("Cyan LED On");
      digitalWrite(redTLED, LOW);
      digitalWrite(greenTLED, HIGH);
      digitalWrite(blueTLED, HIGH);
    } else if (customKey == '6') {
      Serial.println("Magenta LED On");
      digitalWrite(redTLED, HIGH);
      digitalWrite(greenTLED, LOW);
      digitalWrite(blueTLED, HIGH);
    } else if (customKey == '7') {
      Serial.println("White LED On");
      digitalWrite(redTLED, HIGH);
      digitalWrite(greenTLED, HIGH);
      digitalWrite(blueTLED, HIGH);
    } else if (customKey == '8') {
      Serial.println("LED Off");
      digitalWrite(redTLED, LOW);
      digitalWrite(greenTLED, LOW);
      digitalWrite(blueTLED, LOW);
    }else if (customKey == '9') {
      staticFlash();
      Serial.println("Back to Static RGB Mode");
    }else if(customKey == '0'){
      brightness(customKey);
    }
    customKey = NO_KEY; // Reset key to avoid repeated actions
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
  customKey = customKeypad.getKey();

  if (customKey) {
    Serial.print("Input: ");
    Serial.println(customKey);
  }
  if (customKey == 'A') {
    staticRGB();
  }
}

