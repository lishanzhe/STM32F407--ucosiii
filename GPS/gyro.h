/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GYRO_H
#define __GYRO_H

/* Includes ------------------------------------------------------------------*/
#include <stm32f4xx.h>

/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/

extern struct Gyro_State Gyro;

/* Exported function prototype -----------------------------------------------*/
/* Exported function prototype -----------------------------------------------*/
void Gyro_Init(void);
void Gyro_Set(void);
void Gyro_CheckFloat(void);



#endif 

//陀螺仪信息结构体
struct Gyro_State
{
    double angle;            //角度
    double convert1;        //陀螺仪正转系数
    double convert2;        //陀螺仪反转系数
};








