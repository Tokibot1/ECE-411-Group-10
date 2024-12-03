#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <algorithm>

#define SCREEN_WIDTH 128   // OLED display width, in pixels
#define SCREEN_HEIGHT 64   // OLED display height, in pixels
#define OLED_RESET    -1   // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C // I2C address for the OLED

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Define push buttons and LEDs pins
const int buttonPins[4] = {21, 19, 18, 5}; // Adjust these pins as per your hardware setup
const int ledPins[4] = {22, 23, 25, 26};   // Adjust these pins as per your hardware setup
const int buzzerPin = 27;                   // Pin connected to the buzzer

// Game variables
bool inMenu = true;
int selectedMode = 0;
bool gameRunning = false;
bool competitionMode = false;
unsigned long reactionTimes[4];
int sequence[100];
int sequenceLength = 0;
int currentStep = 0;
unsigned long interval = 2000; // Initial interval time in milliseconds
unsigned long lastTime = 0;
bool playerTurn = false; // false for Player 1, true for Player 2

void setup() {
  Serial.begin(115200);

  // Initialize pins
  for (int i = 0; i < 4; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);

  // Initialize display
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("SSD1306 allocation failed");
    for (;;);
  }
  Serial.println("OLED initialized");

  display.clearDisplay();
  showMenu();
}

void loop() {
  if (inMenu) {
    handleMenuInput();
  } else {
    if (competitionMode) {
      competitionModeGame();
    } else {
      reactionTimeGame();
    }
  }
  // Check for exit combination (e.g., pressing first and last buttons together)
  if (digitalRead(buttonPins[0]) == LOW && digitalRead(buttonPins[3]) == LOW) {
    Serial.println("Exiting to menu");
    resetGame();
    inMenu = true;
    showMenu();
    delay(500); // Debounce delay
  }
}

void handleMenuInput() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);

  display.setCursor(0, 0);
  display.println("Select Mode:");
  display.setCursor(0, 20);
  if (selectedMode == 0) {
    display.println("> Reaction");
    display.println("  Competition");
  } else {
    display.println("  Reaction");
    display.println("> Competition");
  }
  display.display();

  // Assign buttons to menu navigation
  if (digitalRead(buttonPins[1]) == LOW) { // Next option
    selectedMode = (selectedMode + 1) % 2;
    Serial.println("Menu option changed");
    delay(300); // Debounce delay
  }
  if (digitalRead(buttonPins[2]) == LOW) { // Previous option
    selectedMode = (selectedMode + 1) % 2;
    Serial.println("Menu option changed");
    delay(300); // Debounce delay
  }
  if (digitalRead(buttonPins[0]) == LOW) { // Select option
    inMenu = false;
    gameRunning = true;
    competitionMode = (selectedMode == 1);
    Serial.print("Selected mode: ");
    Serial.println(competitionMode ? "Competition" : "Reaction");
    display.clearDisplay();
    delay(300); // Debounce delay
  }
}

void reactionTimeGame() {
  static int currentRound = 0;
  if (gameRunning) {
    // Light up a random LED
    int randomLED = random(0, 4);
    digitalWrite(ledPins[randomLED], HIGH);
    Serial.print("LED ");
    Serial.print(randomLED);
    Serial.println(" ON");

    // Play beep sound
    tone(buzzerPin, 1000, 200);

    unsigned long startTime = millis();
    bool buttonPressed = false;

    while (!buttonPressed && millis() - startTime < 5000) { // Wait max 5 seconds
      for (int i = 0; i < 4; i++) {
        if (digitalRead(buttonPins[i]) == LOW) {
          Serial.print("Button ");
          Serial.print(i);
          Serial.println(" pressed");
          // Play success or failure tone
          if (i == randomLED) {
            tone(buzzerPin, 2000, 200); // Correct button
            Serial.println("Correct button");
          } else {
            tone(buzzerPin, 500, 200); // Incorrect button
            Serial.println("Incorrect button");
          }
          reactionTimes[currentRound] = millis() - startTime;
          buttonPressed = true;
          break;
        }
      }
    }
    digitalWrite(ledPins[randomLED], LOW);
    Serial.print("LED ");
    Serial.print(randomLED);
    Serial.println(" OFF");

    currentRound++;
    if (currentRound >= 4) {
      // Display average reaction time
      unsigned long totalTime = 0;
      for (int i = 0; i < 4; i++) {
        totalTime += reactionTimes[i];
      }
      unsigned long averageTime = totalTime / 4;

      display.clearDisplay();
      display.setTextSize(2);
      display.setCursor(0, 0);
      display.print("Avg Time:");
      display.setCursor(0, 20);
      display.print(averageTime);
      display.print(" ms");
      display.display();
      Serial.print("Average reaction time: ");
      Serial.print(averageTime);
      Serial.println(" ms");

      gameRunning = false;
      currentRound = 0;
    }
    delay(500); // Short delay before next round
  }
}

void competitionModeGame() {
  if (gameRunning) {
    if (!playerTurn) {
      // Player 1's turn
      if (currentStep == 0) {
        Serial.println("Player 1's turn");
        sequenceLength = 0;
        interval = 2000;
      }
      unsigned long currentTime = millis();
      if (currentTime - lastTime >= interval) {
        int randomLED = random(0, 4);
        sequence[sequenceLength] = randomLED;
        sequenceLength++;
        digitalWrite(ledPins[randomLED], HIGH);
        Serial.print("LED ");
        Serial.print(randomLED);
        Serial.println(" ON");
        tone(buzzerPin, 1000, 200);
        delay(500);
        digitalWrite(ledPins[randomLED], LOW);
        Serial.print("LED ");
        Serial.print(randomLED);
        Serial.println(" OFF");
        lastTime = currentTime;
        if (interval > 600) {
          interval -= 100;
        } else {
          interval = 500;
        }
        currentStep++;
      }
      // Check for failure condition (e.g., pressing any button)
      for (int i = 0; i < 4; i++) {
        if (digitalRead(buttonPins[i]) == LOW) {
          Serial.println("Player 1 failed");
          tone(buzzerPin, 500, 500);
          displayScore(currentStep - 1);
          playerTurn = true;
          currentStep = 0;
          lastTime = millis();
          delay(1000);
          break;
        }
      }
    } else {
      // Player 2's turn
      Serial.println("Player 2's turn");
      for (int i = 0; i < sequenceLength; i++) {
        // Light up LED
        digitalWrite(ledPins[sequence[i]], HIGH);
        tone(buzzerPin, 1000, 200);
        delay(500);
        digitalWrite(ledPins[sequence[i]], LOW);
        delay(200);
        // Wait for button press
        bool correctButton = false;
        while (!correctButton) {
          for (int j = 0; j < 4; j++) {
            if (digitalRead(buttonPins[j]) == LOW) {
              if (j == sequence[i]) {
                Serial.print("Player 2 pressed correct button: ");
                Serial.println(j);
                tone(buzzerPin, 2000, 200);
                correctButton = true;
              } else {
                Serial.print("Player 2 pressed wrong button: ");
                Serial.println(j);
                tone(buzzerPin, 500, 500);
                displayLost();
                gameRunning = false;
                return;
              }
              while (digitalRead(buttonPins[j]) == LOW); // Wait for button release
            }
          }
        }
      }
      // Player 2 succeeded
      Serial.println("Player 2 succeeded");
      playVictorySong();
      gameRunning = false;
    }
  } else {
    // Wait for any button to restart the game
    for (int i = 0; i < 4; i++) {
      if (digitalRead(buttonPins[i]) == LOW) {
        Serial.println("Restarting game");
        resetGame();
        delay(500); // Debounce delay
        break;
      }
    }
  }
}

void displayScore(int score) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print("Player 1");
  display.setCursor(0, 20);
  display.print("Score:");
  display.print(score);
  display.display();
  Serial.print("Player 1 score: ");
  Serial.println(score);
}

void displayLost() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print("Player 2");
  display.setCursor(0, 20);
  display.print("Lost!");
  display.display();
  Serial.println("Player 2 lost");
}

void playVictorySong() {
  int melody[] = {262, 294, 330, 349, 392, 440, 494, 523};
  int noteDurations[] = {4, 4, 4, 4, 4, 4, 4, 4};

  for (int thisNote = 0; thisNote < 8; thisNote++) {
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(buzzerPin, melody[thisNote], noteDuration);
    delay(noteDuration * 1.3);
  }
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print("Player 2");
  display.setCursor(0, 20);
  display.print("Wins!");
  display.display();
  Serial.println("Player 2 wins");
}

void resetGame() {
  gameRunning = true;
  competitionMode = (selectedMode == 1);
  playerTurn = false;
  currentStep = 0;
  lastTime = millis();
  for (int i = 0; i < 4; i++) {
    digitalWrite(ledPins[i], LOW);
  }
  display.clearDisplay();
}

void showMenu() {
  selectedMode = 0;
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Select Mode:");
  display.println("> Reaction");
  display.println("  Competition");
  display.display();
}