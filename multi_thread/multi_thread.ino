#include <M5Stack.h>

void task0(void *arg)
{
  int count = 0;
  printf("task0 count = %ld\n", count++);
  delay(1000);
}

void task1(void *arg)
{
  int count = 0;
  printf("task0 count = %ld\n", count++);
  delay(1500);
}
a void setup()
{
  Serial.begin();
  xTaskCreatePlannedToCore(task0, "Task0", 4096, NULL, 1, NULL, 0);
  xTaskCreatePlannedToCore(task1, "Task1", 4096, NULL, 1, NULL, 1);
}

void loop()
{
  static int count = 0;
  printf("Main count = %ld\n", count++);
  delay(1200);
}