// Game Collection 3.0
// Improved Snake Game with better button responsiveness and scoring
// Enhanced settings menu
// Added sound and light effects to other game modes
// All 3.0 codes has the screen address of 0x3C
// Notes
// Change the text size to have better display

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin #
#define SCREEN_ADDRESS 0x3C // I2C address
#define SSD1306_NO_SPLASH 

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int buzzerPin = D6; // Buzzer connected to D6

// Define push buttons and LEDs pins
const int buttonPins[4] = {D0, D1, D2, D3};
const int ledPins[4] = {D7, D8, D9, D10};

// Menu variables
const char* menuOptions[] = {"1. Reaction Game", "2. Jumping Dino", "3. Memory Game", "4. Snake Game", "5. Settings"};
int selectedOption = 0;        // Currently selected menu option
const int totalOptions = 5;    // Total number of menu options
bool inMenu = true;            // Whether in menu
int menuOffset = 0;            // Menu scroll offset

// Game settings
int reactionGameRounds = 4;
int memoryGameRounds = 5;
int snakeGameSpeed = 200; // Delay in milliseconds
int dinoGameDifficulty = 1; // Difficulty level

// Function declarations
void waitForAnyButtonPress();
void startCountdown();
void reactionGame();
void dinoGame();
void memoryGame();
void snakeGame();
void settingsMenu();
void adjustSetting(int settingIndex);
void displayWelcomeScreen();
void showMenu();

// Variables for button state
unsigned long lastButtonPress[4] = {0, 0, 0, 0};
const unsigned long debounceDelay = 100; // Debounce delay in milliseconds

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(9600);

  // Initialize buttons as input with internal pull-up resistors
  for (int i = 0; i < 4; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }

  // Initialize LEDs as output and turn them off
  for (int i = 0; i < 4; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }

  // Initialize buzzer
  pinMode(buzzerPin, OUTPUT);

  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("SSD1306 allocation failed");
    for (;;); // Loop forever if initialization fails
  }
  display.clearDisplay();

  // Display welcome screen
  displayWelcomeScreen();

  // Show menu
  showMenu();
}

void loop() {
  // Main program loop
  if (!inMenu) {
    // Execute the selected option
    switch (selectedOption) {
      case 0:
        reactionGame(); // Reaction Game
        break;
      case 1:
        dinoGame(); // Jumping Dino Game
        break;
      case 2:
        memoryGame(); // Memory Game
        break;
      case 3:
        snakeGame(); // Snake Game
        break;
      case 4:
        settingsMenu(); // Settings
        break;
      default:
        break;
    }
    // Return to menu after game ends
    inMenu = true;
    showMenu();
  }
}

// Wait for any button to be pressed
void waitForAnyButtonPress() {
  bool buttonPressed = false;
  while (!buttonPressed) {
    for (int i = 0; i < 4; i++) {
      if (digitalRead(buttonPins[i]) == LOW) {
        buttonPressed = true;
        delay(200); // Debounce delay
        break;
      }
    }
  }
}

// Read button with debounce
bool readButton(int buttonIndex) {
  bool buttonState = LOW;
  if (digitalRead(buttonPins[buttonIndex]) == LOW) {
    unsigned long currentTime = millis();
    if (currentTime - lastButtonPress[buttonIndex] > debounceDelay) {
      lastButtonPress[buttonIndex] = currentTime;
      buttonState = HIGH;
    }
  }
  return buttonState;
}

// Countdown before starting game
void startCountdown() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  for (int i = 3; i > 0; i--) {
    display.clearDisplay();
    display.setCursor(50, 20);
    display.print(i);
    display.display();
    tone(buzzerPin, 1000, 200); // Play tone
    delay(1000);
  }
  display.clearDisplay();
  display.display();
}

// Display welcome screen
void displayWelcomeScreen() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(25, 10);
  display.println("WELCOME");
  display.setTextSize(1);
  display.setCursor(20, 40);
  display.println("Press any button");
  display.display();
  waitForAnyButtonPress();
  display.clearDisplay();
}

// Display menu with scrolling support
void showMenu() {
  while (inMenu) {
    // Clear display
    display.clearDisplay();

    // Draw menu border
    display.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);

    // Set text properties
    display.setTextSize(1);
    display.setTextColor(WHITE);

    // Display menu title
    display.setCursor(30, 2);
    display.println("Select Game");

    // Scroll menu if needed
    if (selectedOption < menuOffset) {
      menuOffset = selectedOption;
    } else if (selectedOption >= menuOffset + 4) {
      menuOffset = selectedOption - 3;
    }

    // Display menu options
    for (int i = menuOffset; i < min(menuOffset + 4, totalOptions); i++) {
      if (i == selectedOption) {
        // Highlight selected option
        display.fillRect(10, 15 + (i - menuOffset) * 12, 108, 12, WHITE);
        display.setTextColor(BLACK);
      } else {
        display.setTextColor(WHITE);
      }
      display.setCursor(15, 17 + (i - menuOffset) * 12);
      display.println(menuOptions[i]);
    }

    // Update display
    display.display();

    // Handle button input
    if (readButton(1)) { // Next option
      selectedOption = (selectedOption + 1) % totalOptions;
    }
    if (readButton(2)) { // Previous option
      selectedOption = (selectedOption - 1 + totalOptions) % totalOptions;
    }
    if (readButton(0)) { // Select current option
      inMenu = false;
    }
  }
}

// Reaction Game
void reactionGame() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(30, 0);
  display.println("Press any");
  display.println("button to");
  display.setTextSize(2);
  display.setCursor(25, 20);
  display.print("START");
  display.display();
  waitForAnyButtonPress();

  startCountdown();

  unsigned long reactionTimes[reactionGameRounds];
  int currentRound = 0;
  while (currentRound < reactionGameRounds) {
    // Random wait time
    delay(random(1000, 3000));

    // Randomly select an LED to light up
    int randomLED = random(0, 4);
    digitalWrite(ledPins[randomLED], HIGH);
    tone(buzzerPin, 2000, 100); // Play tone
    unsigned long startTime = millis();

    // Wait for the player to press the corresponding button
    bool correctButtonPressed = false;
    while (!correctButtonPressed) {
      if (digitalRead(buttonPins[randomLED]) == LOW) {
        correctButtonPressed = true;
        tone(buzzerPin, 1000, 100); // Correct button sound
        digitalWrite(ledPins[randomLED], LOW);
        unsigned long endTime = millis();
        reactionTimes[currentRound] = endTime - startTime;
        currentRound++;
        delay(500); // Wait before next round
      } else {
        // Check for wrong button press
        for (int i = 0; i < 4; i++) {
          if (i != randomLED && digitalRead(buttonPins[i]) == LOW) {
            tone(buzzerPin, 500, 200); // Wrong button sound
            digitalWrite(ledPins[randomLED], LOW);
            // Penalty time
            delay(1000);
            correctButtonPressed = true;
            break;
          }
        }
      }
    }
  }

  // Calculate average reaction time
  unsigned long totalTime = 0;
  for (int i = 0; i < reactionGameRounds; i++) {
    totalTime += reactionTimes[i];
  }
  unsigned long averageTime = totalTime / reactionGameRounds;

  // Display average reaction time
  display.clearDisplay();
  display.setCursor(15, 20);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.print("Avg:");
  display.print(averageTime);
  display.println("ms");
  display.display();
  tone(buzzerPin, 1500, 500); // Play tone

  // Wait for player to press any button to return to menu
  waitForAnyButtonPress();
}

// Jumping Dino Game
void dinoGame() {
  // Initialize game parameters
  const int jumpButtonPin = buttonPins[0]; // Jump button pin
  bool gameRunning = true;
  int dinoX = 10; // Dino X coordinate
  int dinoY = 48; // Dino Y coordinate
  int dinoWidth = 10; // Dino width
  int dinoHeight = 15; // Dino height
  int groundLevel = 48; // Ground Y coordinate
  bool isJumping = false; // Is dino jumping
  float jumpVelocity = 0; // Jump velocity
  float gravity = 0.5 * dinoGameDifficulty; // Gravity acceleration
  float jumpStrength = -8; // Jump initial velocity

  unsigned long score = 0; // Score
  int obstacleSpeed = 2 * dinoGameDifficulty; // Obstacle speed
  const int maxObstacleSpeed = 6 * dinoGameDifficulty; // Max obstacle speed
  unsigned long lastObstacleTime = millis();
  int obstacleGap = 1500 / dinoGameDifficulty; // Obstacle gap in milliseconds

  // Obstacle structure
  struct Obstacle {
    int x;
    int y;
    int width;
    int height;
  };
  const int maxObstacles = 2; // Max obstacles on screen
  Obstacle obstacles[maxObstacles];
  // Initialize obstacles
  for (int i = 0; i < maxObstacles; i++) {
    obstacles[i].x = -1; // Set off-screen
  }

  // Game loop
  while (gameRunning) {
    // Clear display
    display.clearDisplay();

    // Update score
    score++;

    // Increase obstacle speed based on score
    if (score % 1000 == 0 && obstacleSpeed < maxObstacleSpeed) {
      obstacleSpeed++;
      if (obstacleGap > 800 / dinoGameDifficulty) {
        obstacleGap -= 100; // Decrease obstacle gap
      }
    }

    // Draw ground
    display.drawLine(0, groundLevel + dinoHeight, SCREEN_WIDTH, groundLevel + dinoHeight, WHITE);

    // Draw dino (simple rectangle)
    display.fillRect(dinoX, dinoY, dinoWidth, dinoHeight, WHITE);

    // Draw obstacles
    for (int i = 0; i < maxObstacles; i++) {
      if (obstacles[i].x >= 0) {
        display.fillRect(obstacles[i].x, obstacles[i].y, obstacles[i].width, obstacles[i].height, WHITE);
      }
    }

    // Display score
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(80, 0);
    display.print("Score: ");
    display.print(score / 10);

    // Update display
    display.display();

    // Handle jump input
    if (digitalRead(jumpButtonPin) == LOW && !isJumping) {
      isJumping = true;
      jumpVelocity = jumpStrength; // Jump initial velocity
      tone(buzzerPin, 1000, 100); // Play jump sound
      digitalWrite(ledPins[0], HIGH); // Turn on LED to indicate jump
    }

    // Update dino position
    if (isJumping) {
      dinoY += jumpVelocity;
      jumpVelocity += gravity;

      // Check if landed
      if (dinoY >= groundLevel) {
        dinoY = groundLevel;
        isJumping = false;
        digitalWrite(ledPins[0], LOW); // Turn off LED when landed
      }
    }

    // Generate new obstacles
    if (millis() - lastObstacleTime > obstacleGap) {
      lastObstacleTime = millis();
      for (int i = 0; i < maxObstacles; i++) {
        if (obstacles[i].x < 0) {
          obstacles[i].x = SCREEN_WIDTH;
          obstacles[i].width = random(8, 16); // Random width
          obstacles[i].height = random(10, 20); // Random height
          obstacles[i].y = groundLevel + dinoHeight - obstacles[i].height;
          break;
        }
      }
    }

    // Update obstacles
    for (int i = 0; i < maxObstacles; i++) {
      if (obstacles[i].x >= 0) {
        obstacles[i].x -= obstacleSpeed;
        // Check if off-screen
        if (obstacles[i].x + obstacles[i].width < 0) {
          obstacles[i].x = -1; // Reset obstacle
        } else {
          // Check collision
          int dinoLeft = dinoX;
          int dinoRight = dinoLeft + dinoWidth;
          int dinoTop = dinoY;
          int dinoBottom = dinoTop + dinoHeight;

          int obstacleLeft = obstacles[i].x;
          int obstacleRight = obstacleLeft + obstacles[i].width;
          int obstacleTop = obstacles[i].y;
          int obstacleBottom = obstacleTop + obstacles[i].height;

          bool collision = !(dinoRight < obstacleLeft || dinoLeft > obstacleRight || dinoBottom < obstacleTop || dinoTop > obstacleBottom);
          if (collision) {
            gameRunning = false;
            tone(buzzerPin, 500, 500); // Play collision sound
            digitalWrite(ledPins[1], HIGH); // Turn on LED to indicate collision
            break;
          }
        }
      }
    }

    delay(20); // Control game speed
  }

  // Display game over and score
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(15, 10);
  display.println("Game Over");
  display.setTextSize(1);
  display.setCursor(25, 40);
  display.print("Score: ");
  display.print(score / 10);
  display.display();
  tone(buzzerPin, 500, 500); // Play game over sound
  digitalWrite(ledPins[1], LOW); // Turn off collision LED
  delay(3000);

  // Return to menu
  waitForAnyButtonPress();
}

// Memory Game
void memoryGame() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(20, 0);
  display.println("Memory Game");
  display.setCursor(10, 20);
  display.println("Follow the sequence");
  display.display();
  waitForAnyButtonPress();

  const int sequenceLength = memoryGameRounds;
  int sequence[sequenceLength];
  bool gameRunning = true;

  // Generate random sequence
  for (int i = 0; i < sequenceLength; i++) {
    sequence[i] = random(0, 4);
  }

  // Game loop
  for (int round = 1; round <= sequenceLength && gameRunning; round++) {
    // Display current round
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(35, 0);
    display.print("Round ");
    display.println(round);
    display.display();
    delay(1000);

    // Play sequence
    for (int i = 0; i < round; i++) {
      digitalWrite(ledPins[sequence[i]], HIGH);
      tone(buzzerPin, 1000 + sequence[i]*200, 300); // Play different tones
      delay(500);
      digitalWrite(ledPins[sequence[i]], LOW);
      delay(250);
    }

    // Get player input
    for (int i = 0; i < round; i++) {
      bool buttonPressed = false;
      while (!buttonPressed) {
        for (int j = 0; j < 4; j++) {
          if (digitalRead(buttonPins[j]) == LOW) {
            buttonPressed = true;
            tone(buzzerPin, 1000 + j*200, 100); // Play tone
            if (j != sequence[i]) {
              gameRunning = false;
              tone(buzzerPin, 500, 500); // Wrong answer sound
              digitalWrite(ledPins[j], HIGH); // Indicate wrong button
              delay(500);
              digitalWrite(ledPins[j], LOW);
            } else {
              digitalWrite(ledPins[j], HIGH); // Indicate correct button
              delay(200);
              digitalWrite(ledPins[j], LOW);
            }
            delay(200); // Debounce
            break;
          }
        }
      }
      if (!gameRunning) {
        break;
      }
    }
    delay(500);
  }

  // Display game over or win
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  if (gameRunning) {
    display.setCursor(10, 20);
    display.println("You Win!");
    tone(buzzerPin, 1500, 500); // Play win sound
  } else {
    display.setCursor(5, 20);
    display.println("Game Over");
    // tone played during wrong answer
  }
  display.display();
  delay(3000);

  // Return to menu
  waitForAnyButtonPress();
}

// Snake Game
void snakeGame() {
  // Initialize game parameters
  const int snakeMaxLength = 50;
  int snakeX[snakeMaxLength];
  int snakeY[snakeMaxLength];
  int snakeLength = 3;
  int direction = 0; // 0:Right, 1:Down, 2:Left, 3:Up
  int foodX = random(0, SCREEN_WIDTH / 4) * 4;
  int foodY = random(0, SCREEN_HEIGHT / 4) * 4;
  unsigned long score = 0;

  // Initialize snake position
  for (int i = 0; i < snakeLength; i++) {
    snakeX[i] = 64 - i * 4;
    snakeY[i] = 32;
  }

  bool gameRunning = true;

  // Game loop
  while (gameRunning) {
    // Clear display
    display.clearDisplay();

    // Draw food
    display.fillRect(foodX, foodY, 4, 4, WHITE);

    // Draw snake
    for (int i = 0; i < snakeLength; i++) {
      display.fillRect(snakeX[i], snakeY[i], 4, 4, WHITE);
    }

    // Display score
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.print("Score: ");
    display.print(score);

    // Update display
    display.display();

    // Handle input
    if (readButton(0)) { // Turn left
      direction = (direction + 3) % 4;
    }
    if (readButton(1)) { // Turn right
      direction = (direction + 1) % 4;
    }

    // Move snake
    for (int i = snakeLength - 1; i > 0; i--) {
      snakeX[i] = snakeX[i - 1];
      snakeY[i] = snakeY[i - 1];
    }
    switch (direction) {
      case 0:
        snakeX[0] += 4;
        break;
      case 1:
        snakeY[0] += 4;
        break;
      case 2:
        snakeX[0] -= 4;
        break;
      case 3:
        snakeY[0] -= 4;
        break;
    }

    // Check collision with walls
    if (snakeX[0] < 0 || snakeX[0] >= SCREEN_WIDTH || snakeY[0] < 0 || snakeY[0] >= SCREEN_HEIGHT) {
      gameRunning = false;
      tone(buzzerPin, 500, 500); // Play game over sound
      break;
    }

    // Check collision with itself
    for (int i = 1; i < snakeLength; i++) {
      if (snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i]) {
        gameRunning = false;
        tone(buzzerPin, 500, 500); // Play game over sound
        break;
      }
    }

    // Check if food eaten
    if (snakeX[0] == foodX && snakeY[0] == foodY) {
      if (snakeLength < snakeMaxLength) {
        snakeLength++;
        tone(buzzerPin, 1500, 100); // Play eat sound
        digitalWrite(ledPins[2], HIGH); // Indicate food eaten
        delay(100);
        digitalWrite(ledPins[2], LOW);
      }
      // Generate new food position
      foodX = random(0, SCREEN_WIDTH / 4) * 4;
      foodY = random(0, SCREEN_HEIGHT / 4) * 4;
      score += 10;
    }

    delay(snakeGameSpeed); // Control game speed
  }

  // Display game over
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(15, 10);
  display.println("Game Over");
  display.setTextSize(1);
  display.setCursor(25, 40);
  display.print("Score: ");
  display.print(score);
  display.display();
  delay(3000);

  // Return to menu
  waitForAnyButtonPress();
}

// Settings Menu
void settingsMenu() {
  const char* settingsOptions[] = {"Reaction Rounds", "Memory Rounds", "Snake Speed", "Dino Difficulty", "Back"};
  int totalSettingsOptions = 5;
  int selectedSetting = 0;
  int settingsOffset = 0;
  bool inSettings = true;

  while (inSettings) {
    // Clear display
    display.clearDisplay();

    // Draw menu border
    display.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);

    // Set text properties
    display.setTextSize(1);
    display.setTextColor(WHITE);

    // Display menu title
    display.setCursor(30, 2);
    display.println("Settings");

    // Scroll menu if needed
    if (selectedSetting < settingsOffset) {
      settingsOffset = selectedSetting;
    } else if (selectedSetting >= settingsOffset + 4) {
      settingsOffset = selectedSetting - 3;
    }

    // Display settings options
    for (int i = settingsOffset; i < min(settingsOffset + 4, totalSettingsOptions); i++) {
      if (i == selectedSetting) {
        // Highlight selected option
        display.fillRect(10, 15 + (i - settingsOffset) * 12, 108, 12, WHITE);
        display.setTextColor(BLACK);
      } else {
        display.setTextColor(WHITE);
      }
      display.setCursor(15, 17 + (i - settingsOffset) * 12);
      display.println(settingsOptions[i]);
    }

    // Update display
    display.display();

    // Handle button input
    if (readButton(1)) { // Next option
      selectedSetting = (selectedSetting + 1) % totalSettingsOptions;
    }
    if (readButton(2)) { // Previous option
      selectedSetting = (selectedSetting - 1 + totalSettingsOptions) % totalSettingsOptions;
    }
    if (readButton(0)) { // Select current option
      if (selectedSetting == totalSettingsOptions - 1) {
        // Back to main menu
        inSettings = false;
      } else {
        // Adjust setting
        adjustSetting(selectedSetting);
      }
    }
  }
}

// Adjust specific setting
void adjustSetting(int settingIndex) {
  int* settingValue;
  int minValue, maxValue, step;
  const char* settingName;

  switch (settingIndex) {
    case 0:
      settingValue = &reactionGameRounds;
      settingName = "Reaction Rounds";
      minValue = 1;
      maxValue = 10;
      step = 1;
      break;
    case 1:
      settingValue = &memoryGameRounds;
      settingName = "Memory Rounds";
      minValue = 1;
      maxValue = 20;
      step = 1;
      break;
    case 2:
      settingValue = &snakeGameSpeed;
      settingName = "Snake Speed";
      minValue = 50;
      maxValue = 500;
      step = 10;
      break;
    case 3:
      settingValue = &dinoGameDifficulty;
      settingName = "Dino Difficulty";
      minValue = 1;
      maxValue = 5;
      step = 1;
      break;
    default:
      return;
  }

  bool adjusting = true;
  while (adjusting) {
    // Clear display
    display.clearDisplay();

    // Display setting name and value
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(10, 20);
    display.println(settingName);
    display.setTextSize(2);
    display.setCursor(30, 40);
    display.print(*settingValue);

    // Update display
    display.display();

    // Handle button input
    if (readButton(1)) { // Increase
      if (settingIndex == 2) {
        *settingValue -= step; // For snake speed, decreasing value increases speed
      } else {
        *settingValue += step;
      }
      if (*settingValue > maxValue) *settingValue = maxValue;
      if (*settingValue < minValue) *settingValue = minValue;
      tone(buzzerPin, 1200, 100); // Feedback sound
    }
    if (readButton(2)) { // Decrease
      if (settingIndex == 2) {
        *settingValue += step;
      } else {
        *settingValue -= step;
      }
      if (*settingValue > maxValue) *settingValue = maxValue;
      if (*settingValue < minValue) *settingValue = minValue;
      tone(buzzerPin, 800, 100); // Feedback sound
    }
    if (readButton(0)) { // Confirm
      adjusting = false;
      tone(buzzerPin, 1000, 200); // Confirmation sound
    }
  }
}