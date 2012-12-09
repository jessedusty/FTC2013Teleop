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
int currentstage = -2;
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

struct {
	float armAngle;
	float armLength;
	float armBaseRot;
	float wristAngle;
	float wristRotate;
} robotuxPosition;

typedef struct {
	bool armAngle;
	bool armLength;
	bool armBaseRot;
	bool wristAngle;
	bool wristRotate;
} movingDone;

movingDone doneMoving;
robotuxPosition savedPositions[3];

float withinval(float low, float high, float value) {
	if (value < low) value = low;
	if (value > high) value = high;
	return value;
}

int motorPowerCalc(float target, float current, int stopZone, int holdSpeed, int slowSpeed, int fastSpeed)
{
	int slowZone = 1000;
	int holdingZone = stopZone * 4;
	if (target == current) return 0;
	if (abs(target-current) < stopZone) return 0;
	int retval = fastSpeed; //maximum power
	//if (abs(target - current) < slowZone) retval = 1/90 * (target - current);
	if (abs(target - current) < slowZone) retval = slowSpeed;
	if (abs(target - current) < holdingZone) retval = holdSpeed;
	if (target > current) retval = retval * -1;

	return retval;
}
//bool reachedArmAngle;
void armAngleD() {
	//if (rArmAngle
	if (joy2Btn(4)) rArmAngle += 100;
	if (joy2Btn(2)) rArmAngle -= 100;
	if (abs(joystick.joy2_y1) > 10){ rArmAngle += joystick.joy2_y1; currentstage = -2;}
	rArmAngle = withinval(0, 4000, rArmAngle); // 9000
	armAngle = -1 * motorPowerCalc(rArmAngle, cArmAngle, 50, 10, 40, 60);
	motor[ringLifterAngle] = armAngle;
}

void grabberWrist()
{
	if (abs(joystick.joy2_y2) > 10) { rGripperWrist += joystick.joy2_y2; currentstage = -2;}
	rGripperWrist = withinval(0, 4000, rGripperWrist);
	ggripperWrist = motorPowerCalc(rGripperWrist, cGripperWrist, 50, 10, 20, 30);
	motor[gripperWrist] = ggripperWrist;
}

void armLengthD()
{
	if (joystick.joy2_TopHat == 0) { rArmLength += 80; currentstage = -2;}
	if (joystick.joy2_TopHat == 4) { rArmLength -= 80; currentstage = -2;}

	rArmLength = withinval(0, 4000, rArmLength);
	motor[ringLifterLength] = -1 * motorPowerCalc(rArmLength, cArmLength, 50, 10, 20, 30);
}

void armRotateD()
{
	if (abs(joystick.joy2_x1) > 10) {rArmBaseRot += joystick.joy2_x1; currentstage = -2;}
	//rArmBaseRot = withinval(0, 4000, rArmBaseRot);
	motor[armPivotor] = -1 * motorPowerCalc(rArmBaseRot, cArmBaseRot, 50, 10, 30, 60);
}

void endOfArmServos()
{
	if(joy2Btn(5)) servo[whiteGripper] = 123;
	if(joy2Btn(6)) servo[orangeGripper] = 123;
	if(joy2Btn(7)) servo[whiteGripper] = 5;
	if(joy2Btn(8)) servo[orangeGripper] = 5;
	if(abs(joystick.joy2_x2) > 10) { gripperarmrotate += joystick.joy2_x2; currentstage = -2;}
	servo[rotateGripper] = gripperarmrotate;
}

void doneMovingTo () {
doneMoving.wristAngle = (abs(rGripperWrist - cGripperWrist) > 50) ? true : false;
doneMoving.armAngle = (abs(rArmAngle - cArmAngle) > 50) ? true : false;
doneMoving.armLength = (abs(rArmLength - cArmLength) > 50) ? true : false;
doneMoving.armBaseRot = (abs(rArmBaseRot - cArmBaseRot) > 50) ? true : false;
doneMoving.wristRotate = (abs(gripperarmrotate - servo[rotateGripper]) > 50) ? true : false;
}


void updatecurrentstage() {
currentstage = ((currentstage == -1) & doneMoving.armLength & (rArmLength == 0)) ? 0 : currentstage;
currentstage = ((currentstage == 0) & doneMoving.armAngle) ? 1 : currentstage;
currentstage = ((currentstage == 1) & doneMoving.armBaseRot) ? 2 : currentstage;
currentstage = ((currentstage == 2) & doneMoving.armAngle) ? 3 : currentstage;
currentstage = ((currentstage == 3) & doneMoving.wristAngle) ? 4 : currentstage;
currentstage = ((currentstage == 4) & doneMoving.wristRotate) ? 5 : currentstage;
currentstage = ((currentstage == 5) & doneMoving.armLength) ? 6 : currentstage;
}

void gotoposition (int position, int type) {
	if (currentstage != -2) {
		if (type == 1) robotuxPosition &p = savedPositions[position];
		doneMovingTo();
		switch(currentstage) {
		case -1: rArmLength = 0; break;
		case 0:	rArmAngle = (rArmAngle > 2000) ? rArmAngle : 2000; break;
		case 1:	rArmBaseRot = p.armBaseRot;	break;
		case 2:	rArmAngle = p.armAngle; break;
		case 3:	rGripperWrist = p.wristAngle;	break;
		case 4:	gripperarmrotate = p.wristRotate;	break;
		case 5:	rArmLength = p.armLength; break;
		default: currentstage = -2; break;
		}

	}
}

void positionSaving () {

}

void accessoryControl()
{
	grabberWrist();
	armAngleD();
	armLengthD();
	armRotateD();
	endOfArmServos();
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
