#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ===== BOTÕES =====
#define BTN_UP    32
#define BTN_DOWN  33

// ===== BUZZER =====
#define BUZZER_PIN 27
#define BUZZER_CH  0

// ===== OLED =====
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDR 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);pp

// ===== JOGO =====
int paddleY = 24;
int ballX = 64;
int ballY = 32;
int ballVX = -1;
int ballVY = 1;
int score = 0;

// ===== FUNÇÕES DE SOM =====
void beep(int freq, int timeMs) {
  ledcWriteTone(BUZZER_CH, freq);
  delay(timeMs);
  ledcWriteTone(BUZZER_CH, 0);
}

void resetBall() {
  ballX = 64;
  ballY = 32;
  ballVX = (random(0, 2) == 0) ? -1 : 1;
  ballVY = (random(0, 2) == 0) ? -1 : 1;
}

void setup() {
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);

  // PWM do buzzer
  ledcSetup(BUZZER_CH, 2000, 8);
  ledcAttachPin(BUZZER_PIN, BUZZER_CH);

  Wire.begin(21, 22);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    while (true);
  }

  display.clearDisplay();
  display.display();

  randomSeed(micros());
}

void loop() {
  // ===== INPUT =====
  if (!digitalRead(BTN_UP)) paddleY -= 2;
  if (!digitalRead(BTN_DOWN)) paddleY += 2;

  if (paddleY < 0) paddleY = 0;
  if (paddleY > 48) paddleY = 48;

  // ===== BOLA =====
  ballX += ballVX;
  ballY += ballVY;

  // colisão topo/baixo
  if (ballY <= 0 || ballY >= 63) {
    ballVY = -ballVY;
    beep(1500, 20);
  }

  // colisão com paddle
  if (ballX <= 6 && ballY >= paddleY && ballY <= paddleY + 16) {
    ballVX = 1;
    score++;
    beep(2000, 30);
  }

  // perdeu
  if (ballX < 0) {
    score = 0;
    beep(400, 200);
    resetBall();
  }

  // CPU rebate
  if (ballX > 127) {
    ballVX = -1;
    beep(1200, 20);
  }

  // ===== DESENHO =====
  display.clearDisplay();

  display.fillRect(2, paddleY, 3, 16, SSD1306_WHITE);   // paddle
  display.fillRect(ballX, ballY, 2, 2, SSD1306_WHITE); // bola

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(54, 0);
  display.print(score);

  display.display();
  delay(20);
}
