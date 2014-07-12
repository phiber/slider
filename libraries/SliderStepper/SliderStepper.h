

#ifndef SliderStepper_h
#define SliderStepper_h

#include "Arduino.h"
#include <AccelStepper.h>
#include <Wire.h>
#include <Display.h>
#include <ControlButtons.h>

//AccelStepper stepper; // Defaults to AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 5
AccelStepper stepper(1,9,8); // 1 = type, 9 = step, 8 = direction
const int ACCEL_STEPPER_MAX_SPEED = 1024;
const int INIT_SPEED = 6000; // Stepper speed for initialization

const int SPEED_SENSOR_PIN = 0;

const int SWITCH_1_PIN = 2;
const int SWITCH_2_PIN = 7;

const float DIRECTION_TOWARDS_SWITCH_2 = -1;
const float DIRECTION_TOWARDS_SWITCH_1 = 1;

int speedSensorValue = 0;
float _direction = DIRECTION_TOWARDS_SWITCH_2;


int readSpeedSensorValue() {
	int speed = analogRead(SPEED_SENSOR_PIN);
	return map(speed,0,1023,0,100);
}

float changeDirection() {
	_direction = _direction * -1;
	return _direction;
}

float getDirection() {
	return _direction;
}


void changeSpeedTo(int speedPercentage) {
	if (speedPercentage != speedSensorValue) { 
		float maximalSpeed = ACCEL_STEPPER_MAX_SPEED;
		float percentage = (float)speedPercentage / 100;
		float newSpeed = maximalSpeed * percentage * _direction; 
		stepper.setSpeed(newSpeed);
		showOnDisplay("speed = ", speedPercentage);
		speedSensorValue = speedPercentage;
	}
}


bool endReached() {
	int switch1 = digitalRead(SWITCH_1_PIN);
	int switch2 = digitalRead(SWITCH_2_PIN);
	return (
		(getDirection() == DIRECTION_TOWARDS_SWITCH_2 && switch2 == HIGH)
		||
		(getDirection() == DIRECTION_TOWARDS_SWITCH_1 && switch1 == HIGH)
		);
}



void intializeSlider() {
	showOnDisplay("Initializing... ", "To switch 2");
	_direction = DIRECTION_TOWARDS_SWITCH_2;
	stepper.setMaxSpeed(INIT_SPEED); // For faster initialization
	stepper.setSpeed(INIT_SPEED * _direction);
	while ( ! endReached()) {
		if (isCancelButtonPressed()) {
			return;
		}
		stepper.runSpeed();
	}
	stepper.setCurrentPosition(0);
	stepper.setSpeed(0);
	changeDirection();
	showOnDisplay("Initializing... ", "To switch 1");
	stepper.setSpeed(INIT_SPEED * _direction);
	while ( ! endReached()) {
		if (isCancelButtonPressed()) {
			return;
		}
		stepper.runSpeed();
	}
	stepper.setSpeed(0);
	changeDirection();	
}


#endif

