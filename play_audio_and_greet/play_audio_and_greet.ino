#include <M5Stack.h>
#include <Wire.h>
#include "AudioFileSourceSD.h"
#include "AudioFileSourceID3.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"

#define SERVO_ADDR 0x53
#define TRIG 17
#define ECHO 5
#define DISTANCE 35

//TFT_eSPI liblary
TFT_eSPI tft = TFT_eSPI();
//thread config
TaskHandle_t greetThreadTaskHandle;
TaskHandle_t playAudioThreadTaskHandle;

bool isPicked = false; // 手に取ったか.
bool isCloser = false; // 近づいたか.

// メインセットアップ関数.
void setup()
{
  M5.begin();
  Wire.begin(21, 22, 100000);
  Serial.begin(115200);

  setupLcd();
  setupEcho();
  setupDistance();
  createTasks();
}

// 超音波センサの初期化をする.
void setupEcho()
{
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
}

// 距離センサの初期化をする.
void setupDistance()
{
  pinMode(DISTANCE, INPUT);
}

// LCD の初期化をする.
void setupLcd()
{
  M5.Lcd.setTextColor(YELLOW);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(65, 10);
  M5.Lcd.println("SnackPrototype");
  M5.Lcd.setCursor(3, 35);
  M5.Lcd.println("Press buttonA");
  M5.Lcd.println("Start it!.");
}

// 並行スレッドを起動する.
void createTasks()
{
  xTaskCreatePinnedToCore(greetThread, "greetThreadTaskHandle", 4096, NULL, 1, &greetThreadTaskHandle, 0);
  xTaskCreatePinnedToCore(playAudioThread, "playAudioThreadTaskHandle", 4096, NULL, 1, &playAudioThreadTaskHandle, 0);
}

void greetThread(void *pvParameters)
{
  while (1)
  {
    vTaskDelay(1);
    if (isPicked)
    {
      greet();
    }
    else if (isCloser)
    {
      greet();
    }
  }
}

void playAudioThread(void *pvParameters)
{
  while (1)
  {
    vTaskDelay(1);
    if (isPicked)
    {
      playMP3("/Talk1.mp3");
    }
    else if (isCloser)
    {
      playMP3("/Talk2.mp3");
    }
    isCloser = false;
    isPicked = false;
  }
}

// メインループ関数.
void loop()
{
  M5.update();

  int height = detectPicking();
  isPicked = height < 20;

  int distance = detectGettingCloser();
  isCloser = distance > 1300;

  delay(50);
}

// ランダムな動作を生成する.
void greet()
{
  static int BASE_LINE = 100;
  static int MAX = 15;
  static int MIN = 10;

  static int MAX_SPEED = 150;
  static int MIN_SPEED = 120;

  int loopNum = random(2, 9);

  for (int i = 0; i < loopNum; i++)
  {
    int addition = random(MIN, MAX);
    writeServoAngle(0, BASE_LINE);
    delay(random(MIN_SPEED, MAX_SPEED));
    writeServoAngle(0, BASE_LINE + addition);
    delay(random(MIN_SPEED, MAX_SPEED));
  }

  writeServoAngle(0, BASE_LINE);
  delay(random(300, 800));

  if (random(3) == 0)
  {

    writeServoUs(2, 1600);
    delay(1000);
    writeServoUs(2, 2000);
    delay(1000);
  }

  writeServoAngle(0, BASE_LINE);
  delay(random(300, 800));
}

// addr 0x01 means "control the number 1 servo by us"
void writeServoUs(uint8_t number, uint16_t us)
{
  Wire.beginTransmission(SERVO_ADDR);
  Wire.write(0x00 | number);
  Wire.write(us & 0x00ff);
  Wire.write(us >> 8 & 0x00ff);
  Wire.endTransmission();
}

// addr 0x11 means "control the number 1 servo by angle"
void writeServoAngle(uint8_t number, uint8_t angle)
{
  Wire.beginTransmission(SERVO_ADDR);
  Wire.write(0x10 | number);
  Wire.write(angle);
  Wire.endTransmission();
}

// mp3 形式の音声ファイルを再生する.
void playMP3(char *filename)
{
  AudioFileSourceSD *file;
  AudioFileSourceID3 *id3;
  AudioOutputI2S *out;
  AudioGeneratorMP3 *mp3;

  file = new AudioFileSourceSD(filename);
  id3 = new AudioFileSourceID3(file);
  out = new AudioOutputI2S(0, 0); // Output to builtInDAC
  out->SetPinout(26, 25, 1);
  out->SetOutputModeMono(true);
  out->SetGain(0.5);
  mp3 = new AudioGeneratorMP3();
  mp3->begin(id3, out);
  while (mp3->isRunning())
  {
    if (!mp3->loop())
      mp3->stop();
  }
}
// 距離センサで近づきを検知する.
int detectGettingCloser()
{
  int distance = analogRead(DISTANCE);
  return distance;
}

// 超音波センサで手に取ったかを検知する.
int detectPicking()
{
  int time, height;

  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  time = pulseIn(ECHO, HIGH);
  height = (int)(time * 0.017);

  return height;
}
