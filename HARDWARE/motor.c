#include "motor.h"
//#include "mymath.h"
//#include "usart.h"

//typedef enum
//{PA, PB, PC, PD, PE, PF, PG, PH, PI}eGPIO_t;

//PWM_t pwm_structs[] = 
//{
////   clk_freq      pwm_freq        timx    	   chx         gpiox       pinx       duty_init
//	{1000000,		1000,			13,			1,			PF,			8,           0},
//	{1000000,		1000,			9,			2,			PE,			6,           0},
//	{1000000,		1000,			10,			1,			PF,			6,           0},
//	{1000000,		1000,			11,			1,			PF,			7,           0},
//};

//static const u8 pwm_num = sizeof(pwm_structs) / sizeof(pwm_structs[0]);

//void PWM_DIR_Init(void)
//{
//	GPIO_InitTypeDef GPIO_InitStructure;
//	
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH | RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC, ENABLE);

//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
//	GPIO_Init(GPIOH, &GPIO_InitStructure);
//	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);
//	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
//	GPIO_Init(GPIOC, &GPIO_InitStructure);
//	
//	PWM1_DIR = PWM2_DIR = PWM3_DIR = PWM4_DIR = 0;
//	PWM1_EN_N = PWM2_EN_N = PWM3_EN_N = PWM4_EN_N = 0;
//}

//static u8 GetAPBx(u8 timx)
//{
//	if(timx > 14) 
//		return 0xff;
//	else if((timx >= 2 && timx <= 7) || (timx >= 12 && timx <= 14))
//		return 1;
//	else
//		return 2;
//}

//static u8 Get_AF(u8 timx)
//{
//	if(timx <= 2)
//		return (u8)1;
//	else if(timx <= 5)
//		return (u8)2;
//	else if(timx <= 11)
//		return (u8)3;
//	else if(timx <= 14)
//		return (u8)9;
//	else
//		return 0xff;
//}

//static u32 Get_TimRCC(u8 timx)
//{
//	if(timx <= 1)
//		return (u32)1;
//	else if(timx <= 7)
//		return (u32)(1 << (timx - 2));
//	else if(timx <= 8)
//		return (u32)2;
//	else if(timx <= 11)
//		return (u32)(1 << (timx + 7));
//	else if(timx <= 14)
//		return (u32)(1 << (timx - 6));
//	else
//		return 0xff;
//}

//static void PWM_GPIO_Init(PWM_t *pwm_struct)
//{
//	GPIO_InitTypeDef GPIO_InitStructure;
//	GPIO_TypeDef *gpiox = (GPIO_TypeDef *)(AHB1PERIPH_BASE + 0x0400 * pwm_struct->gpiox);
//	u16 pin_bit = 1 << pwm_struct->pinx;
//	
//	RCC_AHB1PeriphClockCmd((u32)(1 << pwm_struct->gpiox), ENABLE);
//	GPIO_InitStructure.GPIO_Pin = pin_bit;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
//	GPIO_Init(gpiox, &GPIO_InitStructure);
//	
//	GPIO_PinAFConfig(gpiox, (u8)pwm_struct->pinx, Get_AF(pwm_struct->tim_num));
//}

//static TIM_TypeDef *Get_TIMx(u8 timx)
//{
//	switch (timx)
//	{
//		case 1:  return TIM1;
//		case 2:  return TIM2;
//		case 3:  return TIM3;
//		case 4:  return TIM4;
//		case 5:  return TIM5;
//		case 6:  return TIM6;
//		case 7:  return TIM7;
//		case 8:  return TIM8;
//		case 9:  return TIM9;
//		case 10: return TIM10;
//		case 11: return TIM11;
//		case 12: return TIM12;
//		case 13: return TIM13;
//		case 14: return TIM14;
//		default: break;
//	}
//	return (TIM_TypeDef *)0;
//}

///**
//  * @brief  PWM波的时钟初始化
//  * @note   
//  * @param  None
//  * @retval None
//  */
//static void PWM_CLK_Init(PWM_t *pwm_struct)
//{   
//	u16 presc;
//	u16 cycle;
//	u32 p_freq;
//	u8  APBx;
//	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
//	
//	APBx = GetAPBx(pwm_struct->tim_num);
//	p_freq = (APBx == 1) ? (SystemCoreClock/2) : SystemCoreClock;
//	presc  = p_freq / pwm_struct->clk_freq;
//	cycle  = pwm_struct->clk_freq / pwm_struct->pwm_freq;
//	if(APBx == 1)
//		RCC_APB1PeriphClockCmd(Get_TimRCC(pwm_struct->tim_num), ENABLE); 
//	else
//		RCC_APB2PeriphClockCmd(Get_TimRCC(pwm_struct->tim_num), ENABLE); 
//	
//	TIM_TimeBaseStructure.TIM_Period = cycle - 1;
//	TIM_TimeBaseStructure.TIM_Prescaler = presc - 1;
//	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
//	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

//	TIM_TimeBaseInit(Get_TIMx(pwm_struct->tim_num), &TIM_TimeBaseStructure);
//}

///**
//  * @brief  PWM波的输出配置
//  * @note   
//  * @param  None
//  * @retval None
//  */
//static void PWM_OCInit(PWM_t *pwm_struct)
//{
//	TIM_OCInitTypeDef  TIM_OCInitStructure;
//	TIM_TypeDef *timx = Get_TIMx(pwm_struct->tim_num);
//	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
//	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
//	TIM_OCInitStructure.TIM_Pulse = pwm_struct->duty_init * pwm_struct->clk_freq / pwm_struct->pwm_freq / 100;
//	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
//	
//	switch (pwm_struct->chx)
//	{
//		case 1:
//			TIM_OC1Init(timx, &TIM_OCInitStructure);
//			TIM_OC1PreloadConfig(timx, TIM_OCPreload_Enable);
//			break;
//		case 2:
//			TIM_OC2Init(timx, &TIM_OCInitStructure);
//			TIM_OC2PreloadConfig(timx, TIM_OCPreload_Enable);
//			break;
//		case 3:
//			TIM_OC3Init(timx, &TIM_OCInitStructure);
//			TIM_OC3PreloadConfig(timx, TIM_OCPreload_Enable);            
//			break;
//		case 4:
//			TIM_OC4Init(timx, &TIM_OCInitStructure);
//			TIM_OC4PreloadConfig(timx, TIM_OCPreload_Enable);            
//			break;
//		default:
//			break;
//	}
//	TIM_ARRPreloadConfig(timx, ENABLE);//ARPE使能
//	
//	TIM_Cmd(timx, ENABLE);
//}

///**
//  * @brief  PWM波的初始化
//  * @note   
//  * @param  None
//  * @retval None
//  */
//void PWM_Init(void)
//{
//	int i;
//	
//	PWM_DIR_Init();
//	
//	for(i = 0; i < pwm_num; ++i)
//	{
//		PWM_GPIO_Init(&pwm_structs[i]);
//		PWM_CLK_Init(&pwm_structs[i]);
//		PWM_OCInit(&pwm_structs[i]);
//	}
//}

//void Motor_Set_Rotation_Speed(u8 pwmx, double speed_rpm)
//{
//	TIM_TypeDef * timx = Get_TIMx(pwm_structs[pwmx-1].tim_num);
//	u32 ccrx, arr;
//	double freq;
//	
//	if (!(pwmx >= 1 && pwmx <= MOTOR_SIZE))
//		return;
//	
//	freq = fabs(speed_rpm) * MOTOR_CPR / 60;
//	if (freq >= MOTOR_FREQ_MIN && freq <= MOTOR_FREQ_MAX)
//	{
//		arr = pwm_structs[pwmx-1].clk_freq / freq - 1;
//		ccrx = pwm_structs[pwmx-1].clk_freq / freq / 2;
//		switch (pwmx)
//		{
//			case 1:
//				if (speed_rpm > 0)
//					PWM1_DIR = 1, PWM1_EN_N = 0;
//				else if (speed_rpm < 0)
//					PWM1_DIR = 0, PWM1_EN_N = 0;
//				else
//					PWM1_DIR = 0, PWM1_EN_N = 0;
//				break;
//			case 2:
//				if (speed_rpm > 0)
//					PWM2_DIR = 1, PWM2_EN_N = 0;
//				else if (speed_rpm < 0)
//					PWM2_DIR = 0, PWM2_EN_N = 0;
//				else
//					PWM2_DIR = 0, PWM2_EN_N = 0;
//				break;
//			case 3:
//				if (speed_rpm > 0)
//					PWM3_DIR = 1, PWM3_EN_N = 0;
//				else if (speed_rpm < 0)
//					PWM3_DIR = 0, PWM3_EN_N = 0;
//				else
//					PWM3_DIR = 0, PWM3_EN_N = 0;
//				break;
//			case 4:
//				if (speed_rpm > 0)
//					PWM4_DIR = 1, PWM4_EN_N = 0;
//				else if (speed_rpm < 0)
//					PWM4_DIR = 0, PWM4_EN_N = 0;
//				else
//					PWM4_DIR = 0, PWM4_EN_N = 0;
//				break;
//			default:
//				break;
//		}
//		timx->ARR = arr;
//		switch (pwm_structs[pwmx-1].chx)
//		{
//			case 1: timx->CCR1 = ccrx; break;
//			case 2: timx->CCR2 = ccrx; break;
//			case 3: timx->CCR3 = ccrx; break;
//			case 4: timx->CCR4 = ccrx; break;
//			default: break;
//		}
//	}
//	else
//	{
//		switch (pwmx)
//		{
//			case 1:
//				PWM1_DIR = 0, PWM1_EN_N = 0;
//				break;
//			case 2:
//				PWM2_DIR = 0, PWM2_EN_N = 0;
//				break;
//			case 3:
//				PWM3_DIR = 0, PWM3_EN_N = 0;
//				break;
//			case 4:
//				PWM4_DIR = 0, PWM4_EN_N = 0;
//				break;
//			default:
//				break;
//		}
//		ccrx = 0;
//		switch (pwm_structs[pwmx-1].chx)
//		{
//			case 1: timx->CCR1 = ccrx; break;
//			case 2: timx->CCR2 = ccrx; break;
//			case 3: timx->CCR3 = ccrx; break;
//			case 4: timx->CCR4 = ccrx; break;
//			default: break;
//		}
//	}
//}

//double Motor_Get_Rotation_Speed(u8 pwmx)
//{
//	TIM_TypeDef * timx = Get_TIMx(pwm_structs[pwmx-1].tim_num);
//	u32 ccrx;
//	
//	if (!(pwmx >= 1 && pwmx <= MOTOR_SIZE))
//		return 0;
//	
//	switch(pwm_structs[pwmx-1].chx)
//	{
//		case 1: ccrx = timx->CCR1; break;
//		case 2: ccrx = timx->CCR2; break;
//		case 3: ccrx = timx->CCR3; break;
//		case 4: ccrx = timx->CCR4; break;
//		default: break;
//	}
//	
//	if (ccrx == 0)
//		return 0;
//	else
//	{
//		switch (pwmx)
//		{
//			case 1:
//				return (PWM1_DIR == 0 ? -1.0 : 1.0) * pwm_structs[pwmx-1].clk_freq * 60.0 / MOTOR_CPR / (timx->ARR + 1);
//			case 2:
//				return (PWM2_DIR == 0 ? -1.0 : 1.0) * pwm_structs[pwmx-1].clk_freq * 60.0 / MOTOR_CPR / (timx->ARR + 1);
//			case 3:
//				return (PWM3_DIR == 0 ? -1.0 : 1.0) * pwm_structs[pwmx-1].clk_freq * 60.0 / MOTOR_CPR / (timx->ARR + 1);
//			case 4:
//				return (PWM4_DIR == 0 ? -1.0 : 1.0) * pwm_structs[pwmx-1].clk_freq * 60.0 / MOTOR_CPR / (timx->ARR + 1);
//			default:
//				return 0;
//		}
//	}
//}

//void Motor_Set_Enabled(u8 enabled)
//{
//	if (enabled)
//		PWM1_EN_N = PWM2_EN_N = PWM3_EN_N = PWM4_EN_N = 0;
//	else
//		PWM1_EN_N = PWM2_EN_N = PWM3_EN_N = PWM4_EN_N = 1;
//}
///********************************PanTilt Motor*****************************/

////void Set_PanTilt_Angle_withspeed(int64_t angle, int32_t speed)
////{
////	u8 i;
////	u8 sum = 0;
////	
////	PanTilt_TxBuffer[0] = 0x3E;
////	PanTilt_TxBuffer[1] = 0xA4;
////	PanTilt_TxBuffer[2] = 0x01;
////	PanTilt_TxBuffer[3] = 0x0C;
////	PanTilt_TxBuffer[4] = 0xEF;
////	for(i = 0;i < 8;i++)
////		PanTilt_TxBuffer[5+i] = *((int8_t*)(&angle)+i);
////	for(i = 0;i < 4;i++)
////		PanTilt_TxBuffer[13+i] = *((int8_t*)(&speed)+i);
////	for(i = 5;i < 17;i++)
////		sum += PanTilt_TxBuffer[i];
////	PanTilt_TxBuffer[17] = sum;
////	
////	DMA_SetCurrDataCounter(USART6_DMA_TX_STREAM, PanTilt_TxBufferSize);
////	DMA_Cmd(USART6_DMA_TX_STREAM, ENABLE);
////}


