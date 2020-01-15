/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GPS_H
#define __GPS_H
#include "MyTypeDef.h"
/* Includes ------------------------------------------------------------------*/
#include <stm32f4xx.h>
//点结构体

/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
struct GPS_State
{
    struct Point position;    
    double radian;            //弧度
};

extern struct GPS_State GPS;

/* Exported function prototype -----------------------------------------------*/
/* Exported function prototype -----------------------------------------------*/
void GPS_Init(struct Point position,double radian);
void GPS_Clear(void);
//全场定位信息结构体


#endif 








