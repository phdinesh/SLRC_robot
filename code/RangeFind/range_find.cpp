#include "range_find.h"
#include "../QueueArray/QueueArray.h"
#include "../Robot/robot.h"
#include <Arduino.h>

#define TABLE_SIZE 12
#define FIRST_ELEMENT 250
#define INTERVAL 250

#define MAX_SAMPLES 500

static unsigned short distanceTable[] = { 150, 140, 130, 100, 60, 50, 40, 35,
		30, 25, 20, 15 };


//samples...
static unsigned short samples = 13;
static unsigned short calibratedDistances[] = {130,120,110,100,90,80,70,60,50,40,30,20,12};

static int calibratedVoltages[] = {33, 157, 338,561, 656,768,824,968,1181,
		1460, 1883, 2452, 2653};

//0==> front sensor, 1 --> Left, 2--> RIGHT
unsigned short distances[3][MAX_SAMPLES]; // 
unsigned short counters[3]= { 0, 0, 0 }; // respective sample counter
unsigned short sharpPins[] = { A2, A0, A1 }; // front , left, right

float averages[3];

float ranges[4]; // +x, -x, +y, -y

uint8_t adjacentNodes[4]; // x,-x,y,-y




const long referenceMv = 5000;

//Ex: direction = 1, x = 0 --> sensor is 1
uint8_t directionMap[4][4] = {{1,2,0,3}, 
		                      {0,3,2,1},
		                      {2,1,3,0},
		                      {3,0,1,2}};



void initRanging() {
	pinMode(A1, INPUT);
	pinMode(A0, INPUT);
	pinMode(A2, INPUT);

}

void resetRanging(){
	counters[0] = counters[1] = counters[2] = 0;
}

void measureRange() {
	uint8_t k = 0;
	for (k = 1; k < 3; k++) {
		int value = ( analogRead(sharpPins[k]) * referenceMv)/1023;
		
		distances[k][counters[k]] = getCalibratedDistance(value);
		Serial.println(getCalibratedDistance(value));
		//.Serial.print(value);Serial.print(",");Serial.print(distances[k][counters[k]]);
		//Serial.print("*-----*");
		if(distances[k][counters[k]] < 120){
			counters[k]++;
			counters[k] >= MAX_SAMPLES ? 0 : counters[k];
			//Serial.println();
		}
	}
	
}

void movingAverageFilter(){
	uint8_t k = 0;
	unsigned short j,n;
	unsigned short value;
	for(k = 1; k < 3; k++){
		for(j = 0; j < counters[k] - 10; j++){
			value = 0;
			for(n = 0; n < 10; n++){
				value += distances[k][j+n];
			}
			value = value / 10;
			distances[k][j] = value;
		}
		
	}
}

uint8_t convertDistanceToNodes(float dist){
	
	if(5 < dist && dist < 30){
		return 1;
		
	}else if(35 < dist && dist < 70){
		return 2;
	}else if(70 < dist && dist < 95){
		return 10;
	}else if(100 < dist && dist < 120){
		return 10;
	}else{
		return 10;
	}
}

//ranges array data should be updated. before calling.
void checkRangeData(){
	
	Point p;
	Serial3.println("Adjacent data ... ");
	for(uint8_t c = 0; c < 4; c++){
		
		adjacentNodes[c] = convertDistanceToNodes(ranges[c]);
		p.y = robotY  + adjacentNodes[c] * yDirections[c]; p.x = robotX + adjacentNodes[c]*xDirections[c];
		Serial3.print("== ");
		Serial3.println(adjacentNodes[c]);
		
		if( isABoxPoint(p) ){
			boxFound = 1;
			boxPlace.y = p.y;
			boxPlace.x = p.x;
			Serial3.print("Robot at : (");Serial3.print(robotY); Serial3.print(", ");
			Serial3.print(robotX); Serial3.print(" ==== ");
			
			Serial3.print("Found...@ ");Serial3.print(boxPlace.y);Serial3.print(",");
			Serial3.println(boxPlace.x);
		}
		
		
	}
	
}



void calculateAverages() {
	uint8_t k = 0;
	unsigned short j;
	for (k = 1; k < 3; k++) {
		averages[k] = 0;
		
		Serial3.print("Values for sensor ");
		Serial3.println(k);
		for (j = 0; j < counters[k]; j++) {
			averages[k] += distances[k][j];
			Serial3.print(distances[k][k]);
			Serial3.print(" , ");
		}
		Serial3.println();
		if(counters[k] > 0){
			averages[k] = averages[k]/ counters[k];
			//Serial3.print("( c : - ");Serial3.print(counters[k]);
		}else{
			averages[k] = 0;
		}
	}
	Serial3.println();

	for(k = 0; k < 4; k++){ // fill according to distances...
		ranges[k] = -5;
		j = directionMap[robotOrientation - 1][k];
		if(j < 3){
			ranges[k] = averages[j] ;
		}
	}//
	
}

short getCalibratedDistance(int mV){
	float fraction = 0;
	float value;
	for(int c = 0; c < samples-1; c++){
		if(mV >= calibratedVoltages[c] && mV < calibratedVoltages[c+1]){
			fraction = (1.0*(mV-calibratedVoltages[c])) /(calibratedVoltages[c+1] - calibratedVoltages[c]) ;
		value =  calibratedDistances[c];
		value += fraction * (calibratedDistances[c] - calibratedDistances[c+1]);
		//Serial.print(value);Serial.print(" = ");
		return (short)value;
			//return (calibratedDistances[c] + (fraction * (calibratedDistances[c] - calibratedDistances[c + 1])));
		}
	}
	
	if(mV > calibratedVoltages[samples - 1]){
		return 10;
	}else{
		return 140;
	}
}


unsigned short getSharpDistance(int mV) {

	if (mV > INTERVAL * TABLE_SIZE) {

		return distanceTable[TABLE_SIZE - 1];
	}

	short index = mV / INTERVAL;

	float fraction = (mV % INTERVAL)/ (float)INTERVAL;

	return (distanceTable[index] - (distanceTable[index]
			- distanceTable[index + 1]) * fraction);

}