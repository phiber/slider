#include <Wire.h>
#include <SPI.h>



#ifndef Adafruit_GFX_h
#include <Adafruit_GFX.h>
#define Adafruit_GFX_h
#endif

#ifndef Adafruit_SSD1306_h
#include <Adafruit_SSD1306.h>
#define Adafruit_SSD1306_h
#endif

#define OLED_RESET 46  //Pin # the OLED module's RST pin is connected to.


Adafruit_SSD1306 display(OLED_RESET);

void setup() {
 
  Serial.begin(9600);


  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC);
  display.setTextSize(0.5);
  display.setTextColor(WHITE); 
  display.setCursor(0,0);
  
  
}

void showOnDisplay(String text) {
	Serial.print(text);
	display.setCursor(0,0);
	display.clearDisplay(); 
	display.println(text);
	display.display();
}

void loop() {
    showOnDisplay("Hello World");
}
