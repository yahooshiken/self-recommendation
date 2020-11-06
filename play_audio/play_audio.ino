#include <M5Stack.h>
#include <WiFi.h>
#include "AudioFileSourceSD.h"
#include "AudioFileSourceID3.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"

AudioGeneratorMP3 *mp3;
AudioFileSourceSD *file;
AudioOutputI2S *out;
AudioFileSourceID3 *id3;

void setup()
{
  M5.begin();
  WiFi.mode(WIFI_OFF);
  delay(500);

  M5.Lcd.setTextFont(2);
  M5.Lcd.printf("Sample MP3 playback begins...\n");
  Serial.printf("Sample MP3 playback begins...\n");
}

void playMP3(char *filename)
{
  file = new AudioFileSourceSD(filename);
  id3 = new AudioFileSourceID3(file);
  out = new AudioOutputI2S(0, 0); // Output to builtInDAC
  out->SetPinout(26, 25, 1);
  out->SetOutputModeMono(true);
  out->SetGain(0.1);
  mp3 = new AudioGeneratorMP3();
  mp3->begin(id3, out);
  while (mp3->isRunning())
  {
    if (!mp3->loop())
      mp3->stop();
  }
}

void loop()
{
  M5.update();
  if (M5.BtnA.isPressed())
  {
    playMP3("/Talk1.mp3");
  }
  if (M5.BtnB.isPressed())
  {
    playMP3("/Talk2.mp3");
  }
  if (M5.BtnC.isPressed())
  {
    playMP3("/Talk3.mp3");
  }
  Serial.printf("MP3 done\n");
  delay(10);
}
