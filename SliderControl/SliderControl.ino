// SliderControl.pde
// -*- mode: C++ -*-
//
// 


#include <AccelStepper.h>

#include <Wire.h>

#include "debug.h"
#include <Menu.h>
#include <SliderStepper.h>
#include <Display.h>
#include <ControlButtons.h>

#ifndef Adafruit_GFX_h
#include <Adafruit_GFX.h>
#endif

//#ifndef Adafruit_SSD1306_h
//#include <Adafruit_SSD1306.h>
//#endif


void some_function_main() {
  Serial.println("Main: Some Function called!");
}

void some_function_trigger() {

  Serial.println("Trigger: Some Function called!");
}


MenuItem menu_main[] = {
  MenuItem::MenuItem("Trigger", 'M', NULL),
  MenuItem::MenuItem("Setup", 'M', NULL)
};

MenuItem menu_trigger[] = {
  MenuItem::MenuItem("Timed", 'M',  NULL),
  MenuItem::MenuItem("SubTrigger2", 'F',  (void*)some_function_trigger)
};

MenuItem menu_timed[] = {
  MenuItem::MenuItem("Setup", 'M',  NULL),
  MenuItem::MenuItem("Start", 'F',  (void*)runTimedTimelapse)
};

MenuItem menu_timed_setup[] = {
  MenuItem::MenuItem("Enter Time", 'F',  (void*)enterTimeForTimed),
  MenuItem::MenuItem("Enter Direction", 'F',  (void*)enterDirectionForTimed),
  MenuItem::MenuItem("Enter #Frames", 'F', (void*)enterFramesForTimed),
  MenuItem::MenuItem("Enter #Laps", 'F', (void*)enterLapsForTimed)
};

MenuItem menu_setup[] = {  
  MenuItem::MenuItem("Initialize", 'F',  (void*)intializeSlider)
}; 

SubMenu sub_menu_main = SubMenu::SubMenu(menu_main, 0, 1, NULL);
SubMenu sub_menu_trigger = SubMenu::SubMenu(menu_trigger, 0, 1, &sub_menu_main);
SubMenu sub_menu_timed = SubMenu::SubMenu(menu_timed, 0, 1, &sub_menu_trigger);
SubMenu sub_menu_timed_setup = SubMenu::SubMenu(menu_timed_setup, 0, 2, &sub_menu_timed);
SubMenu sub_menu_setup = SubMenu::SubMenu(menu_setup, 0,0, &sub_menu_main);



Menu menu  = Menu::Menu(&sub_menu_main);

void setup()
{    


  menu_main[0].function = &sub_menu_trigger;
  menu_main[1].function = &sub_menu_setup;
  menu_trigger[0].function = &sub_menu_timed;
  menu_timed[0].function = &sub_menu_timed_setup;



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


  initializeControlButtons();


  //stepper.setSpeed(speedSensorValue);

  pinMode(FOCUS_PIN, OUTPUT);
  pinMode(TRIGGER_PIN, OUTPUT);
  
}


void showMenu() {
  showOnDisplay(menu.currentMenuItem()->name);

  if (isUpButtonPressed()) {
    menu.up(); 
  }

  if (isDownButtonPressed()) {
    menu.down(); 
  }

  if (isOkButtonPressed()) {
    menu.select();
  }

  if (isCancelButtonPressed()) {
    menu.back();

  } 
}



void loop()
{  
  showMenu();  
}