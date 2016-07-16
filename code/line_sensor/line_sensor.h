#ifndef LINE_SENSOR_H_
#define LINE_SENSOR_H_

extern bool sensorReadings[8] ;

extern short pins[8];

extern float error;

extern short on_sensors ;

void initSensors();

void readSensors();

void printValues();

#endif