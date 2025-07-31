#ifndef CLOCK_DISPLAY_H
#define CLOCK_DISPLAY_H

#include "display.h"

void sideUpper();
void topAndBottomUpper(bool withBottom);
void sideLower();
void topAndBottomLower(bool withTop);
void blankSide();
void printOne();
void printTwo();
void printThree();
void printFour();
void printFive();
void printSix();
void printSeven();
void printEight();
void printNine();
void printNaught();
void initDigit(uint8_t digit);
void printClockDigit(uint8_t digit);
void printHour(uint8_t hour);
void printMinute(uint8_t minute);
void initSeparator();
void separatorOn();
void separatorOff();
void turnOffNumberMinute();
void turnOffNumberHour();

#endif // CLOCK_DISPLAY_H