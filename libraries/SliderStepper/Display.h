
#ifndef Display_h
#define Display_h

#define OLED_RESET 46  //Pin # the OLED module's RST pin is connected to.

#include "Arduino.h"
#include <Wire.h>

#ifndef Adafruit_GFX_h
#include <Adafruit_GFX.h>
#endif
  
#ifndef Adafruit_SSD1306_h
#include <Adafruit_SSD1306.h>
#define Adafruit_SSD1306_h
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

void showIntInput(String text, int number) {
	display.clearDisplay(); 
	display.setTextColor(WHITE, BLACK);
	display.setCursor(0,0);
	display.println(text);
	display.setCursor(0,8);
	display.setTextColor(BLACK, WHITE);
	if (number < 10) display.print("0");
	display.print(number);
	display.setTextColor(WHITE);
	display.display();	
}

void showTimeInput(String text, int hours, int minutes, int seconds, int invertedPosition) {
	display.clearDisplay(); 
	display.setTextColor(WHITE, BLACK);
	display.setCursor(0,0);
	display.println(text);
	display.setCursor(0,8);
	if (invertedPosition == 0) {
		display.setTextColor(BLACK, WHITE);
	}
	if (hours < 10) display.print("0");
	display.print(hours);
	display.setTextColor(WHITE);
	display.print(":");
	
	if (invertedPosition == 1) {
		display.setTextColor(BLACK, WHITE);
	}
	if (minutes < 10) display.print("0");
	display.print(minutes);
	display.setTextColor(WHITE);
	display.print(":");

	if (invertedPosition == 2) {
		display.setTextColor(BLACK, WHITE);
	}
	if (seconds < 10) display.print("0");
	display.print(seconds);
	display.setTextColor(WHITE);
	display.display();	
}


#endif