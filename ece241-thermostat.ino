#include "ClockBasics.h" //clock code
#include "ButtonDebounce.h" //button code
#include "EncoderMonitor.h" //coder code
#include <MsTimer2.h> //timerlibrary
#include<EEPROM.h> //eeprom library
#include<LiquidCrystal.h> //lcd library
// ClockTimer
#define CLOCK_INTERVAL 1000 //variable with 1000 ms
unsigned long ClockTimer; //define clock timer
unsigned long BlinkTimer; //blink timer
unsigned long TempTimer; // temperature timer
////////////START EEPROM//////////////////////
// Variable that are to be updated.
// Declared as globals.
union flt_byte
{
  float F;
  unsigned char B [4];
}
LowTemp, HighTemp;
int WrtPointer = -1;
// indicates portion to be written, -1 being no writes.
// State Variable and its setup.
enum EEPWrtStates { NoEEPWrt, EEPWrtWait, EEPWrt };
volatile EEPWrtStates WrtState = NoEEPWrt;
#define EEP_WRITE_INTERVAL 1250
volatile int EEPWrtCount = EEP_WRITE_INTERVAL;
// Called if a change was made in the paramters.
void ResetWrt()
{
  WrtState = EEPWrtWait; // Move to wait state.
  EEPWrtCount = EEP_WRITE_INTERVAL;
  PORTB |= 0x20; // set pin 13 high indicating
  // a wait to write to EEPROM
} // end of ResetWrt
// Four millisecond ISR and State machine controller
void WrtNextStep()
{
  switch (WrtState)
  {
    case NoEEPWrt: // Nothing happening.
      break;
    case EEPWrtWait:
      // Waiting for 5 seconds to write.
      if (EEPWrtCount) // check if time out has occurred.
      {
        EEPWrtCount--; // Count down to time out
      }
      else // Time out has occurred
      {
        // So move to write state.
        WrtState = EEPWrt; // Go back to Write.
        WrtPointer = 9;
        PORTB &= ~0x20;// Set 13 low, end of wait.
      } // end of write timer.
      break;
    case EEPWrt: // writing.
      WriteOutState(); // Write next data to EEPROM.
      if (WrtPointer == -1) // If we are done
        WrtState = NoEEPWrt; // Writing.
      break;
  } // end of switch
} // end of WrtNextStep
#define lowTempUpdate 0
#define highTempUpdate 1
unsigned char DisplayState = lowTempUpdate;
// Support functions for Display state and LCD
void ReadInState()
{
  // Read in DisplayState
  DisplayState = EEPROM[8];
  if (DisplayState >= 2) // This indicates that the
  { // EEPROM has NOT been written to.
    DisplayState = lowTempUpdate; // So load parameters with defaults.
  }
  else // if EEPROM has been written
  {
    // Read in other parameters.
    LowTemp.B[0] = EEPROM[0];
    LowTemp.B[1] = EEPROM[1];
    LowTemp.B[2] = EEPROM[2];
    LowTemp.B[3] = EEPROM[3];
    HighTemp.B[0] = EEPROM[4];
    HighTemp.B[1] = EEPROM[5];
    HighTemp.B[2] = EEPROM[6];
    HighTemp.B[3] = EEPROM[7];
  }
} // end of ReadInState
void WriteOutState()
{

  if (-1 < WrtPointer) // if there is data to write,
    WrtPointer--; // move to next sample
  PORTB |= 0x10; // Pin 12 to flag write to EEPROM
  // Switch between the different values to be written out.
  switch (WrtPointer)
  {
    case 0:
      EEPROM.update(0, LowTemp.B[0]);
      break;
    case 1:
      EEPROM.update(1, LowTemp.B[1]);
      break;
    case 2:
      EEPROM.update(2, LowTemp.B[2]);
      break;
    case 3:
      EEPROM.update(3, LowTemp.B[3]);
      break;
    case 4:
      EEPROM.update(4, HighTemp.B[0]);
      break;
    case 5:
      EEPROM.update(5, HighTemp.B[1]);
      break;
    case 6:
      EEPROM.update(6, HighTemp.B[2]);
      break;
    case 7:
      EEPROM.update(7, HighTemp.B[3]);
      break;
    case 8:
      EEPROM.update(8, DisplayState);
  } // end of switch to write multiple values
  PORTB &= ~0x10; // Clear flag of write.
} // end of WriteOutState
enum TempStates { xTempC, TempC, SaveTemp }; //define states: temperature not changing, temperature changing, temperature saving
TempStates TState = xTempC; // int initial state: temperature not changing
void DisplayUpdate() //state machine for temp change
{
  switch (TState) //switch state if
  {
    case xTempC:
      if ((ButtonCount == 4 || ButtonCount == 5) && rotate != 0) //button
        count = 4 , 5
                // and
                ecoderPosition has changed
      {
        TState = TempC; //switch to temp change state

      }
      break;
    case TempC: // temp change state
      if (ButtonCount < 4) //if button count less than one, ie, temp done
        changing
      {
        HighTemp.F = DayTemp; //set parameters
        LowTemp.F = NightTemp;
        TState = SaveTemp; //switch to save state
      }
      break;
    case SaveTemp:
      ResetWrt(); //call saving state maching
      if (WrtPointer == -1)
      {
        TState = xTempC;
      }
      break;
  }
} // end of DisplayUpdate()
void setupEEPROM() // set up parameters for eeprom
{
  ReadInState(); // read saved values once turned on
  DayTemp = HighTemp.F; //set saved parameters to temp limits
  NightTemp = LowTemp.F;
  // Indicators of write process.
  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);
  digitalWrite(13, LOW);
  digitalWrite(12, LOW);
  // Have timer run write code every 4 milliseconds.
  MsTimer2::set(4, WrtNextStep);
  MsTimer2::start();
} // End of setup
////////////END EEPROM//////////////////////
/////// Thermocouple/////////////////
#define SCK A5
#define MISO A4
#define CS A3
void SW_SPI_initialize()

{
  pinMode(CS, OUTPUT); //set Chip Select pin as output
  pinMode(SCK, OUTPUT); //set Serial Clock pin as output
  pinMode(MISO, INPUT); //set DATAIN pin as input
}
int SW_SPI16(void )
{
  digitalWrite(CS, LOW); //Chip select set to low
  digitalWrite(SCK, LOW); //Serial Clock pin set to low
  int SPI_data = 0;
  for (int k = 15; k >= 0; k--)
  {
    digitalWrite(SCK, HIGH); //set Serial Clock pin to HIGH
    if (PINC & 0x10)
    {
      bitSet(SPI_data, k);
    }
    digitalWrite(SCK, LOW); //set Serial Clock pin to LOW
  }
  digitalWrite(CS, HIGH); //set Serial Clock pin to LOW
  return SPI_data;
}
float ReadTemperature()
{ //convert bit data to degrees fahrenheit for thermocoumple
  int TempBits = (SW_SPI16() >> 3) & 0x0fff;
  float Temperature = 0.25 * (float)TempBits;
  Temperature = 1.8 * Temperature + 32.0;
  return Temperature;
}
/////// End Thermocouple//////////////
//////Print Data ////////////////
void printData() //function for printing data, printing cursor if settings
//changed
{
  ReadTemperature();
  LcdDriver.setCursor(0, 0);
  LcdDriver.print(ReadTemperature());
  LcdDriver.setCursor(10, 0);
  LcdDriver.print("D:");
  LcdDriver.print(DayTemp);
  LcdDriver.setCursor(0, 1);

  SendClock();
  LcdDriver.print(" ");
  LcdDriver.print("N:");
  LcdDriver.print(NightTemp);
  if (ButtonCount == 1) //blink if seconds are adjusted
  {
    LcdDriver.setCursor(7, 1);
    LcdDriver.blink();
    delay(100);
    LcdDriver.noBlink();
  }
  else if (ButtonCount == 2) //blink if minutes are adjusted
  {
    LcdDriver.setCursor(4, 1);
    LcdDriver.blink();
    delay(100);
    LcdDriver.noBlink();
  }
  else if (ButtonCount == 3) // blink if hours are adjusted
  {
    LcdDriver.setCursor(1, 1);
    LcdDriver.blink();
    delay(100);
    LcdDriver.noBlink();
  }
  else if (ButtonCount == 4) //blink if day temp is adjusted
  {
    LcdDriver.setCursor(10, 0);
    LcdDriver.blink();
    delay(100);
    LcdDriver.noBlink();
  }
  else if (ButtonCount == 5) // blink if night temp is adjusted
  {
    LcdDriver.setCursor(10, 1);
    LcdDriver.blink();
    delay(100);
    LcdDriver.noBlink();
  }
  else
  {
    LcdDriver.noBlink();
    delay(100);
  }
}
//////////End Print Data//////////////
/////// Set Temperature/////////////

int CoolingLED = 5;
int HeatingLED = 6;
void HeatORCool(float TimeSec) //turn on lEDs based on temperature
{
  if (ReadTemperature() - TimeSec >= 5) //if temperature 5 degrees higher than
    set temperature
  {
    digitalWrite(CoolingLED, HIGH); //turn on blue led
  }
  else if (ReadTemperature() - TimeSec <= -5) //if temp 5 degrees lower than
    set temp
  {
    digitalWrite(HeatingLED, HIGH); //turn on red led
  }
  else //turn off leds
  {
    digitalWrite(CoolingLED, LOW);
    digitalWrite(HeatingLED, LOW);
  }
}
void AC() // the parameter analyzed by HeatORCool function based on time of day
{
  if ((Hours >= 7 & Hours < 21) & (Minutes >= 0 & Seconds >= 0)) //if daytime
  {
    HeatORCool(DayTemp); //set parameter is daytemp
  }
  else
  {
    HeatORCool(NightTemp); //set parameter is night temp
  }
}
/////// End Set Temperature//////////
void setup()
{
  EncoderInitialize(); //intitize encoder
  ButtonInitialize(4); //initilize button
  setupEEPROM(); // intitalize eeprom
  // Set up clock
  ClockTimer = millis();
  Hours = 23;
  Minutes = 59;
  Seconds = 55;
  Serial.begin(9600); //set serial monitor to 9600 baud

  BlinkTimer = millis(); //set upt timers
  TempTimer = millis();
  LcdDriver.begin(16, 2); //define lcd display
  LcdDriver.clear();
}
void loop()
{
  if (millis() - BlinkTimer >= 100)
  {
    LcdDriver.clear(); // clear lcd
    ButtonCnt(); //count how many times the button is press
    setParameter(); //change parameters based on button press
    DisplayUpdate(); //update the time or set temp
    printData(); //print data
    BlinkTimer += 100;
  }
  if (millis() - ClockTimer >= CLOCK_INTERVAL) //every seconds
  {
    UpdateClock(); //update clock
    ClockTimer += CLOCK_INTERVAL; //UPDATE TIMER
  }
}
