

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

int timedHours = 0;
int timedMinutes = 0;
int timedSeconds = 0;

int timedLaps = 1;

long numberOfSteps = 0;
float percentageDone = 0;
float displayPercentageDone;

String getDirectionStr() {
	return (_direction == DIRECTION_TOWARDS_SWITCH_2) ? "<-" : "->";
}

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



void doIntializeSlider(float direction) {
	_direction = direction;
	showOnDisplay("Initializing... ", getDirectionStr());
	
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
	showOnDisplay("Initializing... ", getDirectionStr());
	stepper.setSpeed(INIT_SPEED * _direction);
	while ( ! endReached()) {
		if (isCancelButtonPressed()) {
			return;
		}
		stepper.runSpeed(); 
	}
	numberOfSteps = abs(stepper.currentPosition());
	stepper.setSpeed(0);
	changeDirection();	
}


void intializeSlider() {
	doIntializeSlider(DIRECTION_TOWARDS_SWITCH_2);
}

int calculateIncrement(int time) {
	if (time < 10) {
		return 0;
	} else 
	if (time < 1000) {
		return 1;
	} 
	return map(time, 1000, 10000, 2, 10);
}

void enterTimeForTimed() {

	while (!isOkButtonPressed()) {
		int increment;
		long downTime = downButtonTime();
		increment =  calculateIncrement(downTime);
		timedHours=timedHours-increment;
		if (timedHours < 0) {
			timedHours = 99;
		} 
					
		long upTime = upButtonTime();
		increment =  calculateIncrement(upTime);
		timedHours=timedHours+increment;
		if (timedHours >= 100) {
			timedHours = 0;
		} 
		delay(50);
		showTimeInput("Enter Time:", timedHours, timedMinutes, timedSeconds, 0);
	}

	while (!isOkButtonPressed()) {
		long downTime = downButtonTime();
		int increment;
		increment =  calculateIncrement(downTime);
		timedMinutes=timedMinutes-increment;
		if (timedMinutes < 0) {
			timedMinutes = 59;
		} 
					long upTime = upButtonTime();
		increment =  calculateIncrement(upTime);
		timedMinutes=timedMinutes+increment;
		if (timedMinutes >= 60) {
			timedMinutes = 0;
		} 
		delay(50);
		showTimeInput("Enter Time:", timedHours, timedMinutes, timedSeconds, 1);
	}

	while (!isOkButtonPressed()) {
		long downTime = downButtonTime();
		int increment;
		increment =  calculateIncrement(downTime);
		timedSeconds=timedSeconds-increment;
		if (timedSeconds < 0) {
			timedSeconds = 59;
		} 
					long upTime = upButtonTime();
		increment =  calculateIncrement(upTime);
		timedSeconds=timedSeconds+increment;
		if (timedSeconds >= 60) {
			timedSeconds = 0;
		} 
		delay(50);
		showTimeInput("Enter Time:", timedHours, timedMinutes, timedSeconds, 2);
	}
}

void enterDirectionForTimed() {
	while (!isOkButtonPressed()) {
		showOnDisplay("Enter Direction:", getDirectionStr());
		if  (upButtonTime() > 0 || downButtonTime() > 0) { 
			changeDirection();
		}
	}
}

void enterLapsForTimed() {
	while (!isOkButtonPressed()) {
		showIntInput("Enter #Laps:", timedLaps);	
		long downTime = downButtonTime();
		int increment;
		increment =  calculateIncrement(downTime);
		timedSeconds=timedSeconds-increment;
		if (timedSeconds < 0) {
			timedSeconds = 59;
		} 
			
		long upTime = upButtonTime();
		increment =  calculateIncrement(upTime);
		timedSeconds=timedSeconds+increment;
		if (timedSeconds >= 60) {
			timedSeconds = 0;
		} 
	

		if  (isDownButtonPressed()) {
			timedLaps--;
			if (timedLaps < 1) {
				timedLaps = 1;
			} 
		}
		if  (isUpButtonPressed()) {
			timedLaps++;
			if (timedLaps > 99) {
				timedLaps = 99;
			} 
		}
		delay(50);
	}
}



String getTimedDurationStr() {
	String h = ((timedHours < 10) ? "0" : "")+String(timedHours);
	String m = ((timedMinutes < 10) ? "0" : "")+String(timedMinutes);
	String s = ((timedSeconds < 10) ? "0" : "")+String(timedSeconds);
	return h+":"+m+":"+s;
}

void updateDisplay(int lapsToGo) {	
	if (abs(percentageDone - displayPercentageDone) < 5.0) {
		return;
	}; 
	//Serial.print("percentageDone "); Serial.print(percentageDone);
	displayPercentageDone = percentageDone;
	showOnDisplay("T: "+getTimedDurationStr()+" "+getDirectionStr()+" "+String(lapsToGo), "%: "+String((int)displayPercentageDone));	
}
 
void runTimedTimelapse() {
	doIntializeSlider(_direction);
	while(!isOkButtonPressed()) {
		showOnDisplay("Timed: ready", getTimedDurationStr()+" "+getDirectionStr()+" "+String(timedLaps));
		if (isCancelButtonPressed()) return;
	}
	long totalRunSeconds = timedHours * 3600 + timedMinutes * 60 + timedSeconds;
	Serial.print("numberOfSteps: "); Serial.print(numberOfSteps);
	Serial.print("totalRunSeconds"); Serial.print(totalRunSeconds);
	Serial.print("Direction: "); Serial.print(getDirection());
	Serial.print("Laps: "); Serial.print(timedLaps);
	float speed = numberOfSteps / totalRunSeconds * getDirection() * timedLaps;
	Serial.print("Speed: "); Serial.print(speed);
	stepper.setSpeed(speed);
	int lapsToGo = timedLaps;
	showOnDisplay("Timed: running", getTimedDurationStr()+" "+getDirectionStr()+" "+String(lapsToGo));
	long totalNumberOfSteps = timedLaps * numberOfSteps;

	while (! isCancelButtonPressed()) {
		stepper.runSpeed();
		percentageDone = 100.0 - 100.0 * (float)((getDirection() == -1) ? abs(stepper.currentPosition()) : numberOfSteps - abs(stepper.currentPosition()) + (lapsToGo-1) * numberOfSteps)/(float)totalNumberOfSteps;
		//percentageDone = 100.0 * (float)((getDirection() == 1) ? abs(stepper.currentPosition()) : numberOfSteps - abs(stepper.currentPosition()) + (timedLaps - lapsToGo) * numberOfSteps) / (float)totalNumberOfSteps;
		updateDisplay(lapsToGo);
		if (endReached()) {
			lapsToGo--;
			if (lapsToGo == 0) {
				break;
			} else {
				speed = numberOfSteps / totalRunSeconds * changeDirection() * timedLaps;
				stepper.setSpeed(speed);
				showOnDisplay("Timed: running", getTimedDurationStr()+" "+getDirectionStr()+" "+String(lapsToGo));
			}
		}
	}
	while (!(isCancelButtonPressed() || isOkButtonPressed())) {
		showOnDisplay("Timed: finished", getTimedDurationStr()+" "+getDirectionStr()+" "+String(timedLaps));	
	}
}


#endif
