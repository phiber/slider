#ifndef ControlButtons_h
#define ControlButtons_h

#include "Arduino.h"
#include <Wire.h>
#include <Display.h>

const int CANCEL_BUTTON_PIN = 4;
const int OK_BUTTON_PIN = 5;
const int UP_BUTTON_PIN = 11;
const int DOWN_BUTTON_PIN = 6;


long downButtonTime() {
  static unsigned long startTime = 0;
  static boolean state;
  if (digitalRead(DOWN_BUTTON_PIN) == state) {
    state = ! state;
    startTime = millis();
  }
  if (state == LOW) {
    return millis() - startTime;
  } else {
    return 0;
  }
}

bool isDownButtonPressed() {
  static boolean state;
  bool downButtonBeingPressed = downButtonTime() > 0;
  if (state != downButtonBeingPressed) {
    state = downButtonBeingPressed;
    return state;
  }
  return false;
}

long upButtonTime() {
  static unsigned long startTime = 0;
  static boolean state;
  if (digitalRead(UP_BUTTON_PIN) == state) {
    state = ! state;
    startTime = millis();
  }
  if (state == LOW) {
    return millis() - startTime;
  } else {
    return 0;
  }
}

bool isUpButtonPressed() {  
  static boolean state;
  bool upButtonBeingPressed = upButtonTime() > 0;
  if (state != upButtonBeingPressed) {
    state = upButtonBeingPressed;
    return state;
  }
  return false;
}

bool isOkButtonPressed() {
  static boolean okButtonState;
  int okButton = digitalRead(OK_BUTTON_PIN);
  if (okButton != okButtonState) {
    okButtonState = okButton;
    return (okButtonState == HIGH);
  }
  return false;
}

bool isCancelButtonPressed() {
  static boolean cancelButtonState;
  int cancelButton = digitalRead(CANCEL_BUTTON_PIN);
  if (cancelButton != cancelButtonState) {
    cancelButtonState = cancelButton;
    return (cancelButtonState == HIGH);
  }
  return false;
}

void initializeControlButtons() {
  pinMode(CANCEL_BUTTON_PIN, INPUT);
  pinMode(OK_BUTTON_PIN, INPUT);
  pinMode(DOWN_BUTTON_PIN, INPUT);
  pinMode(UP_BUTTON_PIN, INPUT);
}



#endif