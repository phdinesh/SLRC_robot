#ifndef MODULES_H_
#define MODULES_H_

#include "../Motor/Motor.h"
#include "../Encoder/Encoder.h"

extern Motor leftMotor;

extern Motor rightMotor;

extern unsigned int sensorValues[8];

extern Encoder leftEncoder;

extern Encoder rightEncoder;

void moveForwardOneJunction();

int moveForwardDistance(int count);

void moveBackwardDistance(int count);

void activeBreak();

void correctRotation();

short int findBox();

void turnRight();

void moveForwardOneJunctionForDryRun();

void stopMotors();

void adjustWheels();

extern unsigned int left_breaking;

extern unsigned int right_breaking;
	
void doPID2();

void doPIDSlow() ;

void takeReverse(int count);

void doReversePID(); 

void goForwardinSlow(int count);

void turnLeft();
void turnLeft1();
void turnLeftSensors();

void goReverse(int count);

void reverseEncoderPID();

void turnBack();

void adjustWheels();

void correctSensorRotation();


void moveForwardOneJunctionForBoxSearch();

void doPID();


#endif /*MODULES_H_*/
