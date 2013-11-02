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

const int DEBUG = 1; // Debug on=1, off=0

const int SPEED_SENSOR_PIN = 0;
int speedSensorValue = 0;

const int SWITCH_1_PIN = 2;

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

  stepper.setMaxSpeed(1024);

  //Initialize switch1 as Input
  pinMode(SWITCH_1_PIN, INPUT);

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


int readSpeedSensorValue() {
  int speed = analogRead(SPEED_SENSOR_PIN);
  return speed - speed % 10; // flatten sensor value
}

void showOnDisplay(String text, int value) {
  display.setCursor(0,0);
  display.clearDisplay(); 
  display.println(text+value);
  display.display();
}

void changeSpeedTo(int speed) {
  if (speed != speedSensorValue) { 
    stepper.setSpeed(speed);
    debug("Speed = ", speed);
    showOnDisplay("Speed = ", speed);
    speedSensorValue = speed;
  }
}

void loop()
{  
  int speed = readSpeedSensorValue();
  int switch1 = digitalRead(SWITCH_1_PIN);
  //debug("Switch1 = ", switch1);
  if (switch1 == HIGH) {
    speed = 0;
  }
  changeSpeedTo(speed);
  stepper.runSpeed();
}


