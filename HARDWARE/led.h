#ifndef __LED_H
#define __LED_H

#include "sys.h"

#define LED_ON  0
#define LED_OFF 1
#define IO_ON  0
#define IO_OFF 1

#define LED0 PIout(4)
#define LED1 PIout(5)
#define LED2 PIout(6)

void LED_Init(void);	//≥ı ºªØ
void LedOn(void);
#endif
