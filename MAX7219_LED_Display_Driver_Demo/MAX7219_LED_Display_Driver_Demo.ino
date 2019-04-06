/*
 Name:		MAX7219_LED_Display_Driver_Demo.ino
 Created:	27/03/2019 10:40:14 PM
 Author:	Wojciech Cybowski (github.com/wcyb)
 License:	GPL v2
*/

#include "MAX7219_LED_Display_Driver.h"

MAX7219* max7219 = nullptr;
uint8_t intensitySetting = 0xFF;

// the setup function runs once when you press reset or power the board
void setup() {
	max7219 = new MAX7219(52, 50, 48);//pins: data out, load and clk, default 8 digits will be displayed with maximum intensity
	max7219->setDisplayTest(true);//test display mode
	delay(3000);
	max7219->setDisplayTest(false);//disable test mode
	max7219->sendNumber(12345678);//display this number
	delay(3000);
	max7219->sendNumber(3.1415926, 7);//display PI, notice that last digit will be different due to double being 4 bytes long (Arduino Mega)
	delay(3000);
	max7219->sendNumber(-1234567);//display negative number
	delay(3000);
	max7219->sendNumber(-3.141592, 6);//display -PI, notice that last digit will be different due to double being 4 bytes long (Arduino Mega)
	delay(3000);
	max7219->clear();//clear display
}

// the loop function runs over and over again until power down or reset
void loop() {
	if (!(millis() % 100))//do this every 100 ms
	{
		max7219->setIntensity(intensitySetting);
		intensitySetting = (intensitySetting > 0) ? intensitySetting - 5 : 0xFF;//decrease intensity value after every 100 ms (16 steps possible, so it will be darker every 0x0F)
		max7219->sendNumber(millis());//display current runtime in ms
	}
}
