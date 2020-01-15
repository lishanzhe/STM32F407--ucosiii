#include "all.h"


struct route
{
	float dir;
	float dis;
	int id;
}
Route_Blue[100]={
{0	,	1.05	,1},
{0	,	0.83	,1},
{0	,	0.83	,1},
{0	,	0.83	,1},

{1.5,	0.80	,2},

{0	,	0.85	,4},
{0	,	0.85	,11},
{0	,	1.00	,14},

{0	,	1.0		,10},
{1.5,	0.8		,6},	

{0	,	1.0		,15},//del
{0	,	0.8		,10},
{0	,	1.0		,4},
{0	,	1.0		,4},

{0	,	0			,8},

{0	,	1.0		,1},
{0	,	1.0		,1},
{0	,	1.0		,1},
{0	,	1.0		,9},
{0	,	0.6		,1},
},
Route_Red[100]={
{0	,	1.00	,1},
{0	,	0.90	,1},
{0	,	0.90	,1},
{0	,	0.80	,1},

{1.5,	0.8		,3},

{0	,	1.05	,4},
{0	,	0.85	,11},
{0	,	1.0		,4},

{0	,	0.6		,10},
{1.5,	0.5		,12},

{0	,	1.0		,4},
{0	,	0.6		,10},
{0	,	1.0		,4},
{0	,	1.0		,4},

{0	,	0			,7},

{0	,	1.0		,1},
{0	,	1.0		,1},
{0	,	1.0		,1},
{0	,	1.0		,9},
{0	,	0.8		,1},
}
;
void go_route(struct route * Route,int start,int total);

void sys_SRAM(void)
{
	u8 tp_last, key_value, i, is_key = 0;
	u8 SRAM_Type_Index = 0;
	char *SRAM_Type[] = {"SRAMIN ","SRAMEX ","SRAMCCM"};
	u8 *sram_p = 0;
	u8 *sram_tp = 0;
	
	LCD_Clear(WHITE);
	KeyBoard_State = 0;
	Show_Keyboard();
	POINT_COLOR=BLACK;
	LCD_printf(0,6+36*0,300,24,24,"1.Memory Malloc");
	LCD_printf(0,6+36*1,300,24,24,"2.Memory Refresh");
	LCD_printf(0,6+36*2,300,24,24,"3.Memory Free");
	LCD_printf(0,6+36*3,300,24,24,"4.Memory Tab:%s",SRAM_Type[SRAM_Type_Index]);
	LCD_printf(0,6+36*4,300,24,24,"5.SRAMIN USED:%u%%   ",my_mem_perused(SRAMIN));
	LCD_printf(0,6+36*5,300,24,24,"6.SRAMEX USED:%u%%   ",my_mem_perused(SRAMEX));
	LCD_printf(0,6+36*6,300,24,24,"7.SRAMCCM USED:%u%%   ",my_mem_perused(SRAMCCM));
	
	while (1)
	{
		#ifdef BSP_USING_TOUCH_SCREEN
		tp_last = tp_dev.sta&TP_PRES_DOWN;
		tp_dev.scan(0);
		if ((tp_dev.sta&TP_PRES_DOWN) && !tp_last)
		{
			key_value = TP_MainMenu_Judge(tp_dev.x[0], tp_dev.y[0]);
			if (key_value != keyempty1)
				is_key = 1;
		}
		#endif
		#ifdef BSP_USING_USART_KEY
		#ifdef BSP_USING_TOUCH_SCREEN
		else if (Is_USART_Key_Receive)
		#else
		if (Is_USART_Key_Receive)
		#endif
		{
			Is_USART_Key_Receive = 0;
			key_value = Key_RxBuffer[0];
			if (key_value != keyempty1)
				is_key = 1;
		}
		#endif
		if (is_key)
		{
			is_key = 0;
			switch(key_value)
			{
				case key1:
					sram_p = mymalloc(SRAM_Type_Index,2048);//申请2K字节
					if(sram_p != NULL)
						sprintf((char*)sram_p,"Memory Malloc Test:%u   ",i);//向p写入一些内容
					break;
				case key2:
					if(sram_p != NULL)
					{
						sprintf((char*)sram_p,"Memory Malloc Test:%u   ",i);//更新显示内容 	 
						LCD_ShowString(0,6+36*8,300,24,24,sram_p);//显示P的内容
					}
					break;
				case key3:
					myfree(SRAM_Type_Index,sram_p);//释放内存
					sram_p = 0;			//指向空地址
					break;
				case key4:
					++SRAM_Type_Index; 
					if (SRAM_Type_Index > 2) SRAM_Type_Index = 0;
					break;
				case keyback:
					return;
				case keyboardtab:
					KeyBoard_State = !KeyBoard_State;
					Show_Keyboard();
					break;
			}
			POINT_COLOR=BLACK;
			LCD_printf(0,6+36*0,300,24,24,"1.Memory Malloc");
			LCD_printf(0,6+36*1,300,24,24,"2.Memory Refresh");
			LCD_printf(0,6+36*2,300,24,24,"3.Memory Free");
			LCD_printf(0,6+36*3,300,24,24,"4.Memory Tab:%s",SRAM_Type[SRAM_Type_Index]);
			LCD_printf(0,6+36*4,300,24,24,"5.SRAMIN USED:%u%%   ",my_mem_perused(SRAMIN));
			LCD_printf(0,6+36*5,300,24,24,"6.SRAMEX USED:%u%%   ",my_mem_perused(SRAMEX));
			LCD_printf(0,6+36*6,300,24,24,"7.SRAMCCM USED:%u%%   ",my_mem_perused(SRAMCCM));
		}
		else
			delay_ms(1);
		
		if(sram_tp != sram_p)
		{
			sram_tp = sram_p;
			LCD_printf(0,6+36*7,300,24,24,"P Addr:0X%08X",(u32)sram_tp);	//显示p的地址
			if (sram_p)
				LCD_ShowString(0,6+36*8,300,24,24,sram_p);//显示P的内容
		  else
				LCD_Fill(0,6+36*8,319,6+36*8+24,WHITE);	//p=0,清除显示
		}
		++i;
	}
}


void processing(void){
	LCD_Clear(WHITE);
	LCD_printf(0,6+36*0,300,24,24,"=====processing=====");
}
void test_limited(){
	u8 tp_last, key_value, i, is_key = 0;
	LCD_Clear(WHITE);
	KeyBoard_State = 0;
	Show_Keyboard();
	LCD_printf(0,6+36*0,300,24,24,"======test_limited======");
	LCD_printf(0,6+36*1,300,24,24,"1.go_straight_small");
	LCD_printf(0,6+36*2,300,24,24,"2.go_left_small");
	LCD_printf(0,6+36*3,300,24,24,"3.go_right_small");
	LCD_printf(0,6+36*4,300,24,24,"4.go_straight_great");
	LCD_printf(0,6+36*5,300,24,24,"5.go_left_great");
	LCD_printf(0,6+36*6,300,24,24,"6.go_right_great");
	LCD_printf(0,6+36*7,300,24,24,"7.turn90_left");
	LCD_printf(0,6+36*8,300,24,24,"8.turn90_right");
	while(1)
	{

		#ifdef BSP_USING_TOUCH_SCREEN
		tp_last = tp_dev.sta&TP_PRES_DOWN;
		tp_dev.scan(0);
		if ((tp_dev.sta&TP_PRES_DOWN) && !tp_last)
		{
			key_value = TP_MainMenu_Judge(tp_dev.x[0], tp_dev.y[0]);
			if (key_value != keyempty1)
				is_key = 1;
		}
		#endif
		#ifdef BSP_USING_USART_KEY
		#ifdef BSP_USING_TOUCH_SCREEN
		else if (Is_USART_Key_Receive)
		#else
		if (Is_USART_Key_Receive)
		#endif
		{
			Is_USART_Key_Receive = 0;
			key_value = Key_RxBuffer[0];
			if (key_value != keyempty1)
				is_key = 1;
		}
		#endif
		if (is_key)
		{
			is_key = 0;
			switch(key_value)
			{
//				case key1:
//					processing();
//					go_straight_small();
//					break;
//				case key2:
//					processing();
//					go_left_small();
//					break;
//				case key3:
//					processing();
//					go_right_small();
//					break;
//				case key4:
//					processing();
//					go_straight_great();
//					break;
//				case key5:
//					processing();
//					go_left_great();
//					break;
//				case key6:
//					processing();
//					go_right_great();
//					break;
//				case key7:
//					processing();
//					turn90_left();
//					break;
//				case key8:
//					processing();
//					turn90_right();
//					break;
				case keyback:
					return;
			}
			Show_Keyboard();
			LCD_printf(0,6+36*0,300,24,24,"======test_limited======");
			LCD_printf(0,6+36*1,300,24,24,"1.go_straight_small");
			LCD_printf(0,6+36*2,300,24,24,"2.go_left_small");
			LCD_printf(0,6+36*3,300,24,24,"3.go_right_small");
			LCD_printf(0,6+36*4,300,24,24,"4.go_straight_great");
			LCD_printf(0,6+36*5,300,24,24,"5.go_left_great");
			LCD_printf(0,6+36*6,300,24,24,"6.go_right_great");
			LCD_printf(0,6+36*7,300,24,24,"7.turn90_left");
			LCD_printf(0,6+36*8,300,24,24,"8.turn90_right");
		}
		delay_ms(10);
	}
}



void test_small(){
	u8 tp_last, key_value, i, is_key = 0;
	LCD_Clear(WHITE);
	KeyBoard_State = 0;
	Show_Keyboard();

	LCD_printf(0,6+36*0,300,24,24,"======test_small======");
	LCD_printf(0,6+36*1,300,24,24,"1.small_stand");
	LCD_printf(0,6+36*2,300,24,24,"2.small_down");
	LCD_printf(0,6+36*3,300,24,24,"3.great_stand");
	LCD_printf(0,6+36*4,300,24,24,"4.cylinder");
	LCD_printf(0,6+36*5,300,24,24,"5.ground");
	LCD_printf(0,6+36*6,300,24,24,"6.STEP 1");
	LCD_printf(0,6+36*7,300,24,24,"7.STEP 2");
	while(1)
	{

		#ifdef BSP_USING_TOUCH_SCREEN
		tp_last = tp_dev.sta&TP_PRES_DOWN;
		tp_dev.scan(0);
		if ((tp_dev.sta&TP_PRES_DOWN) && !tp_last)
		{
			key_value = TP_MainMenu_Judge(tp_dev.x[0], tp_dev.y[0]);
			if (key_value != keyempty1)
				is_key = 1;
		}
		#endif
		#ifdef BSP_USING_USART_KEY
		#ifdef BSP_USING_TOUCH_SCREEN
		else if (Is_USART_Key_Receive)
		#else
		if (Is_USART_Key_Receive)
		#endif
		{
			Is_USART_Key_Receive = 0;
			key_value = Key_RxBuffer[0];
			if (key_value != keyempty1)
				is_key = 1;
		}
		#endif
		if (is_key)
		{
			is_key = 0;
			switch(key_value)
			{
//				case key1:
//					small_up();
//					break;
//				case key2:
//					small_down();
//					break;
//				case key3:
//					great_up();
//					break;
//				case key4:

//					Cylinder_Flag=0;
//					Write_Database(MISO_CYLINDER_ID);
//					delay_ms(1000);
//					Cylinder_Flag=1;
//					Write_Database(MISO_CYLINDER_ID);
//					delay_ms(1000);
//					Cylinder_Flag=0;
//					Write_Database(MISO_CYLINDER_ID);
//					delay_ms(1000);
//					Cylinder_Flag=1;
//					Write_Database(MISO_CYLINDER_ID);
//					delay_ms(1000);
//					Cylinder_Flag=0;
//					Write_Database(MISO_CYLINDER_ID);
//					delay_ms(1000);
//					Cylinder_Flag=1;
//					Write_Database(MISO_CYLINDER_ID);
//					delay_ms(1000);
//					break;
//				case key5:
//					gen_mid();
//					break;
//				case key6:
//					Input_FloatValue(&speed_rate,"dis_rate");
//					go_forw();
//					speed_rate=1.0;
//					break;
//				case key7:
//					Input_FloatValue(&speed_rate,"dis_rate");
//					leg_forw();
//					speed_rate=1.0;
//					break;
				case keyback:
					return;
			}
			LCD_printf(0,6+36*0,300,24,24,"======test_small======");
			LCD_printf(0,6+36*1,300,24,24,"1.small_stand");
			LCD_printf(0,6+36*2,300,24,24,"2.small_down");
			LCD_printf(0,6+36*3,300,24,24,"3.great_stand");
			LCD_printf(0,6+36*4,300,24,24,"4.cylinder");
			LCD_printf(0,6+36*5,300,24,24,"5.ground");
			LCD_printf(0,6+36*6,300,24,24,"6.STEP 1");
			LCD_printf(0,6+36*7,300,24,24,"7.STEP 2");
		}
		delay_ms(10);
	}
}

void test_great(){
	u8 tp_last, key_value, i, is_key = 0;
	LCD_Clear(WHITE);
	KeyBoard_State = 0;
	Show_Keyboard();
		LCD_printf(0,6+36*0,300,24,24,"======test_great======");
		LCD_printf(0,6+36*1,300,24,24,"1.arg+");
		LCD_printf(0,6+36*2,300,24,24,"2.arg-");
		LCD_printf(0,6+36*3,300,24,24,"3.arg+");
		LCD_printf(0,6+36*4,300,24,24,"4.arg-");
		LCD_printf(0,6+36*5,300,24,24,"5.right+");
		LCD_printf(0,6+36*6,300,24,24,"6.right-");
	while(1)
	{
		
		#ifdef BSP_USING_TOUCH_SCREEN
		tp_last = tp_dev.sta&TP_PRES_DOWN;
		tp_dev.scan(0);
		if ((tp_dev.sta&TP_PRES_DOWN) && !tp_last)
		{
			key_value = TP_MainMenu_Judge(tp_dev.x[0], tp_dev.y[0]);
			if (key_value != keyempty1)
				is_key = 1;
		}
		#endif
		#ifdef BSP_USING_USART_KEY
		#ifdef BSP_USING_TOUCH_SCREEN
		else if (Is_USART_Key_Receive)
		#else
		if (Is_USART_Key_Receive)
		#endif
		{
			Is_USART_Key_Receive = 0;
			key_value = Key_RxBuffer[0];
			if (key_value != keyempty1)
				is_key = 1;
		}
		#endif
		if (is_key)
		{
			is_key = 0;
			switch(key_value)
			{
//				case key1:
//					Loc_Angle_down+=1.5*degree_30;
//					delay_ms(300);
//					break;
//				case key2:
//					Loc_Angle_down-=1.5*degree_30;
//					delay_ms(300);
//					break;
//				case key3:
//					Loc_Angle_up+=1.5*degree_30;
//					delay_ms(300);
//					break;
//				case key4:
//					Loc_Angle_up-=1.5*degree_30;
//					delay_ms(300);
//					break;
//				case key5:
//					set_loc[3]+=4000;
//					delay_ms(300);
//					break;
//				case key6:
//					set_loc[3]-=4000;
//					delay_ms(300);
//					break;
				case keyback:
					return;
			}
			LCD_printf(0,6+36*0,300,24,24,"======test_great======");
			LCD_printf(0,6+36*1,300,24,24,"1.arg+");
			LCD_printf(0,6+36*2,300,24,24,"2.arg-");
			LCD_printf(0,6+36*3,300,24,24,"3.arg+");
			LCD_printf(0,6+36*4,300,24,24,"4.arg-");
			LCD_printf(0,6+36*5,300,24,24,"5.right+");
			LCD_printf(0,6+36*6,300,24,24,"6.right-");
		}
		delay_ms(10);
	}
}

void param(){
	u8 tp_last, key_value, i, is_key = 0;
	LCD_Clear(WHITE);
	KeyBoard_State = 0;
	Show_Keyboard();
		LCD_printf(0,6+36*0,300,24,24,"======param======");
		LCD_printf(0,6+36*1,300,24,24,"1.small_pace");
		LCD_printf(0,6+36*2,300,24,24,"2.great_pace_up");
		LCD_printf(0,6+36*3,300,24,24,"3.great_pace_down");
		LCD_printf(0,6+36*4,300,24,24,"4.pace_delay");
		LCD_printf(0,6+36*5,300,24,24,"5.pace_delay2");
		LCD_printf(0,6+36*6,300,24,24,"6.main_speed");
	while(1)
	{
		
		#ifdef BSP_USING_TOUCH_SCREEN
		tp_last = tp_dev.sta&TP_PRES_DOWN;
		tp_dev.scan(0);
		if ((tp_dev.sta&TP_PRES_DOWN) && !tp_last)
		{
			key_value = TP_MainMenu_Judge(tp_dev.x[0], tp_dev.y[0]);
			if (key_value != keyempty1)
				is_key = 1;
		}
		#endif
		#ifdef BSP_USING_USART_KEY
		#ifdef BSP_USING_TOUCH_SCREEN
		else if (Is_USART_Key_Receive)
		#else
		if (Is_USART_Key_Receive)
		#endif
		{
			Is_USART_Key_Receive = 0;
			key_value = Key_RxBuffer[0];
			if (key_value != keyempty1)
				is_key = 1;
		}
		#endif
		if (is_key)
		{
			is_key = 0;
			switch(key_value)
			{
//				case key1:
//					Input_IntValue(&small_pace,"small_pace");
//					break;
//				case key2:
//					Input_IntValue(&great_pace_up,"great_pace_up");
//					break;
//				case key3:
//					Input_IntValue(&great_pace_down,"great_pace_down");
//					break;
//				case key4:
//					Input_IntValue(&pace_delay,"pace_delay");
//					break;
//				case key5:
//					Input_IntValue(&pace_delay2,"pace_delay2");
//					break;
//				case key6:
//					Input_IntValue(&main_speed,"main_speed");
//					break;
//				case key7:
//					Input_IntValue(&speed_delay,"speed_delay");
//					break;
				case keyback:
					return;
			}
		Show_Keyboard();
		LCD_printf(0,6+36*0,300,24,24,"======param======");
		LCD_printf(0,6+36*1,300,24,24,"1.small_pace");
		LCD_printf(0,6+36*2,300,24,24,"2.great_pace_up");
		LCD_printf(0,6+36*3,300,24,24,"3.great_pace_down");
		LCD_printf(0,6+36*4,300,24,24,"4.pace_delay");
		LCD_printf(0,6+36*5,300,24,24,"5.pace_delay2");
		LCD_printf(0,6+36*6,300,24,24,"6.main_speed");
		}
		delay_ms(10);
	}
}


void sys_dog(void)
{
	u8 tp_last, key_value, i, is_key = 0;
	KeyBoard_State = 0;
	Show_Keyboard();
	LCD_printf(0,6+36*0,300,24,24,"1.test_limited");
	LCD_printf(0,6+36*1,300,24,24,"2.test_small");
	LCD_printf(0,6+36*2,300,24,24,"3.test_great");
	LCD_printf(0,6+36*3,300,24,24,"4.param");
	LCD_printf(0,6+36*4,300,24,24,"5.RED!!");
	LCD_printf(0,6+36*5,300,24,24,"6.BLUE!!");
	LCD_printf(0,6+36*6,300,24,24,"7.Calibration");
	while (1)
	{
		#ifdef BSP_USING_TOUCH_SCREEN
		tp_last = tp_dev.sta&TP_PRES_DOWN;
		tp_dev.scan(0);
		if ((tp_dev.sta&TP_PRES_DOWN) && !tp_last)
		{
			key_value = TP_MainMenu_Judge(tp_dev.x[0], tp_dev.y[0]);
			if (key_value != keyempty1)
				is_key = 1;
		}
		#endif
		#ifdef BSP_USING_USART_KEY
		#ifdef BSP_USING_TOUCH_SCREEN
		else if (Is_USART_Key_Receive)
		#else
		if (Is_USART_Key_Receive)
		#endif
		{
			Is_USART_Key_Receive = 0;
			key_value = Key_RxBuffer[0];
			if (key_value != keyempty1)
				is_key = 1;
		}
		#endif
		if (is_key)
		{
			is_key = 0;
			switch(key_value)
			{
//				case key1:
//					test_limited();
//					break;
//				case key2:
//					test_small();
//					break;
//				case key3:
//					test_great();
//					break;
//				case key4:
//					param();
//					break;
//				case key5:{
//					processing();
//					flag_auto=1;
//					go_route(&Route_Red[0],1,20);
//					flag_auto=0;
//				}
//					break;
//				case key6:{
//					processing();
//					go_route(&Route_Blue[0],1,20);}
//					break;
//				case key7:
// 					moto_chassis[1].total_angle=0;
//					break;
				case keyback:
					return;
				case keyboardtab:
					KeyBoard_State = !KeyBoard_State;
					Show_Keyboard();
					break;
			}
		Show_Keyboard();
		LCD_printf(0,6+36*0,300,24,24,"1.test_limited");
		LCD_printf(0,6+36*1,300,24,24,"2.test_small");
		LCD_printf(0,6+36*2,300,24,24,"3.test_great");
		LCD_printf(0,6+36*3,300,24,24,"4.param");
		LCD_printf(0,6+36*4,300,24,24,"5.RED!!");
		LCD_printf(0,6+36*5,300,24,24,"6.BLUE!!");
		LCD_printf(0,6+36*6,300,24,24,"7.Calibration");
		delay_ms(50);
		}
	}
}
void sys_gyro(void)
{
	u8 tp_last, key_value, i, is_key = 0;
	KeyBoard_State = 0;
	Show_Keyboard();
	
	while (1)
	{
//		LCD_printf(0,6+36*0,300,24,24,"Angel:%f",Gyro_Angle_Total);
		#ifdef BSP_USING_TOUCH_SCREEN
		tp_last = tp_dev.sta&TP_PRES_DOWN;
		tp_dev.scan(0);
		if ((tp_dev.sta&TP_PRES_DOWN) && !tp_last)
		{
			key_value = TP_MainMenu_Judge(tp_dev.x[0], tp_dev.y[0]);
			if (key_value != keyempty1)
				is_key = 1;
		}
		#endif
		#ifdef BSP_USING_USART_KEY
		#ifdef BSP_USING_TOUCH_SCREEN
		else if (Is_USART_Key_Receive)
		#else
		if (Is_USART_Key_Receive)
		#endif
		{
			Is_USART_Key_Receive = 0;
			key_value = Key_RxBuffer[0];
			if (key_value != keyempty1)
				is_key = 1;
		}
		#endif
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

void sys_loc3508(void)
{
	int temp_loc;
	Mode[0] = Location_Mode;
	Input_IntValue(&temp_loc,"set_location");
	if(abs(temp_loc)>100000)
		temp_loc=100000;
	set_loc[0] = temp_loc;
	
}
void sys_spd3508(void)
{
	int temp_spd;
	Mode[0] = Speed_Mode;
	Input_IntValue(&temp_spd,"set_speed");
	if(abs(temp_spd)>3000)
		temp_spd=3000;
	set_spd[0] = temp_spd;
}

void sys_servo(void)
{
	u8 tp_last, key_value, i, is_key = 0;
	int duty;
	LCD_Clear(WHITE);
	KeyBoard_State = 0;
	Show_Keyboard();
		LCD_printf(0,6+36*0,300,24,24,"======servo======");
		LCD_printf(0,6+36*1,300,24,24,"1.duty");
	while(1)
	{
		
		#ifdef BSP_USING_TOUCH_SCREEN
		tp_last = tp_dev.sta&TP_PRES_DOWN;
		tp_dev.scan(0);
		if ((tp_dev.sta&TP_PRES_DOWN) && !tp_last)
		{
			key_value = TP_MainMenu_Judge(tp_dev.x[0], tp_dev.y[0]);
			if (key_value != keyempty1)
				is_key = 1;
		}
		#endif
		#ifdef BSP_USING_USART_KEY
		#ifdef BSP_USING_TOUCH_SCREEN
		else if (Is_USART_Key_Receive)
		#else
		if (Is_USART_Key_Receive)
		#endif
		{
			Is_USART_Key_Receive = 0;
			key_value = Key_RxBuffer[0];
			if (key_value != keyempty1)
				is_key = 1;
		}
		#endif
		if (is_key)
		{
			is_key = 0;
			switch(key_value)
			{
				case key2:
					Input_IntValue(&duty,"duty");
//					PWM_SetDuty(2,duty);
					break;
				case key3:
//					PWM_SetDuty(2,3.0);
//					delay_ms(200);
//					PWM_SetDuty(2,4.0);
//					delay_ms(200);
//					PWM_SetDuty(2,5.0);
//					delay_ms(200);
//					PWM_SetDuty(2,6.0);
//					delay_ms(200);
//					PWM_SetDuty(2,7.0);
//					delay_ms(200);
//					PWM_SetDuty(2,8.0);
//					delay_ms(200);
//					PWM_SetDuty(2,9.0);
//					delay_ms(200);
					break;
				case keyback:
					return;
			}
		Show_Keyboard();
			LCD_printf(0,6+36*0,300,24,24,"======servo======");
			LCD_printf(0,6+36*1,300,24,24,"1.duty");
		}
		delay_ms(10);
	}
}

//

void sys_Gyro()
{
		u8 tp_last, key_value, i, is_key = 0;
		LCD_Clear(WHITE);
		LCD_printf(0,6+36*0,300,24,24,"======Gyro======");
		LCD_printf(0,6+36*1,300,24,24,"angle:%lf",Gyro.angle);
		LCD_printf(0,6+36*2,300,24,24,"Con1:%lf",Gyro.convert1);
		LCD_printf(0,6+36*3,300,24,24,"Con2:%lf",Gyro.convert2);
		LCD_printf(0,6+36*4,300,24,24,"1.Init 2.Set 3.Float");
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
				case key1:
						Gyro_Init();
						break;
         case key2:
            Gyro_Set();
            break;
         case key3:
            Gyro_CheckFloat();
            break;
         case keyback:
            return;
			 }

			}
			delay_ms(10);
		}
}
		
void	sys_Encoder()
{
	u8 tp_last, key_value, i, is_key = 0;
		LCD_Clear(WHITE);
		LCD_printf(0,6+36*0,300,24,24,"====Encoder====");
		LCD_printf(0,6+36*1,300,24,24,"Dis1:%lf",Encoder[0].dis);
		LCD_printf(0,6+36*2,300,24,24,"Dis2:%lf",Encoder[1].dis);
		LCD_printf(0,6+36*3,300,24,24,"1.Init_Con 2.Init_R");
		LCD_printf(0,6+36*4,300,24,24,"3.View 4.Set");
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
				case key1:
					 Encoder_Init_Con();
						break;
         case key2:
//            Encoder_Init_R();
            break;
         case key3:
            Encoder_View();
            break;
				 case key4:
//				    Encoder_Set();
         case keyback:
            return;
		 }

		}
			delay_ms(10);
		}
}
void sys_GPS()
{

}
