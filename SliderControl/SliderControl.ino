// SliderControl.pde
// -*- mode: C++ -*-
//
// 
#define OLED_RESET 12  //Pin # the OLED module's RST pin is connected to.

#include <ArduinoUnit.h>
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
  { "SubTrigger", 'F',  (void*)some_function},
  { "SubTrigger2", 'F',  (void*)some_function}
};

sub_menu sub_menu_trigger = {
  menu_trigger, 0, 1
};


menu_item menu_main[] =
{
    { "Trigger     ", 'M', &sub_menu_trigger },
    { "Some funtion", 'F',(void*)some_function }
};

sub_menu sub_menu_main = {
  menu_main, 0, 1
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
 
Menu menu  = Menu::Menu(&sub_menu_main);

void setup()
{    
  
  initDebug(1);
 menu  = Menu::Menu(&sub_menu_main);
  
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
int okButtonState = LOW;
int cancelButtonState = LOW;

void loop()
{  

  showMenu();
  int okButton = digitalRead(OK_BUTTON_PIN);
  int cancelButton  = digitalRead(CANCEL_BUTTON_PIN);
  int upButton = digitalRead(UP_BUTTON_PIN);
  int downButton = digitalRead(DOWN_BUTTON_PIN);

  if (upButton != upButtonState) {
    upButtonState = upButton;
    if (upButtonState == HIGH) {
      debugStr("up();");
      menu.up(); 
    }
  }

  if (downButton != downButtonState) {
    downButtonState = downButton;
    if (downButtonState == HIGH) {
        debugStr("down();");
        menu.down(); 
    }
  }

  if (okButton != okButtonState) {
      okButtonState = okButton;
      if (okButtonState == HIGH) {
        debugStr("select();");
        menu.select();
      }
  }

  if (cancelButton != cancelButtonState) {
      cancelButtonState = cancelButton;
      if (cancelButtonState == HIGH) {
       
        debugStr(menu.currentMenuItem()->name);
        debugInt("currentIndex ", menu.currentMenu()->currentIndex);
        menu.back();
        debugStr("back();");
        // menu_item currentMenuItem = 
        debugStr(menu.currentMenuItem()->name);
        debugInt("currentIndex ", menu.currentMenu()->currentIndex);
      }
  }    
  
}