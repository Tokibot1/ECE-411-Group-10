// Test code 1.3+
// Added a new game mode and improved the menu
// details:
// 1. Added a new game mode: Memory Game
// 2. Improved the menu by adding a border and highlighting the selected option
// 3. Added a welcome screen at the beginning
// 4. Added a countdown before starting the game
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin #
#define SCREEN_ADDRESS 0x3C ///< I2C address
#define SSD1306_NO_SPLASH 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Define push buttons and LEDs pins
const int buttonPins[4] = {D0, D1, D2, D3};
const int ledPins[4] = {D7, D8, D9, D10};

// 菜单相关变量
const char* menuOptions[] = {"1. Reaction Game", "2. Jumping Block", "3. Memory Game"};
int selectedOption = 0;        // 当前选中的菜单选项
const int totalOptions = 3;    // 菜单选项总数
bool inMenu = true;            // 是否在菜单中

// 函数声明
void waitForAnyButtonPress();
void startCountdown();
void reactionGame();
void dinoGame();
void memoryGame();
void displayWelcomeScreen();
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

  // 显示欢迎界面
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
        reactionGame(); // 反应游戏
        break;
      case 1:
        dinoGame(); // 小恐龙游戏
        break;
      case 2:
        memoryGame(); // 记忆力游戏
        break;
      default:
        break;
    }
    // 执行完毕后返回菜单
    inMenu = true;
    showMenu();
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
    display.setCursor(50, 20);
    display.print(i);
    display.display();
    delay(1000);
  }
  display.clearDisplay();
  display.display();
}

// 显示欢迎界面
void displayWelcomeScreen() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(25, 10);
  display.println("WELCOME");
  display.setTextSize(1);
  display.setCursor(30, 40);
  display.println("Press any button");
  display.display();
  waitForAnyButtonPress();
  display.clearDisplay();
}

// 美化菜单并显示
void showMenu() {
  while (inMenu) {
    // 清空显示屏
    display.clearDisplay();

    // 绘制菜单边框
    display.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);

    // 设置文本属性
    display.setTextSize(1);
    display.setTextColor(WHITE);

    // 显示菜单标题
    display.setCursor(30, 2);
    display.println("Select Game");

    // 显示每个菜单选项
    for (int i = 0; i < totalOptions; i++) {
      if (i == selectedOption) {
        // 高亮当前选中的选项
        display.fillRect(10, 15 + i * 15, 108, 12, WHITE);
        display.setTextColor(BLACK); // 黑色文字，白色背景
      } else {
        display.setTextColor(WHITE); // 白色文字，黑色背景
      }
      display.setCursor(15, 17 + i * 15);
      display.println(menuOptions[i]);
    }

    // 更新显示屏
    display.display();

    // 处理按钮输入
    if (digitalRead(buttonPins[1]) == LOW) { // 下一项
      selectedOption = (selectedOption + 1) % totalOptions;
      delay(200); // 消抖延迟
    }
    if (digitalRead(buttonPins[2]) == LOW) { // 上一项
      selectedOption = (selectedOption - 1 + totalOptions) % totalOptions;
      delay(200); // 消抖延迟
    }
    if (digitalRead(buttonPins[0]) == LOW) { // 选择当前选项
      inMenu = false;
      delay(200); // 消抖延迟
    }
  }
}

// 第一个游戏模式：反应游戏
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

  unsigned long reactionTimes[4];
  int currentRound = 0;
  while (currentRound < 4) {
    // 随机等待一段时间
    delay(random(1000, 3000));

    // 随机选择一个LED点亮
    int randomLED = random(0, 4);
    digitalWrite(ledPins[randomLED], HIGH);
    unsigned long startTime = millis();

    // 等待玩家按下对应的按钮
    while (digitalRead(buttonPins[randomLED]) == HIGH) {
      // 等待正确的按钮被按下
    }
    unsigned long endTime = millis();
    digitalWrite(ledPins[randomLED], LOW);

    // 记录反应时间
    reactionTimes[currentRound] = endTime - startTime;
    currentRound++;
    delay(500); // 等待0.5秒再进入下一轮
  }

  // 计算平均反应时间
  unsigned long totalTime = 0;
  for (int i = 0; i < 4; i++) {
    totalTime += reactionTimes[i];
  }
  unsigned long averageTime = totalTime / 4;

  // 显示平均反应时间
  display.clearDisplay();
  display.setCursor(15, 20);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.print("Avg:");
  display.print(averageTime);
  display.println("ms");
  display.display();

  // 等待玩家按下按钮返回菜单
  waitForAnyButtonPress();
}

// 第二个游戏模式：小恐龙游戏
void dinoGame() {
  // 初始化游戏参数
  const int jumpButtonPin = buttonPins[0]; // 跳跃按钮引脚
  bool gameRunning = true;
  int dinoX = 10; // 恐龙的X坐标
  int dinoY = 48; // 恐龙的Y坐标
  int dinoWidth = 10; // 恐龙的宽度
  int dinoHeight = 15; // 恐龙的高度
  int groundLevel = 48; // 地面Y坐标
  bool isJumping = false; // 是否在跳跃
  float jumpVelocity = 0; // 跳跃速度
  float gravity = 0.5; // 重力加速度
  float jumpStrength = -8; // 跳跃初速度

  unsigned long score = 0; // 分数
  int obstacleSpeed = 2; // 初始障碍物速度
  const int maxObstacleSpeed = 6; // 最大障碍物速度
  unsigned long lastObstacleTime = millis();
  int obstacleGap = 1500; // 初始障碍物间隔（毫秒）

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
    if (score % 1000 == 0 && obstacleSpeed < maxObstacleSpeed) {
      obstacleSpeed++;
      if (obstacleGap > 800) {
        obstacleGap -= 100; // 减少障碍物间隔，最低800ms
      }
    }

    // 绘制地面
    display.drawLine(0, groundLevel + dinoHeight, SCREEN_WIDTH, groundLevel + dinoHeight, WHITE);

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

    // 处理跳跃输入
    if (digitalRead(jumpButtonPin) == LOW && !isJumping) {
      isJumping = true;
      jumpVelocity = jumpStrength; // 跳跃初速度
    }

    // 更新恐龙位置
    if (isJumping) {
      dinoY += jumpVelocity;
      jumpVelocity += gravity;

      // 检查是否落地
      if (dinoY >= groundLevel) {
        dinoY = groundLevel;
        isJumping = false;
      }
    }

    // 生成新障碍物
    if (millis() - lastObstacleTime > obstacleGap) {
      lastObstacleTime = millis();
      for (int i = 0; i < maxObstacles; i++) {
        if (obstacles[i].x < 0) {
          obstacles[i].x = SCREEN_WIDTH;
          obstacles[i].width = random(8, 16); // 随机宽度
          obstacles[i].height = random(10, 20); // 随机高度
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

// 第三个游戏模式：记忆力游戏
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

  const int sequenceLength = 5;
  int sequence[sequenceLength];
  bool gameRunning = true;

  // 生成随机序列
  for (int i = 0; i < sequenceLength; i++) {
    sequence[i] = random(0, 4);
  }

  // 游戏主循环
  for (int round = 1; round <= sequenceLength && gameRunning; round++) {
    // 显示当前轮数
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(35, 0);
    display.print("Round ");
    display.println(round);
    display.display();
    delay(1000);

    // 播放序列
    for (int i = 0; i < round; i++) {
      digitalWrite(ledPins[sequence[i]], HIGH);
      delay(500);
      digitalWrite(ledPins[sequence[i]], LOW);
      delay(250);
    }

    // 接收玩家输入
    for (int i = 0; i < round; i++) {
      bool buttonPressed = false;
      while (!buttonPressed) {
        for (int j = 0; j < 4; j++) {
          if (digitalRead(buttonPins[j]) == LOW) {
            buttonPressed = true;
            if (j != sequence[i]) {
              gameRunning = false;
            }
            delay(200); // 消抖
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

  // 显示游戏结束或胜利
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  if (gameRunning) {
    display.setCursor(10, 20);
    display.println("You Win!");
  } else {
    display.setCursor(5, 20);
    display.println("Game Over");
  }
  display.display();
  delay(3000);

  // 返回菜单
  waitForAnyButtonPress();
}