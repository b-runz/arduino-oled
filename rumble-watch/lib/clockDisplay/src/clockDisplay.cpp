#include "clockDisplay.h"

void sideUpper()
{
  for (size_t i = 0; i < 2; i++)
  {
    SPI.transfer(0b01111100);
  }
}

void topAndBottomUpper(bool withBottom)
{
  uint8_t transferByte = withBottom ? 0b10000011 : 0b00000011;
  for (size_t i = 0; i < 5; i++)
  {
    SPI.transfer(transferByte);
  }
}

void sideLower()
{
  for (size_t i = 0; i < 2; i++)
  {
    SPI.transfer(0b00111110);
  }
}

void topAndBottomLower(bool withTop)
{
  uint8_t transferByte = withTop ? 0b11000001 : 0b11000000;
  for (size_t i = 0; i < 5; i++)
  {
    SPI.transfer(transferByte);
  }
}

void blankSide()
{
  for (size_t i = 0; i < 2; i++)
  {
    SPI.transfer(0x00);
  }
}

void printOne()
{
  for (size_t i = 0; i < 7; i++)
  {
    SPI.transfer(0x00);
  }
  sideUpper();
  for (size_t i = 0; i < 7; i++)
  {
    SPI.transfer(0x00);
  }
  sideLower();
}

void printTwo()
{
  blankSide();
  topAndBottomUpper(true);
  sideUpper();
  sideLower();
  topAndBottomLower(true);
  for (size_t i = 0; i < 2; i++)
  {
    SPI.transfer(0x00);
  }
}

void printThree()
{
  blankSide();
  topAndBottomUpper(true);
  sideUpper();
  blankSide();
  topAndBottomLower(true);
  sideLower();
}

void printFour()
{
  sideUpper();
  for (size_t i = 0; i < 5; i++)
  {
    SPI.transfer(0b10000000);
  }
  sideUpper();
  blankSide();
  for (size_t i = 0; i < 5; i++)
  {
    SPI.transfer(0b0000001);
  }
  sideLower();
}

void printFive()
{
  sideUpper();
  topAndBottomUpper(true);
  for (size_t i = 0; i < 4; i++)
  {
    SPI.transfer(0b0000000);
  }
  topAndBottomLower(true);
  sideLower();
}

void printSix()
{
  sideUpper();
  topAndBottomUpper(true);
  blankSide();
  sideLower();
  topAndBottomLower(true);
  sideLower();
}

void printSeven()
{
  blankSide();
  topAndBottomUpper(false);
  sideUpper();
  for (size_t i = 0; i < 7; i++)
  {
    SPI.transfer(0b0000000);
  }
  sideLower();
}

void printEight()
{
  sideUpper();
  topAndBottomUpper(true);
  sideUpper();
  sideLower();
  topAndBottomLower(true);
  sideLower();
}

void printNine()
{
  sideUpper();
  topAndBottomUpper(true);
  sideUpper();
  for (size_t i = 0; i < 2; i++)
  {
    SPI.transfer(0b0000000);
  }
  topAndBottomLower(true);
  sideLower();
}

void printNaught()
{
  sideUpper();
  topAndBottomUpper(false);
  sideUpper();
  sideLower();
  topAndBottomLower(false);
  sideLower();
}

void initDigit(uint8_t digit)
{
  uint8_t columnStart = 0x13;
  uint8_t columnEnd = 0x1B;

  switch (digit)
  {
  case 2:
    columnStart = 0x1F; // Start column = 31
    columnEnd = 0x27;   // End column = 39
    break;

  case 3:
    columnStart = 0x2E; // Start column = 46
    columnEnd = 0x36;   // End column = 54
    break;

  case 4:
    columnStart = 0x3A; // Start column = 58
    columnEnd = 0x42;   // End column = 66
    break;

  default:
    break;
  }

  digitalWrite(OLED_CS, LOW);

  // Set Page Addressing Mode
  digitalWrite(OLED_DC, LOW); // Command mode
  SPI.transfer(0x20);         // Set Memory Addressing Mode
  SPI.transfer(0x00);         // Horizontal Addressing Mode

  // Set column range (19–27)
  SPI.transfer(0x21); // Set Column Address
  SPI.transfer(columnStart);
  SPI.transfer(columnEnd);

  // Set page range (1–2)
  SPI.transfer(0x22); // Set Page Address
  SPI.transfer(0x01); // Start page = 1
  SPI.transfer(0x02); // End page = 2

  // Write data to set pixel at row 0 (COM0)
  digitalWrite(OLED_DC, HIGH); // Data mode
}

void printClockDigit(uint8_t digit)
{
  switch (digit)
  {
  case 0:
    printNaught();
    break;

  case 1:
    printOne();
    break;

  case 2:
    printTwo();
    break;

  case 3:
    printThree();
    break;

  case 4:
    printFour();
    break;

  case 5:
    printFive();
    break;

  case 6:
    printSix();
    break;

  case 7:
    printSeven();
    break;

  case 8:
    printEight();
    break;

  case 9:
    printNine();
    break;

  default:
    break;
  }
}

void printHour(uint8_t hour)
{
  uint8_t onesDigit = hour % 10;
  uint8_t tensDigit = hour / 10;

  initDigit(1);
  printClockDigit(tensDigit);
  initDigit(2);
  printClockDigit(onesDigit);
}

void printMinute(uint8_t minute)
{
  uint8_t onesDigit = minute % 10;
  uint8_t tensDigit = minute / 10;

  initDigit(3);
  printClockDigit(tensDigit);
  initDigit(4);
  printClockDigit(onesDigit);
}

void initSeparator()
{
  digitalWrite(OLED_CS, LOW);

  // Set Page Addressing Mode
  digitalWrite(OLED_DC, LOW); // Command mode
  SPI.transfer(0x20);         // Set Memory Addressing Mode
  SPI.transfer(0x00);         // Horizontal Addressing Mode

  // Set column range (19–27)
  SPI.transfer(0x21); // Set Column Address
  SPI.transfer(0x2A); // Start column = 42
  SPI.transfer(0x2B); // End column = 43

  // Set page range (1–2)
  SPI.transfer(0x22); // Set Page Address
  SPI.transfer(0x01); // Start page = 1
  SPI.transfer(0x02); // End page = 2

  digitalWrite(OLED_DC, HIGH); // Data mode
}

void separatorOn()
{
  SPI.transfer(0b01100000);
  SPI.transfer(0b01100000);
  SPI.transfer(0b000000110);
  SPI.transfer(0b000000110);
}

void separatorOff()
{
  for (size_t i = 0; i < 4; i++)
  {
    SPI.transfer(0x00);
  }
}