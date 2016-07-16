#include "servo_action.h"

Servo gripServo;
Servo liftServo;

void initServo(){
	
	
	gripServo.attach(10);
	gripServo.write(100);
	
	liftServo.attach(11);
	liftServo.write(100);
}

void writeGripServo(short int angle){
	
	gripServo.write(angle);
	
	
}


void writeLiftServo(short int ang){
	liftServo.write(ang);
	
}

void putArmDown(){
	gripServo.write(150);
	delay(1000);
	liftServo.write(10);
	delay(1000);
	
}

void gripArm(){
	gripServo.write(100);
	delay(1000);
}

void liftArmUp(){
	
	liftServo.write(100);
	delay(1000);
}

