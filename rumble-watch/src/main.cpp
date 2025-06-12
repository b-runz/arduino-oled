#include <display.h>
#include <dateTimeDisplay.h>

void setup()
{
  unsigned long startMillis;
  unsigned long currentMillis;
  const unsigned long period = 1000;

  pinMode(9, INPUT);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  ssd1306_begin();

  init_ssd1306_buffer();
  ssd1306_display();

  DateTime datetime = {
    8,
    6,
    25
  };

  printDate(&datetime);

  // ssd1306_display();

  // memset(ssd1306_buffer, 0xff, 512);
  // ssd1306_display();

  bool separatorToggle = false;

  while (1)
  {
    while (digitalRead(9) == 0)
    {
      // if(separatorToggle){
      //   alarmIndicatorOff();
      //   separatorToggle = true;
      // } else {
      //   alarmIndicatorOn();
      //   separatorToggle = false;
      // }
      // delay(100);
    }
    delay(200);
  }
}
