# ece241-thermostat
Software Documentation:

  The program was divided into five parts:
  
  Main Program (Appendix A): 
  
    •	Defines EEPROM state machines to save temperature data 
    •	Defines temperature readings from thermocouple and converts that data to Fahrenheit
    •	Sets 5 degree temperature difference from read temperature and set temperature to define cooling and heating conditions.
    •	Prints the data onto the LCD
    
  Button Debounce (Appendix B):  Uses state machine to read when the button is pressed
  Clock Basics (Appendix C): Uses state machines to count the time, and prints that data out onto the LCD
  Encoder Monitor (Appendix D): Defines parameters to be changed based on how many times the button is pressed and also defines the       encoder position which allows these parameters to be adjusted once set.
  MsTimer2 (Appendix E): Library taken from the Arduino, a timer
  More information is documented about each part can be found in the Appendix.

Hardware Documentation:  
  Pins 5 and 6 were chosen to run the LED’s due to the reason that they were not taken by other devices. A resistor was added in    series with each LED because the voltage generated by the pin was too high for the LED.
  The LCD prints temperature on the top left-hand corner and the set day temperature on the top right-hand corner. The time is  displayed on the bottom left-hand corner, and the set night temp is set on the bottom right-hand corner. 
The thermocouple was hooked up by the following description:
  •	Serial clock was connected to analog pin 5
  •	Master In, Slave Out was connected to analog pin 4
  •	Chip Select was connected to analog pin 3
Verification Information
  The program was verified by four methods:
    1.	Ensure that the two temperature settings were printed by adding an “N” for night and “D” for day, to check that they were  float, the numbers were checked if they had a decimal.
    2.	Ensure that the temperature can be changed by pressing the button times to allow the changing of temperature and rotating     the encoder to see if the set day temperature and set night temperature can be changed.
    3.	Check if the orange light turned on after turning off setting changes (by pressing the button four times)  and then turning the Arduino off and turning it on to check if the temperatures were saved. This was done by only changing the day temperature, then checking only the night temperature, and then checking both.
    4.	Check if the lights turned on by changing day time and changing night time to see if the blue light (cooling) and the red light (heating) turned on at the right conditions (time of day, temperature difference from read temperature and set temperature).
