#include "dateTimeDisplay.h"

void initDateTime(uint8_t datetimeDigitPlace)
{
  uint8_t startColumn = 0x57;
  uint8_t endColumn = 0x59;

  switch (datetimeDigitPlace)
  {
  case 2:
    startColumn = 0x5B;
    endColumn = 0x5D;
    break;

  case 3:
    startColumn = 0x62;
    endColumn = 0x64;
    break;

  case 4:
    startColumn = 0x66;
    endColumn = 0x68;
    break;

  case 5:
    startColumn = 0x6D;
    endColumn = 0x6F;
    break;

  case 6:
    startColumn = 0x71;
    endColumn = 0x73;
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
  SPI.transfer(startColumn);
  SPI.transfer(endColumn);

  // Set page range (1–2)
  SPI.transfer(0x22); // Set Page Address
  SPI.transfer(0x02); // Start page = 2
  SPI.transfer(0x03); // End page = 3

  digitalWrite(OLED_DC, HIGH); // Data mode
}

void dateTimeBottomAndSides(bool withLowerLeft, bool withMiddle, bool withLowerRight)
{
  uint8_t borderMask = 0b11100101;
  uint8_t transferByte = borderMask;
  if (withLowerLeft)
  {
    transferByte = borderMask | 0b00000010;
  }
  SPI.transfer(transferByte);

  transferByte = borderMask;
  if (!withMiddle)
  {
    transferByte = 0b11100100;
  }
  SPI.transfer(transferByte);

  transferByte = borderMask;
  if (withLowerRight)
  {
    transferByte = borderMask | 0b00000010;
  }

  SPI.transfer(transferByte);
}

void dateTimeTopAndSides(bool withUpperLeft, bool withUpperRight)
{
  uint8_t borderMask = 0b01001110;
  uint8_t transferByte = borderMask;
  if (withUpperLeft)
  {
    transferByte = borderMask | 0b10000000;
  }
  SPI.transfer(transferByte);

  transferByte = 0b01001110;
  SPI.transfer(transferByte);

  transferByte = borderMask;
  if (withUpperRight)
  {
    transferByte = borderMask | 0b10000000;
  }

  SPI.transfer(transferByte);
}

void printDateTimeOne()
{
  uint8_t upperBorderMask = 0b00001110;
  for (size_t i = 0; i < 2; i++)
  {
    SPI.transfer(upperBorderMask);
  }
  SPI.transfer(upperBorderMask | 0b11000000);

  uint8_t bottomBorderMask = 0b11100000;
  for (size_t i = 0; i < 2; i++)
  {
    SPI.transfer(bottomBorderMask);
  }
  SPI.transfer(bottomBorderMask | 0b00000111);
}

void printDateTimeTwo()
{
  dateTimeTopAndSides(false, true);
  dateTimeBottomAndSides(true, true, false);
}

void printDateTimeThree()
{
  dateTimeTopAndSides(false, true);
  dateTimeBottomAndSides(false, true, true);
}

void printDateTimeFour()
{
  uint8_t upperBorderMask = 0b00001110;
  uint8_t upperWithSide = upperBorderMask | 0b11000000;

  SPI.transfer(upperWithSide);
  SPI.transfer(upperBorderMask);
  SPI.transfer(upperWithSide);

  uint8_t bottomBorderMask = 0b11100001;
  for (size_t i = 0; i < 2; i++)
  {
    SPI.transfer(bottomBorderMask);
  }
  SPI.transfer(bottomBorderMask | 0b00000111);
}

void printDateTimeFive()
{
  dateTimeTopAndSides(true, false);
  dateTimeBottomAndSides(false, true, true);
}

void printDateTimeSix()
{
  dateTimeTopAndSides(true, false);
  dateTimeBottomAndSides(true, true, true);
}

void printDateTimeSeven()
{
  uint8_t upperBorderMask = 0b01001110;
  for (size_t i = 0; i < 2; i++)
  {
    SPI.transfer(upperBorderMask);
  }
  SPI.transfer(upperBorderMask | 0b11000000);

  uint8_t bottomBorderMask = 0b11100000;
  for (size_t i = 0; i < 2; i++)
  {
    SPI.transfer(bottomBorderMask);
  }
  SPI.transfer(bottomBorderMask | 0b00000111);
}

void printDateTimeEight()
{
  dateTimeTopAndSides(true, true);
  dateTimeBottomAndSides(true, true, true);
}

void printDateTimeNine()
{
  dateTimeTopAndSides(true, true);
  dateTimeBottomAndSides(false, true, true);
}

void printDateTimeNaught()
{
  dateTimeTopAndSides(true, true);
  dateTimeBottomAndSides(true, false, true);
}

void printDateDigit(uint8_t digit)
{
  switch (digit)
  {
  case 0:
    printDateTimeNaught();
    break;
  case 1:
    printDateTimeOne();
    break;
  case 2:
    printDateTimeTwo();
    break;
  case 3:
    printDateTimeThree();
    break;
  case 4:
    printDateTimeFour();
    break;
  case 5:
    printDateTimeFive();
    break;
  case 6:
    printDateTimeSix();
    break;
  case 7:
    printDateTimeSeven();
    break;
  case 8:
    printDateTimeEight();
    break;
  case 9:
    printDateTimeNine();
    break;
  default:
    break;
  }
}

void printDate(struct DateTime *datetime)
{
  uint8_t dayOnesDigit = datetime->day % 10;
  uint8_t dayTensDigit = datetime->day / 10;
  initDateTime(1);
  printDateDigit(dayTensDigit);
  initDateTime(2);
  printDateDigit(dayOnesDigit);

  uint8_t monthOnesDigit = datetime->month % 10;
  uint8_t monthTensDigit = datetime->month / 10;
  initDateTime(3);
  printDateDigit(monthTensDigit);
  initDateTime(4);
  printDateDigit(monthOnesDigit);

  uint8_t yearOnesDigit = datetime->year % 10;
  uint8_t yearTensDigit = datetime->year / 10;
  initDateTime(5);
  printDateDigit(yearTensDigit);
  initDateTime(6);
  printDateDigit(yearOnesDigit);
}