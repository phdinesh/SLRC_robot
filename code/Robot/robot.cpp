
#include "robot.h"
#include "../Point/Point.h"

short int robotX = 0;
short int robotY = 0;
short int robotOrientation = 1;
short GRID_SIZE_X = 4;
short GRID_SIZE_Y = 5;

short xDirections[] = {1,-1,0,0};
short yDirections[] = {0,0,1,-1};

bool edgeData[] = {1,1,1,1,
				   0,1,1,1,
				   1,0,0,1,
				   1,1,1,1,
				   1,1,0,0};

 bool boxFound = 0;

Point boxPlace(0,0);

bool isABoxPoint(Point check){
	if(check.x < 0 || check.x >= GRID_SIZE_X){
		return 0;
	}
	if(check.y < 0 || check.y >= GRID_SIZE_Y){
		return 0;
	}
	return edgeData[check.y * GRID_SIZE_X + check.x] == 1;
	
}