# SLRC_robot
This code is for autobot of Sri Lankan Robotics Competition 2013.
Robot is a differential driven 3 wheeled robot which uses arduino Mega for processing, Pololu line sensor panel, Two DC motors
and 3 IR range sensors and a small gripper with two servo motors. 
Robot has to follow on a grid where some locations are marked as mines which are identified in a test run and
identify and grab some boxes on the arena and hand over it to a remotely controlled bot.

Here is brief summary of packages inside this repo.

planning - A Breadth First Search algorithm implementation for path planning through grid and a sensory coverage based planner.
Motion - PID controllers, and several primitive movement definitions
RangeFind - Handling of IR range find sensors to identify boxes on the areana
line_snsor - basic sensory behaviours for grid following, white line detection, junction detection, mine detection
robot - memory holders for robot related informations
QueueArray - a Queue implementation used in planning algorithm, This is a 3rd party arduino library
QTRSensors - pololu line sensor library with some modifications
Motor - motor control primitives
Encoder - Arduino Encoder library used with Quadrature encoders - 3rd party library


