
#include <Wire.h>

#ifndef Adafruit_GFX_h
#include <Adafruit_GFX.h>
#define Adafruit_GFX_h  
#endif

#ifndef Adafruit_LEDBackpack_h
#include <Adafruit_LEDBackpack.h>
#define Adafruit_LEDBackpack_h
#endif
                          
Adafruit_24bargraph bar = Adafruit_24bargraph();

void setupProgressBar() {
	bar.begin(0x77);  // pass in the address
}



void clearProgressBar() {
	for (uint8_t b=24; b>0; b--) {
     bar.setBar(b-1, LED_OFF);
   }
   bar.writeDisplay();
}

void setProgressBar(float percentage){	

	uint8_t numberOfBarLEDs = (uint8_t)(24.0 * percentage / 100.0);

	Serial.println("Number of lights: "); Serial.println(numberOfBarLEDs);
	clearProgressBar();

	for (uint8_t i = 0; i < numberOfBarLEDs; i++) {
		bar.setBar(23-i, LED_GREEN);
	}  
  	bar.writeDisplay();
}
