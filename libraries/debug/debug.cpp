/*
 * 
 */
#include "Arduino.h"  
#include "debug.h" 

int DEBUG = 0; 

void initDebug(int debugOn) {
	if (debugOn == 1) {
		DEBUG = 1;	
    	Serial.begin(9600);
  	}
}

void debugStr(String text) {
  if (DEBUG == 0) {
    return;
  } 
  Serial.print(text);
  Serial.print("\n");	
}

void debugInt(String text, int value) {
  if (DEBUG == 0) {
    return;
  }
  Serial.print(text);
  Serial.print(value);
  Serial.print("\n");
}

void debugFloat(String text, float value) {
  if (DEBUG == 0) {
    return;
  }
  Serial.print(text);
  Serial.print(value);
  Serial.print("\n");
}

