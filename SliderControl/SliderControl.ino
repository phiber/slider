// SliderControl.pde
// -*- mode: C++ -*-
//
// 
#define OLED_RESET 12  //Pin # the OLED module's RST pin is connected to.

#include <AccelStepper.h>

#include <Wire.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "debug.h"
#include <Menu.h>
#include <StackArray.h>


bool functionCalled;

void some_function() {
  functionCalled = true;
}

menu_item menu_trigger[] = {
  { "DoTrigger", 'F',  (void*)some_function}
};

menu_item menu_main[] =
{
    // { "This        ", 'M', (void*)menu_trigger },
    // { "is          ", 'M', (void*)menu_trigger },
    { "the         ", 'M', (void*)menu_trigger },
    { "the         ", 'M', (void*)menu_trigger },
    { "Menu        ", 'F',(void*)some_function },
    {"Termination ", 'T', (void*)some_function }
};



Adafruit_SSD1306 display(OLED_RESET);

//AccelStepper stepper; // Defaults to AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 5
//AccelStepper stepper(1,9,8); // 1 = type, 9 = step, 8 = direction
const int ACCEL_STEPPER_MAX_SPEED = 1024;

const int SPEED_SENSOR_PIN = 0;
int speedSensorValue = 0;

const int SWITCH_1_PIN = 2;
const int SWITCH_2_PIN = 7;

const float DIRECTION_TOWARDS_SWITCH_2 = -1;
const float DIRECTION_TOWARDS_SWITCH_1 = 1;
float _direction = DIRECTION_TOWARDS_SWITCH_2;

const int CANCEL_BUTTON_PIN = 4;
const int OK_BUTTON_PIN = 5;
const int UP_BUTTON_PIN = 10;
const int DOWN_BUTTON_PIN = 6;
 
Menu menu  = Menu::Menu((menu_item*)menu_main);

void setup()
{  
  


  initDebug(1);
  menu  = Menu::Menu((menu_item*)menu_main);
  debugInt("menu manin items size = ", sizeof menu_main);
  
  //intializeSlider();

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC);
  display.setTextSize(0.5);
  display.setTextColor(WHITE); 
  display.setCursor(0,0);

  //stepper.setMaxSpeed(ACCEL_STEPPER_MAX_SPEED);

  //Initialize switch1 as Input
  pinMode(SWITCH_1_PIN, INPUT);
  pinMode(SWITCH_2_PIN, INPUT);


  // OK, CANCEL Buttons
  pinMode(CANCEL_BUTTON_PIN, INPUT);
  pinMode(OK_BUTTON_PIN, INPUT);
  pinMode(DOWN_BUTTON_PIN, INPUT);
  pinMode(UP_BUTTON_PIN, INPUT);
  
  
  //stepper.setSpeed(speedSensorValue);
}


void showMenu() {
  showOnDisplay(menu.currentMenuItem()->name);
}




void showOnDisplay(String text) {
  display.setCursor(0,0);
  display.clearDisplay(); 
  display.println(text);
  display.display();
}

void showOnDisplay(String text, int value) {
  showOnDisplay(text+value+"%");
}

int downButtonState = LOW;
int upButtonState = LOW;

void loop()
{  

  showMenu();
  // debugStr(menu.currentMenuItem()->name);
  int okButton = digitalRead(OK_BUTTON_PIN);
  int cacnelButton  = digitalRead(CANCEL_BUTTON_PIN);
  int upButton = digitalRead(UP_BUTTON_PIN);
  int downButton = digitalRead(DOWN_BUTTON_PIN);

  if (upButton != upButtonState) {
    upButtonState = upButton;
    if (upButtonState == HIGH) {
      menu.up(); 
    }
  }

  if (downButton != downButtonState) {
    downButtonState = downButton;
    if (downButtonState == HIGH) {
      menu.down(); 
    }
  }
    
  
  // debugInt("ok = ", okButton);
  // debugInt("cancel = ", cacnelButton);
  // debugInt("up= ", upButton);
  // debugInt("down = ", downButton);


  // if (okButton == HIGH) {
  //    menu.select();
  //  } else if (cacnelButton == HIGH) {
  //    menu.back();
  //  } else if (upButton == HIGH) {
  //    menu.up();
  //  } else if (downButton == HIGH) {
    
  //  }

  //delay(2000);
}