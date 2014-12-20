

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

const int FOCUS_PIN = 10;
const int TRIGGER_PIN = 3;

const float DIRECTION_TOWARDS_SWITCH_2 = -1;
const float DIRECTION_TOWARDS_SWITCH_1 = 1;


const String SPACE = String(" ");

int speedSensorValue = 0;
float _direction = DIRECTION_TOWARDS_SWITCH_2;

int timedHours = 0;
int timedMinutes = 0;
int timedSeconds = 0;

int timedLaps = 1;

int frames = 0;
long bulbTime = 1000;


long numberOfSteps = 0;
float percentageDone = 0;
float displayPercentageDone;

String getDirectionStr() {
	return (_direction == DIRECTION_TOWARDS_SWITCH_2) ? F("<-") : F("->");
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
		showOnDisplay(F("speed = "), speedPercentage);
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
	showOnDisplay(F("Initializing... "), getDirectionStr());
	
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
	showOnDisplay(F("Initializing... "), getDirectionStr());
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
	return map(time, 1000, 10000, 2, 50);
}

void enterFramesForTimed() {
	while (!isOkButtonPressed()) {
		int increment;
		long downTime = downButtonTime();
		increment = calculateIncrement(downTime);
		frames = frames - increment;
		if (frames < 0) {
			frames = 999;
		}

		long upTime = upButtonTime();
		increment = calculateIncrement(upTime);
		frames = frames + increment;
		if (frames > 999) {
			frames = 0;
		}
		delay(50);
		showIntInput(F("Number of frames: "), frames);
	} 
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
		showTimeInput(F("Enter Time:"), timedHours, timedMinutes, timedSeconds, 0);
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
		showTimeInput(F("Enter Time:"), timedHours, timedMinutes, timedSeconds, 1);
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
		showTimeInput(F("Enter Time:"), timedHours, timedMinutes, timedSeconds, 2);
	}
}

void enterDirectionForTimed() {
	while (!isOkButtonPressed()) {
		showOnDisplay(F("Enter Direction:"), getDirectionStr());
		if  (upButtonTime() > 0 || downButtonTime() > 0) { 
			changeDirection();
		}
	}
}

void enterLapsForTimed() {
	while (!isOkButtonPressed()) {
		showIntInput(F("Enter #Laps:"), timedLaps);	
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

void enterBulbForTimed() {
	while (!isOkButtonPressed()) {
		showIntInput(F("Enter Bulb ms:"), bulbTime);
		long downTime = downButtonTime();
		int increment;
		increment =  calculateIncrement(downTime);
		bulbTime = bulbTime - increment;
		if (bulbTime < 1) {
			bulbTime = 1;
		}

		long upTime = upButtonTime();
		increment =  calculateIncrement(upTime);
		bulbTime = bulbTime + increment;
	}
}



String getTimedDurationStr() {
	const String zero = String("0");
	const String empty = String("");
	String h = ((timedHours < 10) ? zero : empty)+String(timedHours);
	String m = ((timedMinutes < 10) ? zero : empty)+String(timedMinutes);
	String s = ((timedSeconds < 10) ? zero : empty)+String(timedSeconds);
	return h+":"+m+":"+s;
}

void updateDisplay(int lapsToGo) {	
	if (abs(percentageDone - displayPercentageDone) < 5.0) {
		return;
	}; 
	Serial.print(F("percentageDone ")); Serial.print(percentageDone);
	displayPercentageDone = percentageDone;
	showOnDisplay("T: "+getTimedDurationStr()+SPACE+getDirectionStr()+SPACE+String(lapsToGo), "%: "+String((int)displayPercentageDone));	
}


 
void runTimedTimelapse() {
	long totalRunSeconds = timedHours * 3600 + timedMinutes * 60 + timedSeconds - (bulbTime/1000 * frames);
	if(totalRunSeconds < 0) {
		while(!(isOkButtonPressed() || isCancelButtonPressed())) {
			showOnDisplay(F("Invalid settings --> t < 0"));
		}
		return;
	}

	doIntializeSlider(_direction);
	while(!isOkButtonPressed()) {
		showOnDisplay(F("Timed: ready"), getTimedDurationStr()+SPACE+getDirectionStr()+SPACE+String(timedLaps));
		if (isCancelButtonPressed()) return;
	}

	float speed = numberOfSteps / totalRunSeconds * getDirection() * timedLaps;
	stepper.setSpeed(speed);
	int lapsToGo = timedLaps;
	showOnDisplay(F("Timed: running"), getTimedDurationStr()+SPACE+getDirectionStr()+SPACE+String(lapsToGo));
	long totalNumberOfSteps = timedLaps * numberOfSteps;

	long positionAtStartOfLap = stepper.currentPosition();
	long stepsRunInLap = 0;
	long triggerInterval = totalRunSeconds * 1000 / frames;
	long triggerMillis  = 0;
	bool triggerPressed = false;
	int lapsCompleted = 0;
	long stepsRun = 0;
	while (! isCancelButtonPressed()) {
		stepper.runSpeed();
		stepsRunInLap = abs(positionAtStartOfLap - stepper.currentPosition());
		lapsCompleted = timedLaps - lapsToGo;
		stepsRun = ((long)lapsCompleted * numberOfSteps + stepsRunInLap);
		percentageDone = ((float)stepsRun)/((float)totalNumberOfSteps) * 100.0;
		float currentSpeed;
		if (!triggerPressed && ((unsigned long)(millis() - triggerMillis) >= triggerInterval)) {
			currentSpeed = stepper.speed();
			stepper.setSpeed(0);
			digitalWrite(TRIGGER_PIN, HIGH);
    		triggerMillis = millis();
    		triggerPressed = true;
		}
		if(triggerPressed && ((unsigned long)(millis() - triggerMillis) >= bulbTime)) {
			digitalWrite(TRIGGER_PIN, LOW);
    		triggerMillis = millis();
    		triggerPressed = false;	
    		updateDisplay(lapsToGo);
    		stepper.setSpeed(currentSpeed);
		}
		if (endReached()) {
			lapsToGo--;
			if (lapsToGo == 0) {
				break;
			} else {
				positionAtStartOfLap = stepper.currentPosition();
				speed = numberOfSteps / totalRunSeconds * changeDirection() * timedLaps;
				stepper.setSpeed(speed);
				showOnDisplay(F("Timed: running"), getTimedDurationStr()+SPACE+getDirectionStr()+SPACE+String(lapsToGo));
			}
		}
	}
	while (!(isCancelButtonPressed() || isOkButtonPressed())) {
		showOnDisplay(F("Timed: finished"), getTimedDurationStr()+SPACE+getDirectionStr()+SPACE+String(timedLaps));	
	}
}


#endif

