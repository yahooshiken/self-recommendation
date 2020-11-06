#include <M5Stack.h>
#include <Wire.h>
#include "AudioFileSourceSD.h"
#include "AudioFileSourceID3.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"

#define SERVO_ADDR 0x53

//TFT_eSPI liblary
TFT_eSPI tft = TFT_eSPI();
//thread config
TaskHandle_t threadTaskHandle;
//timer interrupt variable.
volatile unsigned long usecCount = 0;
hw_timer_t *interruptTimer = NULL;
portMUX_TYPE mutex = portMUX_INITIALIZER_UNLOCKED;

int indicate = 0;
//timer start/stop check variable
int startCheck = 0;

void greetThread(void *pvParameters)
{
  while (1)
  {
    vTaskDelay(1);

    if (indicate == 1)
    {
      greet();
      indicate = 2;
    }
    if (indicate == 2)
    {
      greet();
      indicate = 3;
    }
    if (indicate == 3)
    {
      greet();
    }
    delay(600);

    if (indicate == 4)
    {
      greet();
    }
    if (indicate == 5)
    {
      greet();
      indicate = 0;
    }
  }
}
void setup()
{
  M5.begin();
  Wire.begin(21, 22, 100000);
  Serial.begin(115200);

  interruptTimer = timerBegin(0, 80, true);
  timerAlarmWrite(interruptTimer, 5, true);
  timerAlarmDisable(interruptTimer);
  xTaskCreatePinnedToCore(greetThread, "threadTaskHandle", 8192, NULL, 3, &threadTaskHandle, 0);

  setupLcd();
}

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

void loop()
{
  M5.update();
  if (M5.BtnA.isPressed() && startCheck == 0)
  {
    indicate = 1;
    playMP3("/Talk1.mp3");
  }
  if (indicate == 2)
  {
    playMP3("/Talk2.mp3");
    delay(1100);
  }
  else if (indicate == 3)
  {
    playMP3("/Talk3.mp3");
    delay(700);
  }
  else if (indicate == 4)
  {
    playMP3("/Talk4.mp3");
    delay(400);
  }
  if (M5.BtnB.isPressed() && startCheck == 0)
  {
    indicate = 5;
    delay(550);
    playMP3("/Talk5.mp3");
    delay(15000);
  }
}

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
  out->SetGain(0.05);
  mp3 = new AudioGeneratorMP3();
  mp3->begin(id3, out);
  while (mp3->isRunning())
  {
    if (!mp3->loop())
      mp3->stop();
  }
}

//Timer count
void IRAM_ATTR usecTimer()
{
  portENTER_CRITICAL_ISR(&mutex);
  usecCount += 5;
  portEXIT_CRITICAL_ISR(&mutex);
}
