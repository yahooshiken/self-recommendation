#include <Arduino.h>
#include <M5Stack.h>
#include <Wire.h>

#define SERVO_ADDR 0x53
#define TRIG 17
#define ECHO 5

void setup()
{
  M5.begin(true, false, true);
  M5.Lcd.setTextFont(4);
  M5.Lcd.setCursor(70, 100);
  M5.Lcd.print("Servo Example");
  Wire.begin(21, 22, 100000);
  setupEcho();
}

void setupEcho()
{
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
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

int detect()
{
  int time, distance;
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  time = pulseIn(ECHO, HIGH);
  distance = (int)(time * 0.017);

  return distance;
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

void loop()
{
  int d = detect();

  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.print(d);
  if (d < 20)
  {
    greet();
  }
}