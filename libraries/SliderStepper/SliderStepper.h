

#ifndef SliderStepper_h
#define SliderStepper_h

#include "Arduino.h"
#include <AccelStepper.h>
#include <Wire.h>
#include <Display.h>

#ifndef ProgressBar_h
#include <ProgressBar.h>
#define ProgressBar_h
#endif

#ifndef Config_h
#include <Config.h>
#define Config_h
#endif

#include <ControlButtons.h>

const int CONFIG_ADDRESS = 8192;

//AccelStepper stepper; // Defaults to AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 5
AccelStepper stepper(1,9,8); // 1 = type, 9 = step, 8 = direction
const int ACCEL_STEPPER_MAX_SPEED = 1024;

const int SPEED_SENSOR_PIN = 0;

const int TOM = 1042;


const int SWITCH_1_PIN = 2;
const int SWITCH_2_PIN = 3;

const int FOCUS_PIN = 10;
const int TRIGGER_PIN = 11;


const String SPACE = String(" ");


int speedSensorValue = 0;

config_t configuration;
Config config;


long numberOfSteps = 0;
float percentageDone = 0;
float displayPercentageDone;



void loadSettings() {
    
  int result = config.loadTo(&configuration);
  while (!isOkButtonPressed()) {
  		if (result == 0) {
	    	showOnDisplay(F("Settings loaded."));
  		} else {
	    	showOnDisplay(F("Load failed: "), result);
  		}
  	}
}
  

void saveSettings() {

	int result = config.save(&configuration);
  	while (!isOkButtonPressed()) {
		if (result == 0) {
			showOnDisplay(F("Settings saved."));
	  	} else {
			showOnDisplay(F("Save failed: "), result);
	  	}  
  	}
}



String getDirectionStr() {
	return (configuration._direction == DIRECTION_TOWARDS_SWITCH_2) ? F("<-") : F("->");
}

int readSpeedSensorValue() {
	int speed = analogRead(SPEED_SENSOR_PIN);
	return map(speed,0,1023,0,100);
}

float changeDirection() {
	configuration._direction = configuration._direction * -1;
	return configuration._direction;
}

float getDirection() {
	return configuration._direction;
}


void changeSpeedTo(int speedPercentage) {
	if (speedPercentage != speedSensorValue) { 
		float maximalSpeed = ACCEL_STEPPER_MAX_SPEED;
		float percentage = (float)speedPercentage / 100;
		float newSpeed = maximalSpeed * percentage * configuration._direction; 
		stepper.setSpeed(newSpeed);
		showOnDisplay(F("speed = "), speedPercentage);
		speedSensorValue = speedPercentage;
	}
}


bool endReached() {
	return (
		(getDirection() == DIRECTION_TOWARDS_SWITCH_2 && digitalRead(SWITCH_2_PIN) == HIGH)
		||
		(getDirection() == DIRECTION_TOWARDS_SWITCH_1 && digitalRead(SWITCH_1_PIN) == HIGH)
		);
}



void doIntializeSlider(float direction) {
	configuration._direction = direction;
	showOnDisplay(F("Initializing... "), getDirectionStr());
	
	stepper.setMaxSpeed(configuration.initSpeed); // For faster initialization
	stepper.setSpeed(configuration.initSpeed * configuration._direction);
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
	stepper.setSpeed(configuration.initSpeed * configuration._direction);
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

void enterInitSpeed() {
	while (!isOkButtonPressed()) {
		int increment;
		long downTime = downButtonTime();
		increment = calculateIncrement(downTime);
		configuration.initSpeed = configuration.initSpeed - increment;
		if (configuration.initSpeed < 1000) {
			configuration.initSpeed = 10000;
		}

		long upTime = upButtonTime();
		increment = calculateIncrement(upTime);
		configuration.initSpeed = configuration.initSpeed + increment;
		if (configuration.initSpeed > 10000) {
			configuration.initSpeed = 1000;
		}
		delay(50);
		showIntInput(F("Init Speed: "), configuration.initSpeed);
	} 
}

void enterFramesForTimed() {
	while (!isOkButtonPressed()) {
		int increment;
		long downTime = downButtonTime();
		increment = calculateIncrement(downTime);
		configuration.frames = configuration.frames - increment;
		if (configuration.frames < 0) {
			configuration.frames = 999;
		}

		long upTime = upButtonTime();
		increment = calculateIncrement(upTime);
		configuration.frames = configuration.frames + increment;
		if (configuration.frames > 999) {
			configuration.frames = 0;
		}
		delay(50);
		showIntInput(F("Number of frames: "), configuration.frames);
	} 
}

void enterTimeForTimed() {

	while (!isOkButtonPressed()) {
		int increment;
		long downTime = downButtonTime();
		increment =  calculateIncrement(downTime);
		configuration.timedHours=configuration.timedHours-increment;
		if (configuration.timedHours < 0) {
			configuration.timedHours = 99;
		} 
					
		long upTime = upButtonTime();
		increment =  calculateIncrement(upTime);
		configuration.timedHours=configuration.timedHours+increment;
		if (configuration.timedHours >= 100) {
			configuration.timedHours = 0;
		} 
		delay(50);
		showTimeInput(F("Enter Time:"), configuration.timedHours, configuration.timedMinutes, configuration.timedSeconds, 0);
	}

	while (!isOkButtonPressed()) {
		long downTime = downButtonTime();
		int increment;
		increment =  calculateIncrement(downTime);
		configuration.timedMinutes=configuration.timedMinutes-increment;
		if (configuration.timedMinutes < 0) {
			configuration.timedMinutes = 59;
		} 
					long upTime = upButtonTime();
		increment =  calculateIncrement(upTime);
		configuration.timedMinutes=configuration.timedMinutes+increment;
		if (configuration.timedMinutes >= 60) {
			configuration.timedMinutes = 0;
		} 
		delay(50);
		showTimeInput(F("Enter Time:"), configuration.timedHours, configuration.timedMinutes, configuration.timedSeconds, 1);
	}

	while (!isOkButtonPressed()) {
		long downTime = downButtonTime();
		int increment;
		increment =  calculateIncrement(downTime);
		configuration.timedSeconds=configuration.timedSeconds-increment;
		if (configuration.timedSeconds < 0) {
			configuration.timedSeconds = 59;
		} 
					long upTime = upButtonTime();
		increment =  calculateIncrement(upTime);
		configuration.timedSeconds=configuration.timedSeconds+increment;
		if (configuration.timedSeconds >= 60) {
			configuration.timedSeconds = 0;
		} 
		delay(50);
		showTimeInput(F("Enter Time:"), configuration.timedHours, configuration.timedMinutes, configuration.timedSeconds, 2);
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
		showIntInput(F("Enter #Laps:"), configuration.timedLaps);	
		long downTime = downButtonTime();
		int increment;
		increment =  calculateIncrement(downTime);
		configuration.timedSeconds=configuration.timedSeconds-increment;
		if (configuration.timedSeconds < 0) {
			configuration.timedSeconds = 59;
		} 
			
		long upTime = upButtonTime();
		increment =  calculateIncrement(upTime);
		configuration.timedSeconds=configuration.timedSeconds+increment;
		if (configuration.timedSeconds >= 60) {
			configuration.timedSeconds = 0;
		} 
	

		if  (isDownButtonPressed()) {
			configuration.timedLaps--;
			if (configuration.timedLaps < 1) {
				configuration.timedLaps = 1;
			} 
		}
		if  (isUpButtonPressed()) {
			configuration.timedLaps++;
			if (configuration.timedLaps > 99) {
				configuration.timedLaps = 99;
			} 
		}
		delay(50);
	}
}

void enterBulbForTimed() {
	while (!isOkButtonPressed()) {
		showIntInput(F("Enter Bulb ms:"), configuration.bulbTime);
		long downTime = downButtonTime();
		int increment;
		increment =  calculateIncrement(downTime);
		configuration.bulbTime = configuration.bulbTime - increment;
		if (configuration.bulbTime < 1) {
			configuration.bulbTime = 1;
		}

		long upTime = upButtonTime();
		increment =  calculateIncrement(upTime);
		configuration.bulbTime = configuration.bulbTime + increment;
	}
}



String getTimedDurationStr() {
	const String zero = String("0");
	const String empty = String("");
	String h = ((configuration.timedHours < 10) ? zero : empty)+String(configuration.timedHours);
	String m = ((configuration.timedMinutes < 10) ? zero : empty)+String(configuration.timedMinutes);
	String s = ((configuration.timedSeconds < 10) ? zero : empty)+String(configuration.timedSeconds);
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
	long totalRunSeconds = configuration.timedHours * 3600 + configuration.timedMinutes * 60 + configuration.timedSeconds - (configuration.bulbTime/1000 * configuration.frames);
	if(totalRunSeconds < 0) {
		while(!(isOkButtonPressed() || isCancelButtonPressed())) {
			showOnDisplay(F("Invalid settings --> t < 0"));
		}
		return;
	}

	doIntializeSlider(configuration._direction);
	while(!isOkButtonPressed()) {
		showOnDisplay(F("Timed: ready"), getTimedDurationStr()+SPACE+getDirectionStr()+SPACE+String(configuration.timedLaps));
		if (isCancelButtonPressed()) return;
	}

	float speed = numberOfSteps / totalRunSeconds * getDirection() * configuration.timedLaps;
	stepper.setSpeed(speed);
	int lapsToGo = configuration.timedLaps;
	showOnDisplay(F("Timed: running"), getTimedDurationStr()+SPACE+getDirectionStr()+SPACE+String(lapsToGo));
	long totalNumberOfSteps = configuration.timedLaps * numberOfSteps;

	long positionAtStartOfLap = stepper.currentPosition();
	long stepsRunInLap = 0;
	unsigned long triggerInterval = totalRunSeconds * 1000 / configuration.frames;
	unsigned long triggerMillis  = 0;
	bool triggerPressed = false;
	int lapsCompleted = 0;
	long stepsRun = 0;
	int updateDisplayCount = 10;
	long updateDisplayInterval = totalNumberOfSteps / updateDisplayCount;

	unsigned long currentMillis = 0;
	float currentSpeed;

	while (! isCancelButtonPressed()) {
		stepper.runSpeed();
		if (stepper.currentPosition() > updateDisplayInterval * updateDisplayCount + 1) {
			stepsRunInLap = abs(positionAtStartOfLap - stepper.currentPosition());
			lapsCompleted = configuration.timedLaps - lapsToGo;
			stepsRun = ((long)lapsCompleted * numberOfSteps + stepsRunInLap);
			percentageDone = ((float)stepsRun)/((float)totalNumberOfSteps) * 100.0;
			
			setProgressBar(percentageDone);
		}

		//currentMillis = millis();
		// if (!triggerPressed && (currentMillis - triggerMillis) >= triggerInterval) {
		//  	currentSpeed = stepper.speed();
		//  	stepper.setSpeed(0);
		//  	digitalWrite(TRIGGER_PIN, HIGH);
  //    		triggerMillis = currentMillis;
  //    		triggerPressed = true;
		// }
		// if(triggerPressed && (currentMillis - triggerMillis) >= configuration.bulbTime) {
		//  	digitalWrite(TRIGGER_PIN, LOW);
  //    		triggerMillis = currentMillis;
  //    		triggerPressed = false;	
  //    		updateDisplay(lapsToGo);
  //    		setProgressBar(percentageDone);
  //    		stepper.setSpeed(currentSpeed);
		// }

		if (endReached()) {
			lapsToGo--;
			if (lapsToGo == 0) {
				setProgressBar(100.0);
				break;
			} else {
				positionAtStartOfLap = stepper.currentPosition();
				speed = numberOfSteps / totalRunSeconds * changeDirection() * configuration.timedLaps;
				stepper.setSpeed(speed);
				showOnDisplay(F("Timed: running"), getTimedDurationStr()+SPACE+getDirectionStr()+SPACE+String(lapsToGo));
			}
		}
	}
	while (!(isCancelButtonPressed() || isOkButtonPressed())) {
		showOnDisplay(F("Timed: finished"), getTimedDurationStr()+SPACE+getDirectionStr()+SPACE+String(configuration.timedLaps));	
	}
}


#endif

