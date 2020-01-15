#include "servo.h"
#include "pwm.h"

u8 Servo_Hash_Table[Servo_Size] = {5, 6, 7, 8};

void Servo_PWM_SetDuty(u8 servox, double duty)
{
	if (servox >= 1 && servox <= Servo_Size)
		PWM_SetDuty(Servo_Hash_Table[servox-1], duty, 1);
}

double Servo_PWM_GetDuty(u8 servox)
{
	if (servox >= 1 && servox <= Servo_Size)
		return PWM_GetDuty(Servo_Hash_Table[servox-1]);
	else
		return 0;
}
