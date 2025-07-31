#include "alarmIndicator.h"

void initAlarmIndicator()
{
  digitalWrite(OLED_CS, LOW);

  // Set Page Addressing Mode
  digitalWrite(OLED_DC, LOW); // Command mode
  SPI.transfer(0x20);         // Set Memory Addressing Mode
  SPI.transfer(0x00);         // Horizontal Addressing Mode

  // Set column range (19–27)
  SPI.transfer(0x21); // Set Column Address
  SPI.transfer(0x5C); // Start column = 92
  SPI.transfer(0x6F); // End column = 111

  // Set page range (1–2)
  SPI.transfer(0x22); // Set Page Address
  SPI.transfer(0x01); // Start page = 1
  SPI.transfer(0x01); // End page = 1

  digitalWrite(OLED_DC, HIGH); // Data mode
}

void alarmIndicatorOn()
{
    const uint8_t spiData[] = {
      0b00001110, 0b00000101, 0b00001110, 0b00000000,            // Sequence 1
      0b00001111, 0b00001000, 0b00000000,                        // Sequence 2
      0b00001110, 0b00000101, 0b00001110, 0b00000000,            // Sequence 3
      0b00001111, 0b00000101, 0b00001010, 0b00000000,            // Sequence 4
      0b00001110, 0b00000001, 0b00000110, 0b00000001, 0b00001110 // Sequence 5
  };
  for (size_t i = 0; i < sizeof(spiData); i++)
  {
    SPI.transfer(spiData[i]);
  }
}

void alarmIndicatorOff()
{
  for (size_t i = 0; i < 20; i++)
  {
    SPI.transfer(0x00);
  }
}