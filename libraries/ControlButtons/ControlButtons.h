#ifndef ControlButtons_h
#define ControlButtons_h

#include "Arduino.h"
#include <Wire.h>
#include <Display.h>

const int CANCEL_BUTTON_PIN = 4;
const int OK_BUTTON_PIN = 5;
const int UP_BUTTON_PIN = 10;
const int DOWN_BUTTON_PIN = 6;


int downButtonState = LOW;
int upButtonState = LOW;
int okButtonState = LOW;
int cancelButtonState = LOW;



void initializeControlButtons() {

  pinMode(CANCEL_BUTTON_PIN, INPUT);
  pinMode(OK_BUTTON_PIN, INPUT);
  pinMode(DOWN_BUTTON_PIN, INPUT);
  pinMode(UP_BUTTON_PIN, INPUT);
}

bool isUpButtonPressed() {
  int upButton = digitalRead(UP_BUTTON_PIN);
  if (upButton != upButtonState) {
    upButtonState = upButton;
    return (upButtonState == HIGH);
  }
  return false;
}


bool isDownButtonPressed() {
  int downButton = digitalRead(DOWN_BUTTON_PIN);
  if (downButton != downButtonState) {
    downButtonState = downButton;
    return (downButtonState == HIGH);
  }
  return false;
}


bool isOkButtonPressed() {
  int okButton = digitalRead(OK_BUTTON_PIN);
  if (okButton != okButtonState) {
    okButtonState = okButton;
    return (okButtonState == HIGH);
  }
  return false;
}



bool isCancelButtonPressed() {
  int cancelButton = digitalRead(CANCEL_BUTTON_PIN);
  if (cancelButton != cancelButtonState) {
    cancelButtonState = cancelButton;
    return (cancelButtonState == HIGH);
  }
  return false;
}




#endif