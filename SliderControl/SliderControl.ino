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



Adafruit_SSD1306 display(OLED_RESET);

//AccelStepper stepper; // Defaults to AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 5
AccelStepper stepper(1,9,8); // 1 = type, 9 = step, 8 = direction
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
 


void setup()
{  
 
  initDebug(1);

  
  //intializeSlider();

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC);
  display.setTextSize(0.5);
  display.setTextColor(WHITE); 
  display.setCursor(0,0);

  stepper.setMaxSpeed(ACCEL_STEPPER_MAX_SPEED);

  //Initialize switch1 as Input
  pinMode(SWITCH_1_PIN, INPUT);
  pinMode(SWITCH_2_PIN, INPUT);


  // OK, CANCEL Buttons
  pinMode(CANCEL_BUTTON_PIN, INPUT);
  pinMode(OK_BUTTON_PIN, INPUT);

  stepper.setSpeed(speedSensorValue);
  showMainMenu();

}


void showMainMenu() {

  int sensorValue = readSpeedSensorValue();
  int selection = 1;
  const int MIN_SELECTION = 1;
  const int MAX_SELECTION = 2;
  while ( 1 ) {

    int newSensorValue = readSpeedSensorValue();
    if (sensorValue != newSensorValue) {
      if (newSensorValue > sensorValue && selection < MAX_SELECTION) {
        selection += 1;
        debugInt("Selection changed: ", selection);
      } else if (selection > MIN_SELECTION) {
        selection -= 1;
        debugInt("Selection changed: ", selection);
      }
      sensorValue = newSensorValue;
    }

    display.clearDisplay();
    display.setCursor(0,0);
    for (int i = 1; i <= MAX_SELECTION; i++) {
      if (selection == i) {
        display.setTextColor(BLACK, WHITE);    
      } else {
        display.setTextColor(WHITE);
      }
      display.println(i);
      display.display();
    }

    delay(500);
  
  }
  
}


void intializeSlider() {
  int speed = ACCEL_STEPPER_MAX_SPEED;

  stepper.setMaxSpeed(speed);
  debugStr("To End...");
  changeSpeedTo(speed);
  while ( ! endReached()) {
    stepper.runSpeed();
  }
  debugStr("End reached.");
  stepper.setCurrentPosition(0);
  changeSpeedTo(0);
  changeDirection();
  changeSpeedTo(speed);
  
  debugStr("To Begin..."); 
  while ( ! endReached()) {
    stepper.runSpeed();
  }
  debugStr("Begin reached.");
  changeSpeedTo(0);
  changeDirection();
  long pos = stepper.currentPosition();
  debugStr(String(pos));
  delay(5000);
}




int readSpeedSensorValue() {
  int speed = analogRead(SPEED_SENSOR_PIN);
  return map(speed,0,ACCEL_STEPPER_MAX_SPEED,0,100); // percent will range from -100 to 100.
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

void changeSpeedTo(int speedPercentage) {
  if (speedPercentage != speedSensorValue) { 
    float maximalSpeed = ACCEL_STEPPER_MAX_SPEED;
    float percentage = (float)speedPercentage / 100;
    debugFloat("percentage = ", percentage);
    float newSpeed = maximalSpeed * percentage * _direction; 
    stepper.setSpeed(newSpeed);
    showOnDisplay("speed = ", speedPercentage);
    speedSensorValue = speedPercentage;
  }
}

float changeDirection() {
  _direction = _direction * -1;
  debugFloat("direction changed:", _direction);
  return _direction;
}

float getDirection() {
  return _direction;
}


boolean endReached() {
  int switch1 = digitalRead(SWITCH_1_PIN);
  int switch2 = digitalRead(SWITCH_2_PIN);
  return (
    (getDirection() == DIRECTION_TOWARDS_SWITCH_2 && switch2 == HIGH)
      ||
    (getDirection() == DIRECTION_TOWARDS_SWITCH_1 && switch1 == HIGH)
    );
}

void loop()
{  
  
  int speed = readSpeedSensorValue();
 
  
  //debug("Switch1 = ", switch1);
  if (endReached()) {
      changeSpeedTo(0);
      speed = speed * changeDirection();
  } 
 
  changeSpeedTo(speed);
  stepper.runSpeed();
}