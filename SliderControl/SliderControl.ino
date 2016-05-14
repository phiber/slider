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

#include <SPI.h>

#ifndef Adafruit_GFX_h
#include <Adafruit_GFX.h>
#define Adafruit_GFX_h
#endif

#ifndef Adafruit_SSD1306_h
#include <Adafruit_SSD1306.h>
#define Adafruit_SSD1306_h
#endif

#include <avr/pgmspace.h>

#ifndef ProgressBar_h
#include <ProgressBar.h>
#define ProgressBar_h
#endif
 
#ifndef Adafruit_LEDBackpack_h 
#include <Adafruit_LEDBackpack.h>
#define Adafruit_LEDBackpack_h
#endif

#include <Config.h>

#ifndef SdFat_h
#include <SdFat.h>
#define SdFat_h
#endif

#ifndef Config_h
#include <Config.h>
#define Config_h
#endif

#ifndef ArduinoJson_h
#include <ArduinoJson.h>
#define ArduinoJson_h
#endif



MenuItem menu_main[] = {
  MenuItem("Mode", 'M', NULL),
  MenuItem("Global Settings", 'M', NULL),
  MenuItem("Load Settings", 'F', (void*)loadSettings),
  MenuItem("Save Settings", 'F', (void*)saveSettings)
};

MenuItem menu_mode[] = {
  MenuItem("Timed", 'M',  NULL)
};

MenuItem menu_timed[] = {
  MenuItem("Setup", 'M',  NULL),
  MenuItem("Start", 'F',  (void*)runTimedTimelapse)
};

MenuItem menu_timed_setup[] = {
  MenuItem("Enter Time", 'F',  (void*)enterTimeForTimed),
  MenuItem("Enter Direction", 'F',  (void*)enterDirectionForTimed),
  MenuItem("Enter #Frames", 'F', (void*)enterFramesForTimed),
  MenuItem("Enter Bulb Time", 'F', (void*)enterBulbForTimed),
  MenuItem("Enter #Laps", 'F', (void*)enterLapsForTimed)
};

MenuItem menu_setup[] = {  
  MenuItem("Initialize", 'F',  (void*)intializeSlider)
}; 

MenuItem menu_global_settings[] = {
  MenuItem("Set init speed", 'F', (void*)enterInitSpeed)
};

SubMenu sub_menu_main = SubMenu(menu_main, 0, 3, NULL);
SubMenu sub_menu_mode = SubMenu(menu_mode, 0, 0, &sub_menu_main);
SubMenu sub_menu_global_settings = SubMenu(menu_global_settings, 0, 0, &sub_menu_main);
SubMenu sub_menu_timed = SubMenu(menu_timed, 0, 1, &sub_menu_mode);
SubMenu sub_menu_timed_setup = SubMenu(menu_timed_setup, 0, 4, &sub_menu_timed);
SubMenu sub_menu_setup = SubMenu(menu_setup, 0,0, &sub_menu_main);



Menu menu  = Menu(&sub_menu_main);



void setup()
{    

  

  menu_main[0].function = &sub_menu_mode;
  menu_main[1].function = &sub_menu_global_settings;
  menu_mode[0].function = &sub_menu_timed;
  menu_timed[0].function = &sub_menu_timed_setup;

	


  initDebug(1);
  Serial.print(F("Hello ")); 

  menu  = Menu(&sub_menu_main);
  
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


  setupProgressBar();
  clearProgressBar();
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

