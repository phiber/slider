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

#ifndef EEPROMAnything_h
#include "EEPROMAnything.h"
#define EEPROMAnything_h
#endif

#ifndef EEPROM_h
#include "EEPROM.h"
#define EEPROM_h
#endif

#ifndef Digital_Light_TSL2561_h
#include <Digital_Light_TSL2561.h>
#define Digital_Light_TSL2561_h
#endif


void loadSettings() {
  EEPROM_readAnything(0, configuration);
  while (!isOkButtonPressed()) {
    showOnDisplay(F("Settings loaded."));
  }
}

void saveSettings() {
  EEPROM_writeAnything(0, configuration);
  while (!isOkButtonPressed()) {
    showOnDisplay(F("Settings saved."));
  }
}


MenuItem menu_main[] = {
  MenuItem::MenuItem("Mode", 'M', NULL),
  MenuItem::MenuItem("Load Settings", 'F', (void*)loadSettings),
  MenuItem::MenuItem("Save Settings", 'F', (void*)saveSettings)
};

MenuItem menu_mode[] = {
  MenuItem::MenuItem("Timed", 'M',  NULL),
  MenuItem::MenuItem("Lux timed", 'M', NULL)
};

MenuItem menu_timed[] = {
  MenuItem::MenuItem("Setup", 'M',  NULL),
  MenuItem::MenuItem("Start", 'F',  (void*)runTimedTimelapse)
};

MenuItem menu_lux[] = {
  MenuItem::MenuItem("Setup", 'M',  NULL),
  MenuItem::MenuItem("Start", 'F',  (void*)runLuxTimedTimelapse)
};

MenuItem menu_lux_setup[] = {
  MenuItem::MenuItem("Enter Time", 'F',  (void*)enterTimeForTimed),
  MenuItem::MenuItem("Enter Direction", 'F',  (void*)enterDirectionForTimed),
  MenuItem::MenuItem("Enter #Frames", 'F', (void*)enterFramesForTimed),
  MenuItem::MenuItem("Enter ISO",'F', (void*)enterISOForLux),
  MenuItem::MenuItem("Enter Aperture",'F', (void*)enterApertureForLux),
  MenuItem::MenuItem("Enter #Laps", 'F', (void*)enterLapsForTimed)
};

MenuItem menu_timed_setup[] = {
  MenuItem::MenuItem("Enter Time", 'F',  (void*)enterTimeForTimed),
  MenuItem::MenuItem("Enter Direction", 'F',  (void*)enterDirectionForTimed),
  MenuItem::MenuItem("Enter #Frames", 'F', (void*)enterFramesForTimed),
  MenuItem::MenuItem("Enter Bulb Time", 'F', (void*)enterBulbForTimed),
  MenuItem::MenuItem("Enter #Laps", 'F', (void*)enterLapsForTimed)
};

MenuItem menu_setup[] = {  
  MenuItem::MenuItem("Initialize", 'F',  (void*)intializeSlider)
}; 

SubMenu sub_menu_main = SubMenu::SubMenu(menu_main, 0, 2, NULL);
SubMenu sub_menu_mode = SubMenu::SubMenu(menu_mode, 0, 1, &sub_menu_main);
SubMenu sub_menu_lux = SubMenu::SubMenu(menu_lux, 0, 1, &sub_menu_mode);
SubMenu sub_menu_lux_setup = SubMenu::SubMenu(menu_lux_setup, 0, 5, &sub_menu_lux);
SubMenu sub_menu_timed = SubMenu::SubMenu(menu_timed, 0, 1, &sub_menu_mode);
SubMenu sub_menu_timed_setup = SubMenu::SubMenu(menu_timed_setup, 0, 4, &sub_menu_timed);
SubMenu sub_menu_setup = SubMenu::SubMenu(menu_setup, 0,0, &sub_menu_main);



Menu menu  = Menu::Menu(&sub_menu_main);



void setup()
{    


  menu_main[0].function = &sub_menu_mode;
  menu_main[1].function = &sub_menu_setup;
  menu_mode[0].function = &sub_menu_timed;
  menu_mode[1].function = &sub_menu_lux;
  menu_timed[0].function = &sub_menu_timed_setup;
  menu_lux[0].function = &sub_menu_lux_setup;



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


  setupProgressBar();
  clearProgressBar();

  TSL2561.init();

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