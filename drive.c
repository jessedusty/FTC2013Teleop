#pragma config(Hubs,  S1, HTMotor,  HTMotor,  HTMotor,  HTServo)
#pragma config(Sensor, S1,     ,               sensorI2CMuxController)
#pragma config(Motor,  mtr_S1_C1_1,     ringLifterAngle, tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C1_2,     ringLifterLength, tmotorTetrix, openLoop, encoder)
#pragma config(Motor,  mtr_S1_C2_1,     armPivotor,    tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C2_2,     gripperWrist,  tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C3_1,     leftDrive,     tmotorTetrix, openLoop, reversed)
#pragma config(Motor,  mtr_S1_C3_2,     rightDrive,    tmotorTetrix, openLoop)
#pragma config(Servo,  srvo_S1_C4_4,    rotateGripper,        tServoStandard)
#pragma config(Servo,  srvo_S1_C4_5,    whiteGripper,         tServoStandard)
#pragma config(Servo,  srvo_S1_C4_6,    orangeGripper,        tServoStandard)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

//!!Code automatically generated by 'ROBOTC' configuration wizard               !!

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
// values set throughout the program

float leftstickval = 0;
float rightstickval = 0;
float newrightstickval = 0;

// global speeds of drive motors
float leftmotorval = 0;
float rightmotorval = 0;

// target positions for extremities, lengths in inches, angles in degrees
float armLength, armAngle, armBaseRot;
float rArmLength, rArmAngle, rArmBaseRot;
float cArmLength, cArmAngle, cArmBaseRot;

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

int xThreshhold = 20; // threshhold on the joystick for movement
int yThreshhold = 15;
int rotorAcc = 40; // clicks to not worry about accuracy

int rotorSpeedCalc(float target, float current) {
	if (target == current) return 0;
	int rotorSlow = 20; // accuracy tuning for rotor
	int rotorSlowRange = 500;
	int rotorFast = 70; // max slewing speed

	int returnval = rotorFast;

	int direction;

	if (target < current) { direction = -1; } else { direction = 1; }

	if (abs(target-current) < rotorSlowRange) returnval = (abs(target-current) / 50) + 10;

	returnval = returnval * direction;

	return returnval;
}


int angleSpeedCalc(float target, float current) {
	if (rArmAngle == cArmAngle) return 0;
	int rotorSlow = 20; // accuracy tuning for rotor
	int rotorSlowRange = 500;
	int rotorFast = 70; // max slewing speed

	int returnval = rotorFast;

	int direction;

	if (rArmAngle < cArmAngle) { direction = -1; } else { direction = 1; }

	if (abs(rArmAngle - cArmAngle) < rotorSlowRange) returnval = rotorSlow;

	returnval = returnval * direction;

	return returnval;
}

float degtoenco (float deg) {
	return deg * 40;
}

float leveljoystick (float joystickval) {
	float returnval = joystickval * 0.78125;
	return returnval;
}

float rsave1, rsave2;
void driverotator(float joystickval) {

	if (joy2Btn(9)) {
		if (joy2Btn(1)) rsave1 = cArmBaseRot;
		if (joy2Btn(3)) rsave2 = cArmBaseRot;
	} else {
		if (joy2Btn(1)) {
			rArmBaseRot = rsave1;
			driverslew = false;
		} else if (joy2Btn(3)) {
			rArmBaseRot = rsave2;
			driverslew = false;
		}
	}

	if (abs(joystickval) > xThreshhold) {

		motor[armPivotor] = -1 * joystickval;
		rArmBaseRot = cArmBaseRot;
		driverslew = true;

	} else if (abs(rArmBaseRot-cArmBaseRot) > rotorAcc) {
		if (!driverslew) motor[armPivotor] = rotorSpeedCalc(rArmBaseRot, cArmBaseRot);
		// continue moving motors
	} else {
		motor[armPivotor] = 0;

	}
}
float ahsave1, ahsave2;
void driveArmHeight(float joystickval) {

	if (joy2Btn(9)) {
		if (joy2Btn(1)) ahsave1 = cArmAngle;
		if (joy2Btn(3)) ahsave2 = cArmAngle;
	} else {
		if (joy2Btn(1)) {
			rArmAngle = ahsave1;
			driverslew = false;
		} else if (joy2Btn(3)) {
			rArmAngle = ahsave2;
			driverslew = false;
		}
	}

	if (abs(joystickval) > yThreshhold) {

		motor[ringLifterAngle] = -1 * joystickval;
		rArmAngle = cArmAngle;
		driverslew = true;

	} else if (abs(rArmAngle-cArmAngle) > rotorAcc) {
		if (!driverslew) motor[ringLifterAngle] = angleSpeedCalc(rArmAngle, cArmAngle);
		// continue moving motors
	} else {
		motor[ringLifterAngle] = 0;

	}
}





void mainaccessory () {
	driverotator(leveljoystick(joystick.joy2_x1));
	driveArmHeight(joystick.joy2_y1);
	//driveArmLength(joystick.joy2_y2);

   // add accesory stuff here
}

// motor manager takes in the target positon in inches of extremities
// and converts those values into instructions for the motor, including the ramping of motors

void motorManager () {

}


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
	cArmLength = 0;
	cArmAngle = 0;
	cArmBaseRot = 0;

	nMotorEncoder[leftDrive] = 0;
	nMotorEncoder[rightDrive] = 0;
	nMotorEncoder[armPivotor] = 0;
	nMotorEncoder[robotLifter] = 0;
	nMotorEncoder[ringLifterLength] = 0;
	nMotorEncoder[ringLifterAngle] = 0;
}

void runLoopPause() {
	while (time1[T1] < 50) {
		wait1Msec(1);
	}
}

void GetNewEncoderVals() {
	cArmBaseRot = nMotorEncoder[armPivotor];
	cArmLength = nMotorEncoder[ringLifterLength];
	cArmAngle = nMotorEncoder[ringLifterAngle];
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
    mainaccessory();
    batterycheck();
    //powercontrol();

    motorManager();

    runLoopPause();

  }
}
