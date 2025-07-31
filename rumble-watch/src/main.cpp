#include <display.h>
#include <clockDisplay.h>
#include <alarmIndicator.h>

enum SET_CLOCK_STATE
{
  SHOW_DEFAULT_CLOCK,
  SET_HOUR_ALARM,
  SET_MINUTE_ALARM,
  SET_HOUR_CLOCK,
  SET_MINUTE_CLOCK,
  SET_DATE_DAY,
  SET_DATE_MONTH,
  SET_DATE_YEAR
};

struct LimitContainer {
  uint8_t lower;
  uint8_t higher;
};

const LimitContainer hourLimit = {23, 24};
const LimitContainer minuteLimit = {59, 60};

SET_CLOCK_STATE currentState = SHOW_DEFAULT_CLOCK;

bool ledState = false;
const uint16_t LONG_PRESS_TIME = 1100;

// Flag for interrupt
volatile bool switchPressed = false;
unsigned long lastInterruptTime = 0;

// Improved long press detection with debounce
bool getLongPressSwitchReading()
{

  while (digitalRead(2) == HIGH)
  {
    delay(100);
  }

  // Debounce
  delay(50);
  unsigned long pressStartTime = millis();

  // Check for long press
  while (digitalRead(2) == LOW)
  {
    if (millis() - pressStartTime > LONG_PRESS_TIME)
    {
      // Wait for release to avoid immediate re-trigger
      return true;
    }
  }

  // Short press detected, debounce release
  delay(50);
  return false;
}

uint8_t alarmSetHour = 0;
uint8_t alarmSetMinute = 0;

void readEncoder(int8_t *hourOrMinute, unsigned long* clockTimeOut, const LimitContainer* limitContainer)
{
  uint8_t encoderPinAStartingPos = digitalRead(9);
  uint8_t encoderPinBStartingPos = digitalRead(3);

  while (digitalRead(9) == encoderPinAStartingPos && digitalRead(3) == encoderPinBStartingPos)
  {
    if ((millis() - *clockTimeOut) > 200)
    {
      return;
    }
  }

  // Check if pinA went opposite
  if (digitalRead(9) != encoderPinAStartingPos && digitalRead(3) == encoderPinBStartingPos)
  {
    while (digitalRead(3) == encoderPinBStartingPos)
    {
    }
    delay(10);
    (*hourOrMinute)++;
  }
  else if (digitalRead(3) != encoderPinBStartingPos && digitalRead(9) == encoderPinAStartingPos)
  {
    while (digitalRead(9) == encoderPinAStartingPos)
    {
    }
    delay(10);
    (*hourOrMinute)--;
  }

  if (*hourOrMinute >= limitContainer->higher)
    *hourOrMinute = 0;
  if (*hourOrMinute < 0)
    *hourOrMinute = limitContainer->lower;
}

SET_CLOCK_STATE nextState = SHOW_DEFAULT_CLOCK;

// Interrupt service routine
void handleSwitchInterrupt()
{
  unsigned long currentTime = millis();
  if (currentTime - lastInterruptTime > 50)
  {
    currentState = nextState;
    lastInterruptTime = currentTime;
  }
}

void handleAlarmClockDigits(SET_CLOCK_STATE state, int8_t* hourOrMinute, void (*turnOffFunc)(), void (*printHourOrMinute)(uint8_t), const LimitContainer* limitContainer)
{
  attachInterrupt(digitalPinToInterrupt(2), handleSwitchInterrupt, FALLING);
  unsigned long currentTime = millis();
  bool onOff = false;
  switchPressed = false; // Reset interrupt flag
  initAlarmIndicator();
  alarmIndicatorOn();
  currentState = state;
  while (currentState == state)
  {
    // Encoder part
    readEncoder(hourOrMinute, &currentTime, limitContainer);

    // Blink digit
    if ((millis() - currentTime) > 200)
    {
      currentTime = millis();
      onOff = !onOff;
      if (onOff)
      {
        turnOffFunc();
      }
      else
      {
        printHourOrMinute(*hourOrMinute);
      }
    }
  }
  detachInterrupt(digitalPinToInterrupt(2));
  delay(50);

  initAlarmIndicator();
  alarmIndicatorOff();
}

void setup()
{
  pinMode(9, INPUT);
  pinMode(3, INPUT);
  pinMode(2, INPUT_PULLUP);
  pinMode(10, OUTPUT);

  ssd1306_begin();
  init_ssd1306_buffer();
  ssd1306_display();
  initSeparator();
  separatorOn();

  int8_t minute = 0;
  int8_t hour = 0;

  while (1)
  {
    switch (currentState)
    {
    case SHOW_DEFAULT_CLOCK:
      printHour(hour);
      printMinute(minute);
      while (currentState == SHOW_DEFAULT_CLOCK)
      {
        if (getLongPressSwitchReading())
        {
          currentState = SET_HOUR_ALARM;
          break;
        }
        delay(100); // Reduce CPU load
      }
      break;

    case SET_HOUR_ALARM:
      nextState = SET_MINUTE_ALARM;
      handleAlarmClockDigits(SET_HOUR_ALARM, &hour, turnOffNumberHour, printHour, &hourLimit);
      break;

    case SET_MINUTE_ALARM:
      nextState = SHOW_DEFAULT_CLOCK;
      handleAlarmClockDigits(SET_MINUTE_ALARM, &minute, turnOffNumberMinute, printMinute, &minuteLimit);
      break;

    default:
      break;
    }
  }
}