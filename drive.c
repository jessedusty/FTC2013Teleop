#pragma config(Hubs,  S1, HTMotor,  HTMotor,  HTMotor,  HTServo)
#pragma config(Sensor, S1,     ,               sensorI2CMuxController)
#pragma config(Motor,  mtr_S1_C1_1,     ringLifterAngle, tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C1_2,     ringLifterLength, tmotorTetrix, openLoop, encoder)
#pragma config(Motor,  mtr_S1_C2_1,     armPivotor,    tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C2_2,     gripperWrist,  tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C3_1,     leftDrive,     tmotorTetrix, openLoop, reversed)
#pragma config(Motor,  mtr_S1_C3_2,     rightDrive,    tmotorTetrix, openLoop)
#pragma config(Servo,  srvo_S1_C4_1,    servo1,               tServoNone)
#pragma config(Servo,  srvo_S1_C4_2,    servo2,               tServoNone)
#pragma config(Servo,  srvo_S1_C4_3,    servo3,               tServoNone)
#pragma config(Servo,  srvo_S1_C4_4,    rotateGripper,        tServoStandard)
#pragma config(Servo,  srvo_S1_C4_5,    whiteGripper,         tServoStandard)
#pragma config(Servo,  srvo_S1_C4_6,    orangeGripper,        tServoStandard)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

// THIS IS THE MAIN DRIVE CODE - FOR USE IN TELEOP, THIS FILE HAS BEEN CREATED ON Nov 25th, 2012


#include "JoystickDriver.c" // ABSOLUTLY CRUTIAL FOR DRIVING WITH A JOYSTICK

// VD - varible declarations
int rev = 1; // 1 = not reversed | -1 means reversed
bool normalctl = true; // normal joystick configuration if true, reverse joystick configuration if false
int power = 1; // the number that joystick is divided by to give
int forwardbackwardthresh = 10; // this is the threshold for the left joystick in leftmode
int leftrightthresh = 10; // this is the threshhold for the right joystick in left mode
bool leftmode = true; //leftmode is when up/down is on left and left/right is on right
bool rightmode = false; // rightmode the reverse of leftmode
bool isRunning = true; // specifies wether the robot is not in E-STOP mode - will stop all robot actions if set to false
int quadrent = 0;
bool driverslew = true;

bool heighthold = false;
// values set throughout the program

float leftstickval = 0;
float rightstickval = 0;
float newrightstickval = 0;


// global speeds of drive motors
float leftmotorval = 0;
float rightmotorval = 0;

// target positions for extremities, lengths in inches, angles in degrees
float armLength, armAngle, armBaseRot, ggripperWrist;
float rArmLength, rArmAngle, rArmBaseRot, rGripperWrist;
float cArmLength, cArmAngle, cArmBaseRot, cGripperWrist;
float gripperarmrotate;

// end of "global" varible declaration

// make sure that the values going to the motor arn't less than -100 or greater than 100
float makesureitsinlimits(float input) {
	if (input > 100) input = 100;
	if (input < -100) input = -100;
	return input;
}

// driving joystick (the one that actually moves the robot's position)

void joyval_joystick1st() { // standard mode on left joystick

	leftstickval = pow(((float)joystick.joy1_y1 / (float)64 * (float)5), 2) ;
	if (joystick.joy1_y1 < 0) leftstickval = leftstickval * -1.0;

	rightstickval = pow(((float)joystick.joy1_x1 / (float)64 * (float)5), 2) ;
	if (joystick.joy1_x1 < 0) rightstickval = rightstickval * -1.0;

}

void joyval_joystick1rv() { // reverse mode on left joystick
	leftstickval = pow(((float)joystick.joy1_y2 / (float)64 * (float)5), 2) ;
	if (joystick.joy1_y2 < 0) leftstickval = leftstickval * -1.0;

	rightstickval = pow(((float)joystick.joy1_x1 / (float)64 * (float)5), 2) ;
	if (joystick.joy1_x1 < 0) rightstickval = rightstickval * -1.0;
}

void joyval_joystick2st() { // standard mode on right joystick
	leftstickval = pow(((float)joystick.joy2_y1 / (float)64 * (float)5), 2) ;
	if (joystick.joy2_y1 < 0) leftstickval = leftstickval * -1.0;

	rightstickval = pow(((float)joystick.joy2_x2 / (float)64 * (float)5), 2) ;
	if (joystick.joy2_x2 < 0) rightstickval = rightstickval * -1.0;
}

void joyval_joystick2rv() { // reverse mode on right joystick
	leftstickval = pow(((float)joystick.joy2_y2 / (float)64 * (float)5), 2) ;
	if (joystick.joy2_y2 < 0) leftstickval = leftstickval * -1.0;

	rightstickval = pow(((float)joystick.joy2_x1 / (float)64 * (float)5), 2) ;
	if (joystick.joy2_x1 < 0) rightstickval = rightstickval * -1.0;
}

//discover the values for use in the driving routine (using above methods)

void joyval_correct() {
	if ((normalctl) & (rev == 1)) {
		joyval_joystick1st();
		} else if ((normalctl) & (rev == -1)) {
		joyval_joystick1rv();
		} else if ((!normalctl) & (rev == 1)) {
		joyval_joystick2st();
		} else if ((!normalctl) & (rev == -1)) {
		joyval_joystick2rv();
		} else {
		PlayImmediateTone(500, 1);
	}
	leftstickval = leftstickval * rev;
	if (abs(leftstickval) < forwardbackwardthresh) leftstickval = 0;
	//rightstickval = rightstickval * -1.0
	if (abs(rightstickval) < leftrightthresh) rightstickval = 0;
}

float whichismax(float left, float right) {
	float returnval;
	if (left > right) returnval = left;
	if (right > left) returnval = right;
	if (right == left) returnval = (left + right) / 2;
	return returnval;
}

//MTR - move the motors, where the main drive motors are driven
void movethemotors() {
	motor[leftDrive] = leftmotorval;
	motor[rightDrive] = rightmotorval;
}

//DJ - where the main driving happens (it calls the other stuff above)
void driving_joystick() {
	joyval_correct(); //put the correct joystick values in their varibles
	if (leftstickval > 0.0) {
		if (rightstickval > 0.0) {
			quadrent = 1;
			leftmotorval = leftstickval - rightstickval;
			rightmotorval = whichismax(leftstickval, rightstickval);
			} else {
			quadrent = 2;
			leftmotorval = whichismax(leftstickval, (rightstickval * -1.0));
			rightmotorval = leftstickval + rightstickval;
		}
		} else {
		if (rightstickval > 0.0) {
			quadrent = 3;
			leftmotorval = (whichismax((leftstickval * -1.0), rightstickval)) * -1.0;
			rightmotorval = leftstickval + rightstickval;
			} else {
			quadrent = 4;
			leftmotorval = leftstickval - rightstickval;
			rightmotorval = (whichismax((leftstickval * -1.0), (rightstickval * -1.0))) * -1.0;
		}
	}

	movethemotors();
}

// power control (used for slowing down driving motors to get greater accuracy)

void powercontrol () {
	if (normalctl) {
		if (joy1Btn(1)) power = 1;
		if (joy1Btn(2)) power = 2;
		if (joy1Btn(3)) power = 3;
		if (joy1Btn(4)) power = 4;
		} else {
		if (joy2Btn(1)) power = 1;
		if (joy2Btn(2)) power = 2;
		if (joy2Btn(3)) power = 3;
		if (joy2Btn(4)) power = 4;
	}
}

// accesory controler contols
// edited by Oren the Awesome

#define slowZone 1000
#define holdingZone 300
#define stopZone 200

float withinval(float low, float high, float value) {
	if (value < low) value = low;
	if (value > high) value = high;
	return value;
}
int motorPowerCalc(float target, float current)
{
	if (target == current) return 0;
	if (abs(target-current) < stopZone) return 0;
	int retval = 30; //maximum power
	//if (abs(target - current) < slowZone) retval = 1/90 * (target - current);
	if (abs(target - current) < slowZone) retval = 20;
	if (abs(target - current) < holdingZone) retval = 10;
	if (target > current) retval = retval * -1;

	return retval;
}
void armAngleD() {
	if (joy2Btn(4)) rArmAngle += 100;
	if (joy2Btn(2)) rArmAngle -= 100;
	if (abs(joystick.joy2_y1) > 10){
		rArmAngle += joystick.joy2_y1;
		}
	rArmAngle = withinval(0, 4000, rArmAngle); // 9000
	armAngle = -1 * motorPowerCalc(rArmAngle, cArmAngle);
	motor[ringLifterAngle] = armAngle;
}

void grabberWrist()
{
	if (abs(joystick.joy2_y2) > 10){
		rGripperWrist += joystick.joy2_y2;
		} else {
		//rGripperWrist = cGripperWrist;
	}
	rGripperWrist = withinval(0, 4000, rGripperWrist);
	ggripperWrist = motorPowerCalc(rGripperWrist, cGripperWrist);
	motor[gripperWrist] = ggripperWrist;

}


void accessoryControl()
{
	grabberWrist();
	armAngleD();
}

// motor manager takes in the target positon in inches of extremities
// and converts those values into instructions for the motor, including the ramping of motors

// BC - battery check
void batterycheck () {
	if (externalBattery == -1) PlayImmediateTone(4000, 1);
}

void resetencodersandvals () {
	armLength = 0;
	armAngle = 0;
	armBaseRot = 0;
	rArmLength = 0;
	rArmAngle = 0;
	rArmBaseRot = 0;
	rGripperWrist = 0;
	cArmLength = 0;
	cArmAngle = 0;
	cArmBaseRot = 0;
	cGripperWrist = 0;

	nMotorEncoder[leftDrive] = 0;
	nMotorEncoder[rightDrive] = 0;
	nMotorEncoder[armPivotor] = 0;
	nMotorEncoder[gripperWrist] = 0;
	nMotorEncoder[ringLifterLength] = 0;
	nMotorEncoder[ringLifterAngle] = 0;
}

void runLoopPause() {
	while (time1[T1] < 50) {
		wait1Msec(1);
	}
	time1[T1] = 0;
}

void GetNewEncoderVals() {
	cArmBaseRot = nMotorEncoder[armPivotor];
	cArmLength = nMotorEncoder[ringLifterLength];
	cArmAngle = nMotorEncoder[ringLifterAngle];
	cGripperWrist = nMotorEncoder[gripperWrist];
}


// TMT this is the main thread code
task main() {
	waitForStart();
	isRunning = true; // sets isRunning to true, just in case it gets set to false
	time1[T1] = 0;
	resetencodersandvals();
	while (isRunning) {
		getJoystickSettings(joystick);
		GetNewEncoderVals();

		driving_joystick();
		accessoryControl();
		batterycheck();


		//powercontrol();

		runLoopPause();

	}
}
