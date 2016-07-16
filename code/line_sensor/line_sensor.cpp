
/*
 * Line sensor readings.
 * 
 */

#include "line_sensor.h"
#include <Arduino.h>

#define SENSOR_SIZE 8
// Chane LINE according to the colour.
#define LINE 0 


bool sensorReadings[8] ;
//short pins[8] = {46,47,48,49,50,51,52,53};
short pins[8] = {36,34,32,30,28,26,24,22};

float error = 0;

short on_sensors = 0;

void initSensors(){
	
	for(int i = 0; i < SENSOR_SIZE; i++){
		pinMode(pins[i], INPUT);
	}
}

short errorVector [] = {-4,-3,-2,-1,1,2,3,4};
void readSensors(){
	on_sensors = 0;
	error = 0;
	for(int i = 0; i < SENSOR_SIZE; i++){
		sensorReadings[i] =  ( digitalRead(pins[i]) == LINE );
		on_sensors += sensorReadings[i];	
		error += sensorReadings[i] * errorVector[i];
	}
	//calculate error and save error
	
	if(on_sensors != 0){
		error = error*1.0/ on_sensors ;
	}
	
	
}

void printValues(){
	for(int i = 0; i < SENSOR_SIZE; i++){
		Serial.print(sensorReadings[i]);
		Serial.print("=");
	}
	Serial.print("=== >"); Serial.println(error);
}