/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MENUFUNC_H
#define __MENUFUNC_H
/* Includes ------------------------------------------------------------------*/
#include "sys.h"

/* Defines ------------------------------------------------------------------*/
/* Exported variables -------------------------------------------------------*/
extern u8 KeyBoard_State;
/* Exported macro ------------------------------------------------------------*/
/* function prototypes -------------------------------------------------------*/
void Show_Menu_List(void);
void Show_Keyboard(void);
void KeyNumOperate(u8 KeyValue);
void GoKeyNum(int KeyNum);

#endif 

/**********************************END OF FILE*********************************/

