#include "all.h"
#define    EncoderAmount 2
struct Encoder_State Encoder[2];

/*
 * 函数名: Encoder_Init
 * 描  述: 码盘标定系数
 * 输  入: 无
 * 输  出: 无
 * 调  用: 外部调用
 */
void Encoder_Init_Con(void)
{
	u8 tp_last, key_value, i=0, is_key = 0;
		LCD_Clear(WHITE);
		LCD_printf(0,6+36*9,300,24,24,"X+");
    LCD_printf(0,6+36*0,300,24,24,"Find a refrence line and press OK");
	  LCD_printf(0,6+36*1,300,24,24,"to mark the start position");
		while(i==0)
		{
			if (Is_USART_Key_Receive)
			{
				Is_USART_Key_Receive = 0;
				key_value = Key_RxBuffer[0];
				if (key_value != keyempty1)
					is_key = 1;
			}
			if (is_key)
			{
				is_key = 0;
				switch(key_value)
					{		
							case keyOK :
								i=1;
								MOSI_CMD = ENC_L_INIT;
								Write_Database(M_G_CMD_ID);
								break;
							case keyback:
								return;
					}
			}
		}
		i=0;
	  delay_ms(10);
		LCD_Clear(WHITE);
    LCD_printf(0,6+36*0,300,24,24,"X+:Push 3m");
	  LCD_printf(0,6+36*1,300,24,24,"Press OK to end");
		while(i==0)
		{
			LCD_printf(0,6+36*2,300,24,24,"Encoder[0].dis:%lf",Encoder[0].dis);
			LCD_printf(0,6+36*2,300,24,24,"Encoder[1].dis:%lf",Encoder[1].dis);
			if (Is_USART_Key_Receive)
			{
				Is_USART_Key_Receive = 0;
				key_value = Key_RxBuffer[0];
				if (key_value != keyempty1)
					is_key = 1;
			}
			if (is_key)
			{
				is_key = 0;
				switch(key_value)
					{		
							case keyOK :
								i=1;
								Write_Database(M_G_CMD_ID);
								break;
							case keyback:
								return;
					}
			}
		}
		i=0;
	  delay_ms(10);
		LCD_Clear(WHITE);
		LCD_printf(0,6+36*9,300,24,24,"Y+");
    LCD_printf(0,6+36*0,300,24,24,"Find a refrence line and press OK");
	  LCD_printf(0,6+36*1,300,24,24,"to mark the start position");
		
		while(i==0)
		{
			if (Is_USART_Key_Receive)
			{
				Is_USART_Key_Receive = 0;
				key_value = Key_RxBuffer[0];
				if (key_value != keyempty1)
					is_key = 1;
			}
			if (is_key)
			{
				is_key = 0;
				switch(key_value)
					{		
							case keyOK :
								i=1;
								Write_Database(M_G_CMD_ID);
								break;
							case keyback:
								return;
					}
			}
		}
		i=0;
	  delay_ms(10);
		LCD_Clear(WHITE);
    LCD_printf(0,6+36*0,300,24,24,"Y+:Push 3m");
	  LCD_printf(0,6+36*1,300,24,24,"Press OK to end");
		while(i==0)
		{
			LCD_printf(0,6+36*2,300,24,24,"Encoder[0].dis:%lf",Encoder[0].dis);
			LCD_printf(0,6+36*2,300,24,24,"Encoder[1].dis:%lf",Encoder[1].dis);
			if (Is_USART_Key_Receive)
			{
				Is_USART_Key_Receive = 0;
				key_value = Key_RxBuffer[0];
				if (key_value != keyempty1)
					is_key = 1;
			}
			if (is_key)
			{
				is_key = 0;
				switch(key_value)
					{		
							case keyOK :
								i=1;
								Write_Database(M_G_CMD_ID);
								break;
							case keyback:
								return;
					}
			}
		}
		delay_ms(10);
		LCD_Clear(WHITE);
		LCD_printf(0,6+36*2,300,24,24,"Press any key to continue...");
	 	LCD_printf(0,6+36*2,300,24,24,"Invoke Encoder_View to check");
		while(1)
		{
			if (Is_USART_Key_Receive)
			{
				Is_USART_Key_Receive = 0;
				key_value = Key_RxBuffer[0];
				if (key_value != keyempty1)
					is_key = 1;
			}
			if (is_key)
			{
				is_key = 0;
				switch(key_value)
					{	
							case keyback:
								return;
					}
			}
		}
}
/*
 * 函数名: Encoder_View
 * 描  述: 查看码盘的信息
 * 输  入: 无
 * 输  出: 无
 * 调  用: 外部调用
 */
void Encoder_View(void)
{
	u8 tp_last, key_value, i=0, is_key = 0;
		LCD_Clear(WHITE);
}
