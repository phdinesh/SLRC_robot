#ifndef SERVO_ACTION_H_
#define SERVO_ACTION_H_

#include <Arduino.h>
#include "../Servo/Servo.h"

extern Servo gripServo;
extern Servo grabServo;
extern Servo liftServo;

void initServo();

void writeGripServo(short int angle);

void writeGrabServo(short int angle);
void writeLiftServo(short int ang);

void putArmDown();
void gripArm();
void liftArmUp();

#endif /*SERVO_ACTION_H_*/
