#include <Arduino.h>
#include "main.h"
#include "line_sensor/line_sensor.h"
#include "Motor/Motor.h"
#include "Motion/motion.h"
#include "Robot/robot.h"
#include "planning/path_planning.h"
#include "servos/servo_action.h"
#include "Analog_Sensor/analog_sensor.h"
#include "EEPROM/EEPROM.h"
//remove later
#include "RangeFind/range_find.h"

#define STD_BY 42

unsigned int values[8];

uint8_t gameMode = 0;

void stopIm() {

	leftMotor.stop();
	rightMotor.stop();
	Serial.println("FALLING");

	while (1) {
		//Serial.println("STOPPED.");
		delay(50);
	}

}

void gpIO() {

	pinMode(22, OUTPUT);
	pinMode(24, OUTPUT);
	pinMode(26, OUTPUT);
	pinMode(28, OUTPUT);

	pinMode(30, INPUT);
	pinMode(32, INPUT);
	pinMode(34, INPUT);
	pinMode(36, INPUT);

	/*
	 *
	 digitalWrite(22, digitalRead(30));
	 digitalWrite(24, digitalRead(34));
	 digitalWrite(26, digitalRead(32));
	 digitalWrite(28, digitalRead(36)); 
	 */

}

const long referenceMv = 5000;
int value;

//Game Mode
int gameStage = 0; //0, dry run, 1 = box search

void beginGame() {

	if (digitalRead(30) == HIGH) {
		gameStage = 0; // dry run.
	} else if (digitalRead(34) == HIGH) {
		gameStage = 1; // box search. and place,
	} else if (digitalRead(32) == HIGH) {
		gameStage = 2; // grab manual's box and place @ pit
	} else if (digitalRead(36) == HIGH) {
		gameStage = 3;
	}

}
int input = 0;
void setup() {

	//s1.attach(11);
	//s2.attach(10);

	pinMode(21, INPUT);
	digitalWrite(21, LOW);

	gpIO();

	initServo();

	initilize_sensors(0);
	loadEEPROM();
	initRanging();

	beginGame();

	Serial.begin(9600);

//	while(!Serial3.available()){
	//	delay(100);
	//}


	//value = Serial.parseInt();

}

void localizeToGrid() {
	robotX = 0;
	robotY = 0;
	robotOrientation = 1;
	if (digitalRead(30) == HIGH) {
		moveForwardOneJunction();
		gameMode = 0;
	} else if (digitalRead(34) == HIGH) {
		moveForwardOneJunction();
		gameMode = 1;
	} else if (digitalRead(32) == HIGH) {
		gameMode = 2;
	}
	stopMotors();
	delay(2000);

}

void grabManualBox() {

	Point tb(0, 2);
	goToNeighbourCoordinate(tb, NAVIGATION_MODE);

	goForwardinSlow(300);
	
	changeOrientation(3);

	correctSensorRotation();
	stopMotors();
	correctSensorRotation();
	correctSensorRotation();
	takeReverse(300);
	//for(int x = 0; x < 2; x++){
	//takeReverse(2000);
	correctSensorRotation();
	stopMotors();
	correctSensorRotation();
	//goForwardinSlow(1500);
	//}
	//correctSensorRotation();
	//correctSensorRotation();

	putArmDown();

	delay(1000);

	while (digitalRead(21) == HIGH) {
		digitalWrite(26, HIGH);
	}
	digitalWrite(26, LOW);
	//putArmDown();

	gripArm();

	delay(500);
	
	liftArmUp();

	Point cur(robotY, robotX);
	Point pit(1, 3);
	initSearch();
	doBFS(cur, pit);

	goToPlannedPath();

	changeOrientation(2);
	
	correctSensorRotation();
	stopMotors();
	correctSensorRotation();
	
	for(int x = 0; x < 2; x++){
		goReverse(2500); // takeRe
			
		//	goForwardinSlow(700);
			leftMotor.setSpeed(0);
			rightMotor.setSpeed(0);
			correctSensorRotation();
			delay(100);
			correctSensorRotation();
			delay(100);
			
			//takeReverse(2000);
			//correctSensorRotation();
			//stopMotors();
			//correctSensorRotation();
			
			goForwardinSlow(2000);
			delay(1000);
	}

	liftServo.write(10);
	delay(500);
	gripServo.write(150);
	delay(500);
	
	liftArmUp();
	delay(500);
	
	/*leftMotor.backward();
	rightMotor.backward();
	
	leftMotor.setSpeed(80);
	rightMotor.setSpeed(62);
	
	leftEncoder.write(0); rightEncoder.write(0);
	
	while(leftEncoder.read() < 2500){
		
	}*/
	
	//gripServo.write(150);
	//delay(500);

	
	
	//liftArmUp();

	
	//initSearch();

}

void goNearToBridge() {

	Point f(robotY, robotX);
	Point t(4, 3);

	initSearch();
	doBFS(f, t);

	goToPlannedPath();
	
	changeOrientation(1);
	

}

void placeBoxForManual() {
	Point placement(0, 3);

	initSearch();
	doBFS(boxPlace, placement);

	goToPlannedPath();
	changeOrientation(3);

	stopMotors();
	//putArmDown();

	liftServo.write(10);
	delay(500);
	gripServo.write(150);

	delay(1000);
	liftArmUp();
	delay(1000);

}

void grabBox() {

	if (boxFound) {

		initSearch();
		Point f(robotY, robotX);

		doBFS(f, boxPlace);

		goToNeighbourOfLast();

		//Serial3.print("Sensor error : ");

		correctSensorRotation();
		stopMotors();
		correctSensorRotation();
		delay(1000);
		//stopMotors();

		putArmDown();

		delay(1000);
		leftMotor.forward();
		leftMotor.setSpeed(80);
		rightMotor.forward();
		rightMotor.setSpeed(62);

		while (digitalRead(21) == HIGH) {
			digitalWrite(24, HIGH);
			leftMotor.setSpeed(80);
			rightMotor.setSpeed(62);
		}
		leftMotor.stop();
		rightMotor.stop();
		stopMotors();

		delay(500);
		gripArm();
		delay(500);
		liftArmUp();
		delay(1000);

		goToNeighbourCoordinate(boxPlace, NAVIGATION_MODE);
		stopMotors();

	}

}

void doRing(){
	
	if(ringNode == 1){
		moveForwardOneJunction();
		turnLeft1();
		moveForwardOneJunction();
	}else{
		turnLeft1();
		moveForwardOneJunction();
		turnRight();
		moveForwardOneJunction();
		turnLeft1();
	}
	
}

void goFromBridge(){
	
	leftMotor.forward();
	rightMotor.forward();
	qtra.readLine(sensorValues, QTR_EMITTERS_ON, 1 );
	while(sensorValues[0] > 500){
		
		doPID2();
		
	}
	
	while(sensorValues[0] < 500){
		doPID2();
	}
	stopMotors();
	turnLeft1();
	moveForwardOneJunction(); // at forward junction
	stopMotors();
	
	//doRing();
	
}



void searchBox() {

	robotY = 0;
	robotX = 0;
	robotOrientation = 1;
	
	leftMotor.forward(); rightMotor.forward();
	leftMotor.setSpeed(60); rightMotor.setSpeed(60);
	
	do{
		
		qtra.readLine(sensorValues, QTR_EMITTERS_ON,1);
	}while(sensorValues[0] > 200 || sensorValues[7] > 200 );
	
	
	//do{
		
		goForwardinSlow(800);
		
	
		
	stopMotors();
	delay(1000);
	digitalWrite(26, HIGH);
	
	
	moveForwardOneJunctionForBoxSearch();

	robotY = 0;
	robotX = 0;
	do {

		Point s(robotY, robotX);
		//	robotOrientation = 1;

		initSearch();
		digitalWrite(22, HIGH);
		doNodeCovering(s);
		digitalWrite(22, LOW);

		//printPlannedPath();

		goToPlannedBoxSearchPath();

	} while (!boxFound);

	//digitalWrite(22, HIGH);

}

void dryRun(){
	
	doSpiralDryRun();
	
	
	Point cur(robotY, robotX);
	Point t(4,3);
	
	initSearch();
	doBFS(cur, t);
	goToPlannedPath();
	changeOrientation(1);
	
	goFromBridge();
	
	robotY = 5; robotX = 0;
	moveForwardOneJunctionForDryRun();
	
	
}

void doGame() {

	if(gameStage == 0) {
		robotX = 0;
		robotY = 0;
		robotOrientation = 1;

		dryRun();
		
		while (1) {
			delay(100);
		}
	} else if (gameStage == 1) {
		robotX = 0;
		robotY = 0;
		robotOrientation = 1;
		//moveForwardOneJunctionForBoxSearch();
		searchBox();
		grabBox();
		placeBoxForManual();
		
		gameStage++;

	}else if(gameStage == 2){
		robotX = 3; robotY = 0;
		robotOrientation = 3;
		
		grabManualBox();
		gameStage++;
		
	}else if(gameStage == 3){
		robotX = 3; robotY = 1;
		robotOrientation = 2;
		goNearToBridge();
		
		goFromBridge();
		doRing();
		
		
		while(1){
			delay(100);
		}
	}

	//turnLeft1();
}

unsigned int sensors[8];
void loop() {

	doGame();
	
	
	
/*	while(!Serial.available()){
		
		value = analogRead(A2);
		Serial.print(value);Serial.print("==");Serial.println(input);
		delay(200);
	}
	input = Serial.parseInt();
	

	delay(100);
	//goFromBridge();
	/*correctSensorRotation();
	correctSensorRotation();
	
	leftMotor.forward();
	rightMotor.forward();
	
	qtra.readLine(sensorValues, QTR_EMITTERS_ON, 1);
	while(qtra.on_line < 5){
		doPIDSlow();
	}*/
	//stopMotors();
	
	//leftMotor.backward(); rightMotor.backward();
	
	//leftEncoder.write(0); rightEncoder.write(0);
	/*while(1){
		
		qtra.readLine(sensorValues, QTR_EMITTERS_ON,1);
		
		for(int i = 0; i < 8; i++){
			Serial.print(sensorValues[i]);
			Serial.print(" - " );
		}
		Serial.println();
		delay(100);
		
		
		
	}*/
	//gripServo.write(150);
}

int main() {
	init();
	setup();

	while (1) {
		loop();
	}

}