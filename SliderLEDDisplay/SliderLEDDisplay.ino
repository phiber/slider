
#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"

unsigned long displayLEDBarGraphInterval= 100;
unsigned long previousMillis= 0;
uint8_t barGraphState = 0;
uint8_t barGraphColor = LED_GREEN;

Adafruit_24bargraph bar = Adafruit_24bargraph();

void setup() {  
  bar.begin(0x77);  // pass in the address

}


void loop() {
  if ((unsigned long)(millis() - previousMillis) >= displayLEDBarGraphInterval) {
    incrementBarGraph(barGraphColor);
    previousMillis = millis();
  }
}

void incrementBarGraph(uint8_t color){
  if (barGraphState < 24) {
     bar.setBar(barGraphState, color);
  }
  if(barGraphState == 24) {
    clearBarGraph();
    barGraphState = 0;
    barGraphColor = (barGraphColor == LED_GREEN) ? LED_RED : (barGraphColor == LED_RED)? LED_YELLOW : LED_GREEN ;
  } else {
      barGraphState++;
  } 
  bar.writeDisplay();


}

void clearBarGraph() {
   for (uint8_t b=24; b>0; b--) {
     bar.setBar(b-1, LED_OFF);
   }
   bar.writeDisplay();
}
