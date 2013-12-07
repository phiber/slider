// SliderControl.pde
// -*- mode: C++ -*-
//
// 
#define OLED_RESET 12  //Pin # the OLED module's RST pin is connected to.

#include <AccelStepper.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display(OLED_RESET);

//AccelStepper stepper; // Defaults to AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 5
AccelStepper stepper(1,9,8); // 1 = type, 9 = step, 8 = direction
const int ACCEL_STEPPER_MAX_SPEED = 1024;

const int DEBUG = 1; // Debug on=1, off=0

const int SPEED_SENSOR_PIN = 0;
int speedSensorValue = 0;

const int SWITCH_1_PIN = 2;
const int SWITCH_2_PIN = 7;

const float DIRECTION_TOWARDS_SWITCH_2 = -1;
const float DIRECTION_TOWARDS_SWITCH_1 = 1;
float _direction = DIRECTION_TOWARDS_SWITCH_2;

void setup()
{  

  if (DEBUG != 0) {
    Serial.begin(9600);
  }

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);

  stepper.setMaxSpeed(ACCEL_STEPPER_MAX_SPEED);

  //Initialize switch1 as Input
  pinMode(SWITCH_1_PIN, INPUT);
  pinMode(SWITCH_2_PIN, INPUT);

  stepper.setSpeed(speedSensorValue);

}

void debug(String text, int value) {
  if (DEBUG == 0) {
    return;
  }
  Serial.print(text);
  Serial.print(value);
  Serial.print("\n");
}

void debug(String text, float value) {
  if (DEBUG == 0) {
    return;
  }
  Serial.print(text);
  Serial.print(value);
  Serial.print("\n");
}


int readSpeedSensorValue() {
  int speed = analogRead(SPEED_SENSOR_PIN);
  return map(speed,0,1023,0,100); // percent will range from -100 to 100.
}

void showOnDisplay(String text, int value) {
  display.setCursor(0,0);
  display.clearDisplay(); 
  display.println(text+value+"%");
  display.display();
}

void changeSpeedTo(int speedPercentage) {
  if (speedPercentage != speedSensorValue) { 
    float maximalSpeed = ACCEL_STEPPER_MAX_SPEED;
    float percentage = (float)speedPercentage / 100;
    //debug("percentage = ", percentage);
    float newSpeed = maximalSpeed * percentage * _direction; 
    debug("newSpeed = ", newSpeed);
    stepper.setSpeed(newSpeed);

    showOnDisplay("Speed = ", speedPercentage);
    speedSensorValue = speedPercentage;
  }
}

float changeDirection() {
  _direction = _direction * -1;
  debug("direction changed:", _direction);
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




