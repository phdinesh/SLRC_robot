#ifndef RANGE_FIND_H_
#define RANGE_FIND_H_

#include <Arduino.h>
#include "../Ultrasonic/Ultrasonic.h"


extern float ranges[4];

void measureRange();
extern uint8_t directionMap[4][4];

void movingAverageFilter();

void resetRanging();

void calculateAverages();

void checkRangeData();

void initRanging();

unsigned short getSharpDistance(int mV);

 short getCalibratedDistance(int mV);

#endif /*RANGE_FIND_H_*/
