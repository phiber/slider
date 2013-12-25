// #define debug_h
#include "Arduino.h"


void initDebug(int debugOn);
void debugStr(String text);  
void debugInt(String text, int value);
void debugFloat(String text, float value);