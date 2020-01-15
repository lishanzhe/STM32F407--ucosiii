#ifndef __QEI_H
#define __QEI_H

#include "sys.h"

extern volatile int64_t QEI_overflow_cnt[];
extern TIM_TypeDef* QEI_TIM[];

void QEI_Init(void);
int64_t Get_Encoder_Pulse_Count(u8 i);

#endif
