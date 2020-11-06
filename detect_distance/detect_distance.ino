#include <M5Stack.h>

#define DISTANCE 35

void setup()
{
  M5.begin();
  pinMode(DISTANCE, INPUT);
  M5.Lcd.setTextSize(10);
}

void loop()
{
  M5.Lcd.fillScreen(BLACK);

  int distance = (int)pulseIn(DISTANCE, HIGH);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.print(distance);

  delay(1000);
}