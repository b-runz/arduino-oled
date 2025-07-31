#include <display.h>
#include <clockDisplay.h>

void setup()
{
  pinMode(9, INPUT);
  pinMode(3, INPUT);

  ssd1306_begin();

  init_ssd1306_buffer();
  ssd1306_display();

  initSeparator();
  separatorOn();
  printHour(0);
  int8_t minute = 0;

  uint8_t encoderPinAStartingPos = digitalRead(9);
  uint8_t encoderPinBStartingPos = digitalRead(3);

  while (1)
  {
    printMinute(minute);
    while (digitalRead(9) == encoderPinAStartingPos && digitalRead(3) == encoderPinBStartingPos)
    {
    }

    // Check if pinA went oppsite
    if (digitalRead(9) != encoderPinAStartingPos && digitalRead(3) == encoderPinBStartingPos)
    {
      // Wait for the other pin to go opposite
      while (digitalRead(3) == encoderPinBStartingPos)
      {
      }
      delay(10);

      minute++;
      encoderPinAStartingPos = digitalRead(9);
      encoderPinBStartingPos = digitalRead(3);

    } else if (digitalRead(3) != encoderPinBStartingPos && digitalRead(9) == encoderPinAStartingPos)
    {
      // Wait for the other pin to go opposite
      while (digitalRead(9) == encoderPinAStartingPos)
      {
      }
      
      delay(10);

      minute--;
      encoderPinAStartingPos = digitalRead(9);
      encoderPinBStartingPos = digitalRead(3);

    }

    if (minute == 60)
    {
      minute = 0;
    }
    if (minute < 0)
    {
      minute = 59;
    }
  }
}
