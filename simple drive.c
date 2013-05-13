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
#pragma config(Servo,  srvo_S1_C4_5,    orangeGripper,        tServoStandard)
#pragma config(Servo,  srvo_S1_C4_6,    whiteGripper,         tServoStandard)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//


#include "JoystickDriver.c" // ABSOLUTLY CRUTIAL FOR DRIVING WITH A JOYSTICK


task main()
{
	while (true) {
		getJoystickSettings(joystick);
		motor[ringLifterAngle] = joystick.joy1_y1;
		motor[armPivotor] = joystick.joy1_y2;
		motor[leftDrive] = joystick.joy2_y1;
		motor[rightDrive] = joystick.joy2_y2;
}



}
