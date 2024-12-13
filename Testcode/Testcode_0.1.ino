// The first verson, test if the screen can light up
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define SCREEN_ADDRESS 0x3C

#define ledPin 22
#define buttonPin 21

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

  unsigned long reactionTime;
  bool gameStarted = false;
  
void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  digitalWrite(ledPin, LOW);
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.clearDisplay();

  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Press button to start!");
  display.display();
  Serial.println("Successfully inialiazed");
}

void loop() {
  Serial.print(F("gameStarted: "));
  Serial.println(gameStarted ? "true" : "false");
  delay(10000);
  if (!gameStarted) {
    if (digitalRead(buttonPin) == LOW) {
      Serial.println("Button pressed");
      gameStarted = true;
      startCountdown();
      digitalWrite(ledPin, HIGH);
      reactionTime = millis();
    }
  } else {
    if (digitalRead(buttonPin) == LOW) {
      digitalWrite(ledPin, LOW);
      reactionTime = millis() - reactionTime;
      displayReactionTime(reactionTime);
      gameStarted = false;
    }
  }
}

void startCountdown() {
  Serial.println("Strat Countdown");

  display.clearDisplay();
  for (int i = 3; i > 0; i--) {
    display.setCursor(41, 3);
    display.setTextSize(8);
    display.setTextColor(WHITE);
    display.println(i);
    display.display();
    delay(1000);
    display.clearDisplay();
  }
  display.setCursor(20, 4);
  display.setTextSize(8);
  display.setTextColor(WHITE);
  display.println("GO");
  display.display();
  delay(1000);
}

void displayReactionTime(unsigned long time) {
  display.clearDisplay();
  display.setCursor(15, 9);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.print("Time: ");
  display.print(time);
  display.println(" ms");
  display.display();
}