#include "motion.h"
#include "../line_sensor/line_sensor.h"
#include "../planning/path_planning.h"
#include "../RangeFind/range_find.h"
#include "../Robot/Robot.h"
#include <Arduino.h>
#include "../Analog_Sensor/analog_sensor.h"
#include "../Memory/memory_saved.h"

#define LBREAK 1030
#define RBREAK 1030

#define WHITE_LIMIT 300
#define COLOUR_LIMIT 500

#define BLACK_LIMIT 700

//breaking distances.

unsigned int left_breaking;
unsigned int right_breaking;
//end of

//Analog sensors
unsigned int sensorValues[8];

//

#define STOP_DELAY 50
//46,44,42,40,38
//8,9
Motor rightMotor(8, 52, 50);

Motor leftMotor(9, 48, 46);
//28,29 echo, trigger.

Encoder rightEncoder(2, 2);

Encoder leftEncoder(3, 3);

/*
 int base_speed = 175;
 //int p, d = 0, i = 0;
 float r_speed, l_speed;
 int min_speed = 95, max_speed = 255;
 //int min_speed = 120, max_speed = 180;
 float local_error = 0;
 int test;
 //28,15
 //20,10
 //25,10
 //30
 float p[] = { 25, 15 };
 float dp[] = { 0, 0 };

 void doPID() {
 //test = 9;

 local_error = error;
 readSensors();

 if (on_sensors == 0) {

 if (local_error < 0) {
 while (on_sensors == 0) {
 readSensors();
 rightMotor.setSpeed(120);
 leftMotor.setSpeed(0);
 }
 } else {
 while (on_sensors == 0) {
 readSensors();
 rightMotor.setSpeed(0);
 leftMotor.setSpeed(120);
 }

 }
 leftMotor.setSpeed(min_speed);
 rightMotor.setSpeed(min_speed);
 } else {

 l_speed = base_speed + p[0] * error + p[1] * (local_error - error);
 r_speed = base_speed - p[0] * error - p[1] * (local_error - error);

 if (r_speed < min_speed) {
 r_speed = min_speed;
 }
 if (r_speed > max_speed) {
 r_speed = max_speed;
 }
 if (l_speed < min_speed) {
 l_speed = min_speed;
 }
 if (l_speed > max_speed) {
 l_speed = max_speed;
 }
 leftMotor.setSpeed(l_speed);
 rightMotor.setSpeed(r_speed);
 //Serial.print("error : ");Serial.println(error);
 //Serial.print(l_speed); Serial.print("--");Serial.println(r_speed);
 }
 }
 */
//analog line follow


//bases 150,132
int line_error;
int previous_error;
//1,35 ok , 
//float parameters[] = { 2.5, 45.0 };

float parameters[] = { 4, 60.0 };
int speedl, speedr;
//Base speed
int base = 150, min = 80, max = 220;
int base1 = 132, min1 = 62, max1 = 220;

//int base = 120, min = 80, max = 160;
//int base1 = 102, min1 = 62, max1 = 160;

void doPID2() {

	//	qtra.calibrate(QTR_EMITTERS_ON);
	previous_error = line_error;

	line_error = qtra.readLine(sensorValues, QTR_EMITTERS_ON, 1);
	line_error = line_error - 3500; // what ever the value ok.

	line_error = line_error / 100; //scale factor.

	speedl = base + parameters[0] * line_error + parameters[1] * (line_error
			- previous_error);
	speedr = base1 - parameters[0] * line_error - parameters[1] * (line_error
			- previous_error);

	if (speedl < min ) {
		speedl = min;
	}
	if (speedl > max ) {
		speedl = max;
	}
	if (speedr < min1) {
		speedr = min1;
	}

	if (speedr > max1) {
		speedr = max1;
	}

	leftMotor.setSpeed(speedl);
	rightMotor.setSpeed(speedr);

}

//end


void stopLeftMotor() {
	unsigned int past_left = 0;
	//leftEncoder.write(0);
	left_breaking = 0;
	leftMotor.stop();
	do {
		past_left = left_breaking;
		//leftEncoder.write(0);
		delay(STOP_DELAY);
		left_breaking = leftEncoder.read();
	} while ((left_breaking != past_left));

}

void stopRightMotor() {

	unsigned int past_right = 0;
	//rightEncoder.write(0);
	right_breaking = 0;
	rightMotor.stop();
	do {
		past_right = right_breaking;
		//leftEncoder.write(0);
		delay(STOP_DELAY);
		right_breaking = rightEncoder.read();
	} while ((right_breaking != past_right));
}

void stopMotors() {
	leftEncoder.write(0);
	rightEncoder.write(0);

	leftMotor.stop();
	rightMotor.stop();
	stopLeftMotor();
	stopRightMotor();
}

// Basic motion functions

//Move forward a one junction and stop wheels at the junction.
void moveForwardOneJunction() {
	//Serial.println("Move One Junction.");
	leftMotor.forward();
	rightMotor.forward();
	qtra.readLine(sensorValues, QTR_EMITTERS_ON, 1);
	while (sensorValues[0] > COLOUR_LIMIT || sensorValues[7] > COLOUR_LIMIT) {
		doPID2();
	}

	while (sensorValues[0] < COLOUR_LIMIT && sensorValues[7] < COLOUR_LIMIT) {
		doPID2();
	}

	//moveForwardDistance(2000);
	activeBreak();
}

// Dry run move function.

void moveForwardOneJunctionForDryRun() {

	unsigned int blacks = 0;

	//Serial.println("Move One Junction.");
	leftMotor.forward();
	rightMotor.forward();
	qtra.readLine(sensorValues, QTR_EMITTERS_ON, 1);
	while (sensorValues[0] > COLOUR_LIMIT || sensorValues[7] > COLOUR_LIMIT) {
		doPID2();
	}

	while (sensorValues[0] < COLOUR_LIMIT && sensorValues[7] < COLOUR_LIMIT) {
		doPID2();
		if ( (sensorValues[0] < WHITE_LIMIT && sensorValues[7] < WHITE_LIMIT)) {

			if ((sensorValues[3] > BLACK_LIMIT && sensorValues[4] > BLACK_LIMIT)
					|| (sensorValues[2] > BLACK_LIMIT && sensorValues[3]
							> BLACK_LIMIT) || (sensorValues[5] > BLACK_LIMIT
					&& sensorValues[4] > BLACK_LIMIT)) {
				blacks++;

			}

			/*Serial3.print("robot @ "); Serial3.print(robotY);Serial3.print(", ");Serial3.println(robotX);
			 for(int c = 0; c < 8; c++){
			 Serial3.print(sensorValues[c]); Serial3.print(" = ");
			 }
			 Serial3.println();*/

		}

	}

	//moveForwardDistance(2000);

	activeBreak();
	saveDryRunNode((blacks == 0));
	if (blacks > 0) {
		digitalWrite(22, HIGH);

		//delay(1000);
	} else {
		digitalWrite(22, LOW);
	}
}
//Moving forward for box searching....


void moveForwardOneJunctionForBoxSearch() {

	//Serial.println("Move One Junction.");
	leftMotor.forward();
	rightMotor.forward();
	qtra.readLine(sensorValues, QTR_EMITTERS_ON, 1);

	resetRanging(); // ready for new ranging...
	while (sensorValues[0] > COLOUR_LIMIT || sensorValues[7] > COLOUR_LIMIT) {
		doPID2();
		//measureRange(); // range
	}

	while (sensorValues[0] < COLOUR_LIMIT && sensorValues[7] < COLOUR_LIMIT) {
		measureRange();
		doPID2();
	}

	//moveForwardDistance(2000);


	Serial3.print("( ");
	Serial3.print(robotY);
	Serial3.print(", ");
	Serial3.print(robotX);
	Serial3.print(" ) ");

	Serial3.print("Orientation : ");
	Serial3.print(robotOrientation);
	Serial3.println("==>");
	
	activeBreak();
	
	calculateAverages();
	
	//movingAverageFilter();

	checkRangeData();

	
Serial3.println("===");

	Serial3.print(ranges[0]);
	Serial3.print(",");
	Serial3.print(ranges[1]);
	Serial3.print(",");
	Serial3.print(ranges[2]);
	Serial3.print(",");
	Serial3.println(ranges[3]);

	Serial3.println("======");
	
	
}

//


void correctSensorRotation() {
	line_error = qtra.readLine(sensorValues, QTR_EMITTERS_ON, 1)-3500;
	//Serial3.print("error : ");
	//Serial3.println(line_error);

	//	Serial.print("Sensor values : ");
	for (int c = 0; c < 8; c++) {
		//	Serial.print(sensorValues[c]);
		//Serial.print(", ");
	}
	//Serial.println();

	if (line_error < 0) {
		//Serial3.print("Negative : Line ERror: ");
		//Serial3.println(line_error);
		leftMotor.backward();
		leftMotor.setSpeed(60);
		rightMotor.forward();
		rightMotor.setSpeed(60);

		while (line_error < -1000) {
			line_error = qtra.readLine(sensorValues, QTR_EMITTERS_ON, 1) - 3500;
			//Serial3.println(line_error);
		}
		leftMotor.stop();
		rightMotor.stop();

	} else if (line_error > 0) {
		//Serial3.print("positive : Line ERror: ");
		//Serial3.println(line_error);

		leftMotor.forward();
		leftMotor.setSpeed(80);
		rightMotor.backward();
		rightMotor.setSpeed(80);

		while (line_error > 1000) {
			line_error = qtra.readLine(sensorValues, QTR_EMITTERS_ON, 1) - 3500;
			//Serial3.println(line_error);
		}
		leftMotor.stop();
		rightMotor.stop();
	}

}

//


//

unsigned char all_blacked = 0;
unsigned char retaken = 0;

void turnLeft1() {
	//Serial3.println("Left Turn.");
	stopMotors();
	//adjustWheels();

	//delay(1000);

	all_blacked = 0;
	retaken = 0;

	rightMotor.forward();

	rightMotor.setSpeed(70);//62
	leftMotor.backward();
	leftMotor.setSpeed(80);
	leftEncoder.write(0);

	rightEncoder.write(0);

	while (rightEncoder.read() < 1200 || leftEncoder.read() < 1200) {
		qtra.readLine(sensorValues, QTR_EMITTERS_ON, 1);

		if (qtra.on_line == 0 && retaken == 0) {
			all_blacked = 1;
			//Serial3.println("all_blacked occured");

		}
		//Serial3.print("ON LINE :");
		//Serial3.println(qtra.on_line);
		qtra.readLine(sensorValues, QTR_EMITTERS_ON, 1);
		if (all_blacked == 1 && qtra.on_line != 0) {
			retaken = 1;
			//Serial3.println("Retaken...");
			//Serial3.print(" ===== ");
			//Serial3.println(qtra.on_line);
		}

		if (rightEncoder.read() > 1200) {
			rightMotor.stop();
		}
		if (leftEncoder.read() > 1200) {
			leftMotor.stop();

		}

		delayMicroseconds(100);
	}

	rightMotor.forward();
	rightMotor.setSpeed(70);//62

	leftMotor.backward();

	leftMotor.setSpeed(80);

	while (all_blacked == 0 || retaken == 0) {
		//Serial3.println("In while...");
		qtra.readLine(sensorValues, QTR_EMITTERS_ON, 1);
		if (qtra.on_line == 0 && retaken == 0) {
			all_blacked = 1;
			//Serial3.println("in while all_blacked occured");

		}
		qtra.readLine(sensorValues, QTR_EMITTERS_ON, 1);

		if (all_blacked == 1 && qtra.on_line != 0) {
			retaken = 1;
			//			Serial3.println("in while Retaken...");
			///		Serial3.print(" ===== ");
			//	Serial3.println(qtra.on_line);
		}
	}
	rightMotor.stop();
	leftMotor.stop();
	rightMotor.stop();

	//correctRotation();

	//delay(1000);

	correctSensorRotation();

	stopMotors();
	//stopLeftMotor();
	//stopRightMotor();

}

//turn right

void turnRight() {

	stopMotors();

	rightMotor.backward();
	rightMotor.setSpeed(70);//70
	leftMotor.forward();
	leftMotor.setSpeed(70);//70

	leftEncoder.write(0);
	rightEncoder.write(0);

	all_blacked = 0;
	retaken = 0;

	while (rightEncoder.read() < 1200 || leftEncoder.read() < 1200) {
		qtra.readLine(sensorValues, QTR_EMITTERS_ON, 1);

		if (qtra.on_line == 0 && retaken == 0) {
			all_blacked = 1;
			///	Serial3.println("all_blacked occured");

		}
		//Serial3.print("ON LINE :");
		//Serial3.println(qtra.on_line);
		qtra.readLine(sensorValues, QTR_EMITTERS_ON, 1);
		if (all_blacked == 1 && sensorValues[7] < WHITE_LIMIT) {
			retaken = 1;
			//Serial3.println("Retaken...");
			//Serial3.print(" ===== ");
			//Serial3.println(qtra.on_line);
		}

		if (rightEncoder.read() > 1200) {
			rightMotor.stop();
		}
		if (leftEncoder.read() > 1200) {
			leftMotor.stop();

		}

		delayMicroseconds(100);
	}

	//respeed.
	rightMotor.backward();
	rightMotor.setSpeed(70);
	leftMotor.forward();
	leftMotor.setSpeed(70);
	///////////

	while (all_blacked == 0 || retaken == 0) {
		// Serial3.println("In while...");
		qtra.readLine(sensorValues, QTR_EMITTERS_ON, 1);
		if (qtra.on_line == 0 && retaken == 0) {
			all_blacked = 1;
			//Serial3.println("in while all_blacked occured");

		}
		qtra.readLine(sensorValues, QTR_EMITTERS_ON, 1);

		if (all_blacked == 1 && sensorValues[7] < WHITE_LIMIT) {
			retaken = 1;
			//Serial3.println("in while Retaken...");
			//Serial3.print(" ===== ");
			//Serial3.println(qtra.on_line);
		}
	}

	rightMotor.stop();
	leftMotor.stop();
	//rightMotor.stop();

	//correctRotation();

	//delay(1000);

	correctSensorRotation();
	stopMotors();
	//correctSensorRotation();

	//stopLeftMotor();
	//stopRightMotor();

}

//Turn correction


//turn right


// diameter = 62 mm , 3100 = pi * d = 195 mm = 3100 cuts
//

int moveForwardDistance(int count) {
	//leftMotor.forward();
	//leftMotor.setSpeed(150);
	leftEncoder.write(0);
	while (leftEncoder.read() < count) {
		doPID();
	}
	leftMotor.stop();
	rightMotor.stop();
	return leftEncoder.read();

}

void moveBackwardDistance(int count) {
	leftMotor.backward();
	leftMotor.setSpeed(150);
	rightMotor.backward();
	rightMotor.setSpeed(150);

	rightEncoder.write(0);

	while (rightEncoder.read() < count) {
		Serial3.println(rightEncoder.read());

	}
	leftMotor.stop();
	rightMotor.stop();
	leftMotor.forward();
	rightMotor.forward();
	leftMotor.setSpeed(0);
	rightMotor.setSpeed(0);
}

void turnBack() {

	stopMotors();
	rightMotor.backward();
	rightMotor.setSpeed(80);

	leftMotor.forward();
	leftMotor.setSpeed(80);

	leftEncoder.write(0);
	rightEncoder.write(0);

	all_blacked = 0;
	retaken = 0;

	while (rightEncoder.read() < 3000 || leftEncoder.read() < 3000) {
		qtra.readLine(sensorValues, QTR_EMITTERS_ON, 1);

		if (qtra.on_line == 0 && retaken == 0) {
			all_blacked = 1;
		}

		if (all_blacked == 1 && qtra.on_line != 0) {
			retaken = 1;
		}

		qtra.readLine(sensorValues, QTR_EMITTERS_ON, 1);
		if (qtra.on_line == 0 && retaken == 1) {
			all_blacked = 2;
		}

		if (all_blacked == 2 && qtra.on_line != 0) {
			retaken = 2;
		}

		if (rightEncoder.read() > 3000) {
			rightMotor.stop();
		}
		if (leftEncoder.read() > 3000) {
			leftMotor.stop();

		}

		delayMicroseconds(100);
	}

	//respeeding...
	rightMotor.backward();
	rightMotor.setSpeed(80);

	leftMotor.forward();
	leftMotor.setSpeed(80);

	//
	while (all_blacked != 2 || retaken != 2) {
		qtra.readLine(sensorValues, QTR_EMITTERS_ON, 1);

		if (qtra.on_line == 0 && retaken == 0) {
			all_blacked = 1;
		}

		if (all_blacked == 1 && qtra.on_line != 0) {
			retaken = 1;
		}

		qtra.readLine(sensorValues, QTR_EMITTERS_ON, 1);
		if (qtra.on_line == 0 && retaken == 1) {
			all_blacked = 2;
		}

		if (all_blacked == 2 && qtra.on_line != 0) {
			retaken = 2;
		}

	}

	rightMotor.stop();
	leftMotor.stop();
	correctSensorRotation();
	stopMotors();

	//stopLeftMotor();
	//stopRightMotor();

}
//

void moveLeftWheel(int count) {
	Serial3.print("L : ");
	Serial3.println(count);
	leftEncoder.write(0);
	if (count < 0) {
		leftMotor.backward();
		count = -count;
	} else {
		leftMotor.forward();
	}
	leftMotor.setSpeed(80);
	while (leftEncoder.read() < count) {
		delay(100);
	}
	Serial3.println(leftEncoder.read());
	leftMotor.stop();
	delay(100);
	leftMotor.forward();

}
// move right wheel

void moveRightWheel(int count) {

	Serial3.print("R : ");
	Serial3.println(count);
	rightEncoder.write(0);
	if (count < 0) {
		rightMotor.backward();
		count = -count;
	} else {
		rightMotor.forward();
	}
	rightMotor.setSpeed(80);
	while (rightEncoder.read() < count) {
		delay(100);
	}
	rightMotor.stop();
	delay(100);
	rightMotor.forward();

}

//adjusting for junction
void adjustWheels() {
	if (left_breaking > right_breaking) {
		Serial3.print("Moving left : ");
		Serial3.println(right_breaking - left_breaking);
		Serial3.println("==");
		moveLeftWheel(right_breaking - left_breaking);
	} else {
		Serial3.print("Moving Right : ");
		Serial3.println(left_breaking - right_breaking);
		Serial3.println("==");
		moveRightWheel(left_breaking - right_breaking);
	}
}

// start of the box finding..


void activeBreak() {
	//for(int i = 0; i < 1000; i++){
	leftMotor.stop();
	rightMotor.stop();
	//delay(1);
	//}
}

//////////////////////////////////////////////////// Alternative PID functions
/////////////////////////////////////////////////////////
float parameters_s[] = { 4, 60.0 };
int speedl_s, speedr_s;
//Base speed
int base_s = 60, min_s = 40, max_s = 80;
int base1_s = 60, min1_s = 40, max1_s = 80;

//int base = 120, min = 80, max = 160;
//int base1 = 102, min1 = 62, max1 = 160;

int previous_error_s, line_error_s;

void doPIDSlow() {

	//	qtra.calibrate(QTR_EMITTERS_ON);
	previous_error_s = line_error_s;

	line_error_s = qtra.readLine(sensorValues, QTR_EMITTERS_ON, 1);
	line_error_s = line_error_s - 3500; // what ever the value ok.

	line_error_s = line_error_s / 100; //scale factor.

	speedl_s = base_s + parameters_s[0] * line_error_s + parameters_s[1]
			* (line_error_s - previous_error_s);
	speedr_s = base1_s - parameters_s[0] * line_error_s - parameters_s[1]
			* (line_error_s - previous_error_s);

	if (speedl_s < min_s) {
		speedl_s = min_s;
	}
	if (speedl_s > max_s) {
		speedl_s = max_s;
	}
	if (speedr_s < min1_s) {
		speedr_s = min1_s;
	}

	if (speedr_s > max1_s) {
		speedr_s = max1_s;
	}

	leftMotor.setSpeed(speedl_s);
	rightMotor.setSpeed(speedr_s);

}
//////////////////

float parameters_r[] = { 5, 30 };
int speedl_r, speedr_r;
//Base speed
int base_r = 60, min_r = 40, max_r = 70;
int base1_r = 60, min1_r = 40, max1_r = 80;

//int base = 120, min = 80, max = 160;
//int base1 = 102, min1 = 62, max1 = 160;

int previous_error_r, line_error_r;

void doReversePID() {

	//	qtra.calibrate(QTR_EMITTERS_ON);
	previous_error_r = line_error_r;

	line_error_r = qtra.readLine(sensorValues, QTR_EMITTERS_ON, 1);
	line_error_r = line_error_r - 3500; // what ever the value ok.

	line_error_r = line_error_r / 100; //scale factor.

	speedl_r = base_r + parameters_r[0] * line_error_r + parameters_r[1]
			* (line_error_r - previous_error_r);
	speedr_r = base1_r - parameters_r[0] * line_error_r - parameters_r[1]
			* (line_error_r - previous_error_r);

	if (speedl_r < min_r) {
		speedl_r = min_r;
	}
	if (speedl_r > max_r) {
		speedl_r = max_r;
	}
	if (speedr_r < min1_r) {
		speedr_r = min1_r;
	}

	if (speedr_r > max1_r) {
		speedr_r = max1_r;
	}

	leftMotor.setSpeed(speedr_r);
	rightMotor.setSpeed(speedl_r);

	/*if(line_error_r > 0){
	 leftMotor.setSpeed(0);
	 rightMotor.setSpeed(80);
	 }else if(line_error_r < 0){
	 rightMotor.setSpeed(0);
	 leftMotor.setSpeed(80);
	 }else {
	 leftMotor.setSpeed(80);
	 rightMotor.setSpeed(80);
	 }*/
}

void goForwardinSlow(int count) {

	leftEncoder.write(0);
	rightEncoder.write(0);
	leftEncoder.write(0);

	leftMotor.forward();
	rightMotor.forward();

	while (leftEncoder.read() < count) {
		doPIDSlow();
	}
	leftMotor.stop();
	rightMotor.stop();

}

void takeReverse(int count) {
	//leftMotor.backward();
	//rightMotor.backward();
	leftEncoder.write(0);
	rightEncoder.write(0);

	leftMotor.backward();
	rightMotor.backward();
	leftMotor.setSpeed(70);
	rightMotor.setSpeed(80);

	while (leftEncoder.read() < count) {
		delay(1);
	}
	leftMotor.stop();
	rightMotor.stop();

}

void goReverse(int count) {
	
	leftEncoder.write(0);
	rightEncoder.write(0);
	
	leftMotor.backward();
	rightMotor.backward();
	while(leftEncoder.read() < count){
		reverseEncoderPID();
		
	}
	activeBreak();
	
	
	//stopMotors();
}

//////////////////////
void goDistanceSlowly(int distance) {
	leftEncoder.write(0);
	rightEncoder.write(0);

	while (leftEncoder.read() < distance) {
		//	doPID
	}

}

/////////////////////////////
int mid1 = 80, low1 = 60, high1 = 100;
int mid2 = 50, low2 = 30, high2 = 80;

int leftSpeedEn, rightSpeedEn;

int encoderError = 0;

float encodeParams[] = { 10, 5 };

void reverseEncoderPID() {

	encoderError = leftEncoder.read() - rightEncoder.read();

	//if(encoderError < 0){
	leftSpeedEn = mid1 - encodeParams[0] * encoderError;
	rightSpeedEn = mid2 + encodeParams[0] * encoderError;

	if (leftSpeedEn < low1) {
		leftSpeedEn = low1;
	}
	if (leftSpeedEn > high1) {
		leftSpeedEn = high1;
	}

	if (rightSpeedEn < low2) {
		rightSpeedEn = low2;
	}
	if (rightSpeedEn > high2) {
		rightSpeedEn = high2;
	}

	leftMotor.setSpeed(leftSpeedEn);
	rightMotor.setSpeed(rightSpeedEn);
}

