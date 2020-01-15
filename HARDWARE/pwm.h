#ifndef __PWM_H
#define __PWM_H

#include "sys.h"

typedef struct
{
	u32 clk_freq;
	u32 pwm_freq;
	u8  tim_num;
	u8  chx;
	u8  gpiox;
	u8  pinx;
	double duty_init;
}PWM_t;

extern PWM_t pwm_structs[];

void PWM_Init(void);
double PWM_GetDuty(u8 pwmx);
u8 PWM_SetDuty(u8 pwmx, double duty);

#endif
