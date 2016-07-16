#include "dry_run.h"
#include "../Motion/motion.h"
#include "../Robot/robot.h"

// Note : robot is at 0,0 at orientation of 1,


void doDryRun(){

	short c, y,x;
	
/*   c c c c
 *   c c c c
 *   c c c c
 *   c c c c
 */
	//0,0
	x = 0;
	for(c = 1; c < 4; c++){
		robotX = x; robotY = c;
		moveForwardOneJunctionForDryRun();
	}
	
	turnRight();
	
	//0,3
	y = 3;
	for(c = 1; c < 4; c++){
		robotX = c; robotY = y;
		moveForwardOneJunctionForDryRun();
	}
	turnRight();
	
	//3,3
	x = 3;
	for(c = 2; c >=0 ; c--){
		robotX = x; robotY = c;
		moveForwardOneJunctionForDryRun();
	}
	turnRight();
	
	//3,0
	y = 0;
	for(c = 2; c >= 1; c--  ){
		robotX = c; robotY = y;
		moveForwardOneJunctionForDryRun();
	}
	turnRight();
	//1,0

	/*   c c c c
	 *   c c c c
	 *   c c c c
	 *   c c c c
	 */
	x = 1;
	for(c = 1; c <=2; c++){
		robotX = x;
		robotY = c;
		moveForwardOneJunctionForDryRun();
		
	}
	//1,2
	turnRight();
	
	y = 2;
	for(c = 2; c <=2; c++){
			robotX = c;
			robotY = y;
			moveForwardOneJunctionForDryRun();
			
	}
	//2,2
	turnRight();
	x = 2;
	y = 1;
	moveForwardOneJunctionForDryRun();
	
	
	
	
	
}

