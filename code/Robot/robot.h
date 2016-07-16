#ifndef ROBOT_H_
#define ROBOT_H_

#include "../Point/Point.h"

extern short int robotX;
extern short int robotY;
extern short int robotOrientation;

extern short GRID_SIZE_X ;
extern short GRID_SIZE_Y ;

extern short xDirections[] ;//= {1,-1,0,0};
extern short yDirections[] ;//= {0,0,1,-1};

extern bool edgeData[20];

extern bool boxFound;

bool isABoxPoint(Point check);

extern Point boxPlace;

#endif /*ROBOT_H_*/
