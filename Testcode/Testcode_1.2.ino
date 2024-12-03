// Test code 1.2+
// Finished the sturcture of menu
// Added the function of displayFunction
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define SSD1306_NO_SPLASH 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


// Define push buttons and LEDs pins
const int buttonPins[4] = {D0,D1,D2,D3};
const int ledPins[4] = {D7,D8,D9,D10};

// 菜单相关变量
const char* menuOptions[] = {"1. Play Game", "2. Function"};
int selectedOption = 0;        // 当前选中的菜单选项
const int totalOptions = 2;    // 菜单选项总数
bool inMenu = true;            // 是否在菜单中

// 函数声明
void waitForAnyButtonPress();
void startCountdown();
void playGame();
void displayFunction();
void showMenu();

void setup() {
  // 初始化串口通信，方便调试
  Serial.begin(9600);

  // 初始化按钮为输入模式，并启用内部上拉电阻
  for (int i = 0; i < 4; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }

  // 初始化LED为输出模式，并设置为低电平（熄灭）
  for (int i = 0; i < 4; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }

  // 初始化OLED显示屏
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("SSD1306 allocation failed");
    for (;;); // 如果初始化失败，进入死循环
  }
  display.clearDisplay();
    // Welcome screen
    displayWelcomeScreen();
  // 显示菜单
  showMenu();
}

void loop() {
  // 主程序循环
  if (!inMenu) {
    // 根据选中的选项执行对应的功能
    switch (selectedOption) {
      case 0:
        playGame(); // 执行游戏功能
        break;
      case 1:
        displayFunction(); // 执行占位功能
        break;
      default:
        break;
    }
    // 执行完毕后返回菜单
    inMenu = true;
    showMenu();
  }
}

// 显示菜单并处理导航
void showMenu() {
  while (inMenu) {
    // 清空显示屏
    display.clearDisplay();

    // 设置文本属性
    display.setTextSize(1);
    display.setTextColor(WHITE);

    // 显示每个菜单选项
    for (int i = 0; i < totalOptions; i++) {
      if (i == selectedOption) {
        // 高亮当前选中的选项
        display.fillRect(0, i * 8, 128, 8, WHITE);
        display.setTextColor(BLACK); // 黑色文字，白色背景
      } else {
        display.setTextColor(WHITE); // 白色文字，黑色背景
      }
      display.setCursor(0, i * 8);
      display.println(menuOptions[i]);
    }

    // 更新显示屏
    display.display();

    // 处理按钮输入
    if (digitalRead(buttonPins[1]) == LOW) { // 下一项
      selectedOption = (selectedOption + 1) % totalOptions;
      Serial.println("Menu option changed");
      delay(200); // 消抖延迟
    }
    if (digitalRead(buttonPins[2]) == LOW) { // 上一项
      selectedOption = (selectedOption - 1 + totalOptions) % totalOptions;
      Serial.println("Menu option changed");
      delay(200); // 消抖延迟
    }
    if (digitalRead(buttonPins[0]) == LOW) { // 选择当前选项
      inMenu = false;
      Serial.print("Selected option: ");
      Serial.println(menuOptions[selectedOption]);
      delay(200); // 消抖延迟
    }
  }
}

// 等待任意按钮按下
void waitForAnyButtonPress() {
  bool buttonPressed = false;
  while (!buttonPressed) {
    for (int i = 0; i < 4; i++) {
      if (digitalRead(buttonPins[i]) == LOW) {
        buttonPressed = true;
        delay(200); // 消抖延迟
        break;
      }
    }
  }
}

// 开始游戏前的倒计时
void startCountdown() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  for (int i = 3; i > 0; i--) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print(i);
    display.display();
    delay(1000);
  }
  display.clearDisplay();
  display.display();
}

// 第一个菜单选项的功能：开始游戏
void playGame() {
  Serial.println("Game Started");
  // Show the initial message
  // Need to update decide the better Cursor position
  // Test the best text size
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(30, 0);
  display.println(" Press any");
  display.println(" button to");
  display.setTextSize(2);
  display.setCursor(15,17);
  display.print("PLAY");
  display.display();
  // Wait for any button press to start the game
  waitForAnyButtonPress(); 
  // Start the countdown
  startCountdown();
    unsigned long reactionTimes[4];
    int currentRound = 0;
    while (currentRound < 4) {
    // Select a random LED to light up
    int randomLED = random(0, 4);
    digitalWrite(ledPins[randomLED], HIGH);
    unsigned long startTime = millis();
    while (!digitalRead(buttonPins[randomLED]) == LOW) {
      // Waiting for the correct button press
    }
    unsigned long endTime = millis();
    digitalWrite(ledPins[randomLED], LOW);
    Serial.print("LED ");
    // Record the reaction time
    reactionTimes[currentRound] = endTime - startTime;
    currentRound++;
    delay(500); // Wait 0.5 seconds before lighting up another LED
  } 
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
    display.setCursor(25, 15);
    display.print(averageTime);
    display.println("ms");
    display.display();
  // Wait for any button press to restart the game
    waitForAnyButtonPress();
}


void displayFunction() {
  // 初始化游戏参数
  const int jumpButtonPin = buttonPins[0]; // 跳跃按钮引脚
  bool gameRunning = true;
  int dinoX = 10; // 恐龙的X坐标
  int dinoY = 48; // 恐龙的Y坐标
  int dinoWidth = 10; // 恐龙的宽度
  int dinoHeight = 15; // 恐龙的高度
  int groundLevel = 48; // 地面Y坐标
  bool isJumping = false; // 是否在跳跃
  int jumpSpeed = 0; // 跳跃速度
  int jumpAcceleration = -1; // 跳跃加速度（向上为负）
  int gravity = 1; // 重力加速度
  int maxJumpSpeed = 10; // 最大跳跃速度
  bool canJump = true; // 是否可以跳跃

  unsigned long score = 0; // 分数
  int obstacleSpeed = 2; // 初始障碍物速度
  const int maxObstacleSpeed = 8; // 最大障碍物速度
  unsigned long lastObstacleTime = millis();
  int obstacleGap = 1000; // 初始障碍物间隔（毫秒）

  // 障碍物结构体
  struct Obstacle {
    int x;
    int y;
    int width;
    int height;
  };
  const int maxObstacles = 2; // 同屏最大障碍物数量
  Obstacle obstacles[maxObstacles];

  // 初始化障碍物数组
  for (int i = 0; i < maxObstacles; i++) {
    obstacles[i].x = -1; // 设置为屏幕外
  }

  // 游戏主循环
  while (gameRunning) {
    // 清空显示屏
    display.clearDisplay();

    // 更新分数
    score++;

    // 根据分数增加障碍物速度，设定上限
    if (score % 500 == 0 && obstacleSpeed < maxObstacleSpeed) {
      obstacleSpeed++;
      if (obstacleGap > 500) {
        obstacleGap -= 50; // 减少障碍物间隔，最低500ms
      }
    }

    // 绘制地面
    display.drawLine(0, groundLevel + dinoHeight, 127, groundLevel + dinoHeight, WHITE);

    // 绘制小恐龙（简单的矩形代替）
    display.fillRect(dinoX, dinoY, dinoWidth, dinoHeight, WHITE);

    // 绘制障碍物
    for (int i = 0; i < maxObstacles; i++) {
      if (obstacles[i].x >= 0) {
        display.fillRect(obstacles[i].x, obstacles[i].y, obstacles[i].width, obstacles[i].height, WHITE);
      }
    }

    // 显示分数
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(80, 0);
    display.print("Score: ");
    display.print(score / 10);

    // 更新显示屏
    display.display();

    // 处理跳跃逻辑
    if (digitalRead(jumpButtonPin) == LOW && !isJumping && canJump) {
      isJumping = true;
      jumpSpeed = maxJumpSpeed;
      canJump = false; // 禁止连续跳跃
    }

    if (isJumping) {
      dinoY += jumpSpeed;
      jumpSpeed += jumpAcceleration;
      if (jumpSpeed <= -maxJumpSpeed) {
        jumpSpeed = -maxJumpSpeed;
      }
    } else {
      // 重力作用
      if (dinoY < groundLevel) {
        dinoY += gravity;
        if (dinoY > groundLevel) {
          dinoY = groundLevel;
          canJump = true; // 恢复跳跃能力
        }
      }
    }

    // 防止跳出地面
    if (dinoY > groundLevel) {
      dinoY = groundLevel;
      isJumping = false;
      canJump = true;
    }

    // 生成新障碍物
    if (millis() - lastObstacleTime > obstacleGap) {
      lastObstacleTime = millis();
      for (int i = 0; i < maxObstacles; i++) {
        if (obstacles[i].x < 0) {
          obstacles[i].x = SCREEN_WIDTH;
          obstacles[i].width = 8;
          obstacles[i].height = 15;
          obstacles[i].y = groundLevel + dinoHeight - obstacles[i].height;
          break;
        }
      }
    }

    // 更新障碍物位置
    for (int i = 0; i < maxObstacles; i++) {
      if (obstacles[i].x >= 0) {
        obstacles[i].x -= obstacleSpeed;
        // 检查是否出了屏幕
        if (obstacles[i].x + obstacles[i].width < 0) {
          obstacles[i].x = -1; // 移出屏幕，等待重新使用
        } else {
          // 检查碰撞
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
            break;
          }
        }
      }
    }

    delay(20); // 控制游戏速度
  }

  // 显示游戏结束和分数
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
  delay(3000);

  // 返回菜单
  waitForAnyButtonPress();
}


void displayWelcomeScreen() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(50, 0);
  display.println("TEAM 10");
  display.setTextSize(1);
  display.setCursor(50, 10);
  display.println("GAME!");
  display.setCursor(50, 20);
  display.println("Will start :)");
  display.display();
  delay(3000); // 显示3秒钟
  display.clearDisplay();
  display.display();
}