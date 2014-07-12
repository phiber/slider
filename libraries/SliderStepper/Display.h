
#ifndef Display_h
#define Display_h

#define OLED_RESET 12  //Pin # the OLED module's RST pin is connected to.

#include "Arduino.h"
#include <Wire.h>

#ifndef Adafruit_GFX_h
#include <Adafruit_GFX.h>
#endif

#ifndef Adafruit_SSD1306_h
#include <Adafruit_SSD1306.h>
#endif



Adafruit_SSD1306 display(OLED_RESET);



void showOnDisplay(String text) {
	display.setCursor(0,0);
	display.clearDisplay(); 
	display.println(text);
	display.display();
}

void showOnDisplay(String line1, String line2) {
	display.clearDisplay(); 
	display.setCursor(0,0);
	display.println(line1);
	display.setCursor(0,8);
	display.println(line2);
	display.display();	
}

void showOnDisplay(String text, int value) {
	showOnDisplay(text+value+"%");
}


#endif