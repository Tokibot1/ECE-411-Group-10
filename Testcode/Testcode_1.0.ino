//Functional test code for the reaction time game
//11/10/2020
//ECE 411 Group 10
//Vesion 1.0 - Xiang
//Note: this code work for the basic functionality of the reaction time game
//Things to update: 
//(MAY)Menu
//(MUST)Another game mode
//(MUST)Add a buzzer function

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define SSD1306_NO_SPLASH 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


// Define push buttons and LEDs pins
const int buttonPins[4] = {D0,D1,D2,D3};
const int ledPins[4] = {D7,D8,D9,D10};


unsigned long reactionTimes[4];
int currentRound = 0;


void setup() {
  Serial.begin(9600);
  // Initialize buttons and LEDs
  for (int i = 0; i < 4; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }


  // Initialize the OLED display
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.display();
  delay(2000);
  display.clearDisplay();


  // Show the initial message
  // Need to update decide the better Cursor position
  // Test the best text size
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println(" Press any");
  display.println(" button to");
  display.setTextSize(2);
  display.setCursor(28, 38);
  display.print("PLAY");


  display.display();


  // Wait for any button press to start the game
  waitForAnyButtonPress();
  
  // Start the countdown
  startCountdown();
}


void loop() {
  if (currentRound < 4) {
    // Select a random LED to light up
    int randomLED = random(0, 4);
    digitalWrite(ledPins[randomLED], HIGH);


    unsigned long startTime = millis();
    while (!digitalRead(buttonPins[randomLED]) == LOW) {
      // Waiting for the correct button press
    }
    unsigned long endTime = millis();
    digitalWrite(ledPins[randomLED], LOW);


    // Record the reaction time
    reactionTimes[currentRound] = endTime - startTime;
    currentRound++;
    
    delay(500); // Wait 0.5 seconds before lighting up another LED
  } else {
    // Calculate and display the average reaction time
    unsigned long totalTime = 0;
    for (int i = 0; i < 4; i++) {
      totalTime += reactionTimes[i];
    }
    unsigned long averageTime = totalTime / 4;


    display.clearDisplay();
    display.setCursor(15, 9);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.print("Avg.Time");
    display.setTextSize(1);
    display.setCursor(15, 15);
    display.print(averageTime);
    display.println("ms");
    display.display();


    // Wait for any button press to restart the game
    waitForAnyButtonPress();
    currentRound = 0;  // Reset the game
    startCountdown();
  }
}

// Wait for any button press
// If checked for button pressed, it will easily "go over" this function
void waitForAnyButtonPress() {
  bool buttonPressed = false;
  while (!buttonPressed) {
    for (int i = 0; i < 4; i++) {
      if (digitalRead(buttonPins[i]) == LOW) {
        buttonPressed = true;
        break;
      }
    }
  }
}

// Making it shows 3, 2, 1, GO
// Use this for a mode of showing something on the screen
void startCountdown() {
  display.clearDisplay();
  for (int i = 3; i > 0; i--) {
    display.setCursor(41, 3);
    display.setTextSize(3);
    display.setTextColor(WHITE);
    display.println(i);
    display.display();
    delay(1000);
    display.clearDisplay();
  }
  display.setCursor(20, 4);
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.println("GO");
  display.display();
  delay(1000);
  display.clearDisplay();
}
