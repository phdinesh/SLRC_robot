
#include "analog_sensor.h"
#include "../QTRSensors/QTRSensors.h"
#include "../Motion/Motion.h"

unsigned char pinOuts[8] = {8, 9, 10, 11, 12, 13,14,15};

QTRSensorsAnalog qtra(pinOuts ,8, 4,QTR_NO_EMITTER_PIN);



void initilize_sensors(bool load){
	
	
		qtra.calibrate(QTR_EMITTERS_ON);
		

}


