#include "memory_saved.h"
#include "../robot/robot.h"
#include <Arduino.h>
#include "../EEPROM/EEPROM.h"



void saveDryRunNode(bool isValid){
	
	Serial3.print(robotY); Serial3.print(",");Serial3.print(robotX);Serial3.print("==>");
	Serial3.println(isValid);
	EEPROM.write(GRID_SIZE_X * robotY + robotX, isValid);
	
	
}

void loadNodeData(bool validityData[]){
	unsigned short node_counter;
	
	for(node_counter = 0; node_counter < GRID_SIZE_Y * GRID_SIZE_X; node_counter++){
		validityData[node_counter] = EEPROM.read(node_counter);
	}
}