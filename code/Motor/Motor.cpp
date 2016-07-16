/* 
 * File:   Motor.cpp
 * Author: Dinesh Madusanke
 * Version: 1.0v
 * 
 * Created on November 29, 2012, 9:13 AM
 */

#include "Motor.h"
#include "Arduino.h"


Motor::Motor(int enable_pin, int in1, int in2) {

    this->enable_pin = enable_pin;
    this->in1 = in1;
    this->in2 = in2;

    pinMode(enable_pin,OUTPUT);
    pinMode(in1, OUTPUT);
    pinMode(in2, OUTPUT);

}

void Motor::start(){

    digitalWrite(enable_pin, HIGH);

}

void Motor :: forward(){
   // start();
	setSpeed(0);
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
}

void Motor :: backward(){
    //start();
	setSpeed(0);
	digitalWrite(in2, HIGH);
    digitalWrite(in1, LOW);
}


void Motor :: stop(){
    digitalWrite(enable_pin, HIGH);
    digitalWrite(in1, HIGH);
    digitalWrite(in2, HIGH);
}

void Motor :: free(){
    digitalWrite(enable_pin, LOW);
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
}


int Motor :: readSpeed(){
    return this->speed;
}

void Motor::setSpeed(int speed){
    analogWrite(enable_pin,speed);
    this->speed = speed;
}
