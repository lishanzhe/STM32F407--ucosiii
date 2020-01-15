#ifndef __SERVO_H
#define __SERVO_H

#include "sys.h"

#define Servo_Size 4

extern u8 Servo_Hash_Table[];

void Servo_PWM_SetDuty(u8 servox, double duty);
double Servo_PWM_GetDuty(u8 servox);

#endif
