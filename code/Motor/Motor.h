/* 
 * File:   Motor.h
 * Author: Dinesh Madusanke
 *
 * Created on November 29, 2012, 9:13 AM
 */

#ifndef MOTOR_H
#define	MOTOR_H


class Motor{
public:
    Motor(int enable_pin, int in1, int in2);
    void start();
    void forward();
    void backward();
    void stop();
    void free();
    void setSpeed(int speed);
    int readSpeed();
    
private:
    int enable_pin;
    int in1;
    int in2;
    int speed;
   
};

#endif	/* MOTOR_H */

