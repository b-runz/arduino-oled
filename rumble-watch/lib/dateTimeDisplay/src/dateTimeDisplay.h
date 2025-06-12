#ifndef DATE_TIME_DISPLAY_H
#define DATE_TIME_DISPLAY_H

#include "display.h"

struct DateTime
{
  uint8_t day;
  uint8_t month;
  uint8_t year;
};

void initDateTime(uint8_t datetimeDigitPlace);
void dateTimeBottomAndSides(bool withLowerLeft, bool withMiddle, bool withLowerRight);
void dateTimeTopAndSides(bool withUpperLeft, bool withUpperRight);
void printDateTimeOne();
void printDateTimeTwo();
void printDateTimeThree();
void printDateTimeFour();
void printDateTimeFive();
void printDateTimeSix();
void printDateTimeSeven();
void printDateTimeEight();
void printDateTimeNine();
void printDateTimeNaught();
void printDateDigit(uint8_t digit);
void printDate(struct DateTime *datetime);

#endif // DATE_TIME_DISPLAY_H