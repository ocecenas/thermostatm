
///Appendix C
#ifndef ClockBasics_H
#define ClockBasics_H
#include <LiquidCrystal.h>
LiquidCrystal LcdDriver(11, 9, 5, 6, 7, 8);
// Variable used as clock settings.
int Hours, Minutes, Seconds;
// This function is to be called every second
// to update the clock represented by the
// global variables Hours, Minutes, Seconds
void UpdateClock()
{
  // Check if Seconds not at wrap point.
  if (Seconds < 59 & Seconds >= 0)
  {
    Seconds++; // Move seconds ahead.
  }
  else
  {
    Seconds = 0; // Reset Seconds
    // and check Minutes for wrap.
    if (Minutes < 59 & Minutes >= 0)
    {
      Minutes++; // Move seconds ahead.
    }
    else
    {
      Minutes = 0; // Reset Minutes
      // check Hours for wrap
      if (Hours < 23 & Hours >= 0)
      {
        Hours++;// Move Hours ahead.
      }
      else
      {
        Hours = 0;// Reset Hours
      }// End of Hours test.
    } // End of Minutes test
  } // End of Seconds test
} // end of UpdateClock()
void SendClock()
{
  // Check if leading zero needs to be sent
  if (Hours < 10)
  {
    LcdDriver.print("0");
  }
  LcdDriver.print(Hours); // Then send hours

  LcdDriver.print(":"); // And separator
  // Check for leading zero on Minutes.
  if (Minutes < 10)
  {
    LcdDriver.print("0");
  }
  LcdDriver.print(Minutes); // Then send Minutes
  LcdDriver.print(":"); // And separator
  // Check for leading zero needed for Seconds.
  if (Seconds < 10)
  {
    LcdDriver.print("0");
  }
  LcdDriver.print(Seconds); // Then send Seconds
  // with new line
} // End of SendClock()
// States for setting clock.
enum ClockStates
{
  CLOCK_RUNNING, CLOCK_SET_HOURS,
  CLOCK_SET_MINUTES, CLOCK_SET_SECONDS
};
ClockStates clockState = CLOCK_RUNNING;
// Function that processes incoming characters to set the clock.
void SettingClock(char Input)
{
  // interpret input based on state
  switch (clockState)
  {
    case CLOCK_RUNNING:
      if (Input == 'S')
      {
        clockState = CLOCK_SET_HOURS;
        Hours = 0; // Reset clock values.
        Minutes = 0;
        Seconds = 0;
      }
      break;
    case CLOCK_SET_HOURS: //
      if (Input >= '0' && Input <= '9')
        Hours = 10 * (Hours % 10) + Input - '0';
      else if (Input == ':')
        clockState = CLOCK_SET_MINUTES;
      else if (Input == 'R')
        clockState = CLOCK_RUNNING;
      break;
    case CLOCK_SET_MINUTES: //
      if (Input >= '0' && Input <= '9')

        Minutes = 10 * (Minutes % 10) + Input - '0';
      else if (Input == ':')
        clockState = CLOCK_SET_SECONDS;
      else if (Input == 'R')
        clockState = CLOCK_RUNNING;
      break;
    case CLOCK_SET_SECONDS: //
      if (Input >= '0' && Input <= '9')
        Seconds = 10 * (Seconds % 10) + Input - '0';
      else if (Input == 'R')
        clockState = CLOCK_RUNNING;
      break;
  }// End of clock mode switch.
} // End of SettingClock
#endif