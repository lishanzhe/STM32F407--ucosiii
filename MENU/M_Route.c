#include "all.h"

void route_Mecanum(void)
{
	u8 i, tp_last, key_value;
	double pwm_duty[4] = {0};
	double temp;
	
	LCD_Clear(WHITE);
	KeyBoard_State = 0;
	Show_Keyboard();
	POINT_COLOR=BLACK;
	for (i = 0; i < 4; ++i)
	{
		LCD_printf(0,6+36*i,300,24,24,"%d.PWM%d Duty = %lf",i+1,i+1,PWM_GetDuty(i+1));
	}
	LCD_printf(0,6+36*4,300,24,24,"5.Speed_X = %lf",Speed_X);
	LCD_printf(0,6+36*5,300,24,24,"6.Speed_Y = %lf",Speed_Y);
	LCD_printf(0,6+36*6,300,24,24,"7.Speed_Rot = %lf",Speed_Rotation);
	LCD_printf(0,6+36*7,300,24,24,"8.Speed_Stop");
	LCD_printf(0,6+36*8,300,24,24,"9.Angle = %lf        ",GPS_List[0].angle);
	LCD_printf(0,6+36*9,300,24,24,"10.X = %lf        ",GPS_List[0].position.x);
	LCD_printf(0,6+36*10,300,24,24,"11.Y = %lf        ",GPS_List[0].position.y);
	
	while (1)
	{
		for (i = 0; i < 4; ++i)
		{
			LCD_printf(0,6+36*i,300,24,24,"%d.PWM%d Duty = %lf",i+1,i+1,PWM_GetDuty(i+1));
		}
		LCD_printf(0,6+36*8,300,24,24,"9.Angle = %lf        ",GPS_List[0].angle);
		LCD_printf(0,6+36*9,300,24,24,"10.X = %lf        ",GPS_List[0].position.x);
		LCD_printf(0,6+36*10,300,24,24,"11.Y = %lf        ",GPS_List[0].position.y);
		#ifndef USING_TOUCH_SCREEN
		if (USART_Key_Receive)
		{
			USART_Key_Receive = 0;
			key_value = Key_RxBuffer[0];
		#else
		tp_last = tp_dev.sta&TP_PRES_DOWN;
		tp_dev.scan(0); 		 
		if ((tp_dev.sta&TP_PRES_DOWN) && !tp_last)
		{			//触摸屏被按下	
		 	if (tp_dev.x[0]<lcddev.width&&tp_dev.y[0]<lcddev.height)
			{
				key_value = TP_Row_Judge(tp_dev.x[0], tp_dev.y[0]);
		#endif
				switch(key_value)
				{
					case key1:
						Input_DoubleValue(&pwm_duty[0],"PWM1");
						if (pwm_duty[0] <= PWM_Limit)
							PWM_SetDuty(1,pwm_duty[0],1);
						break;
					case key2:
						Input_DoubleValue(&pwm_duty[1],"PWM2");
						if (pwm_duty[1] <= PWM_Limit)
							PWM_SetDuty(2,pwm_duty[1],1);
						break;
					case key3:
						Input_DoubleValue(&pwm_duty[2],"PWM3");
						if (pwm_duty[2] <= PWM_Limit)
							PWM_SetDuty(3,pwm_duty[2],1);
						break;
					case key4:
						Input_DoubleValue(&pwm_duty[3],"PWM4");
						if (pwm_duty[3] <= PWM_Limit)
							PWM_SetDuty(4,pwm_duty[3],1);
						break;
					case key5:
						Input_DoubleValue(&temp,"Speed_X");
						Speed_X = temp;
						SetSpeed(Speed_X, Speed_Y, Speed_Rotation);
						break;
					case key6:
						Input_DoubleValue(&temp,"Speed_Y");
						Speed_Y = temp;
						SetSpeed(Speed_X, Speed_Y, Speed_Rotation);
						break;
					case key7:
						Input_DoubleValue(&temp,"Speed_Rotation");
						Speed_Rotation = temp;
						SetSpeed(Speed_X, Speed_Y, Speed_Rotation);
						break;
					case key8:
						SPEED_STOP;
						pwm_duty[0] = pwm_duty[1] = pwm_duty[2] = pwm_duty[3] = 0;
						temp = 0;
						SetSpeed(Speed_X, Speed_Y, Speed_Rotation);
						break;
					case keyback:
						SPEED_STOP;
						SetSpeed(Speed_X, Speed_Y, Speed_Rotation);
						return;
					case keyboardtab:
						KeyBoard_State = !KeyBoard_State;
						Show_Keyboard();
						break;
					default:
						break;
				}
				Show_Keyboard();
				POINT_COLOR=BLACK;
				for (i = 0; i < 4; ++i)
				{
					LCD_printf(0,6+36*i,300,24,24,"%d.PWM%d Duty = %lf",i+1,i+1,PWM_GetDuty(i+1));
				}
				LCD_printf(0,6+36*4,300,24,24,"5.Speed_X = %lf",Speed_X);
				LCD_printf(0,6+36*5,300,24,24,"6.Speed_Y = %lf",Speed_Y);
				LCD_printf(0,6+36*6,300,24,24,"7.Speed_Rot = %lf",Speed_Rotation);
				LCD_printf(0,6+36*7,300,24,24,"8.Speed_Stop");
				LCD_printf(0,6+36*8,300,24,24,"9.Angle = %lf        ",GPS_List[0].angle);
				LCD_printf(0,6+36*9,300,24,24,"10.X = %lf        ",GPS_List[0].position.x);
				LCD_printf(0,6+36*10,300,24,24,"11.Y = %lf        ",GPS_List[0].position.y);
			#ifndef USING_TOUCH_SCREEN
			#else
			}
			#endif
		}
		else
			delay_ms(1);
	}
}

void route_Line(void)
{
	u8 tp_last, key_value;
	u8 goline = 0;
	double aim_point_x = 3000, aim_point_y = 0, aim_angle = 0, aim_radian = 0;
	double speed_sta = 1500, speed_end = 1500;
	double speed_max = 1600;		//mm/s
	struct Point end_point = {3000,0};
	
	LCD_Clear(WHITE);
	KeyBoard_State = 0;
	Show_Keyboard();
	POINT_COLOR=BLACK;
	LCD_printf(0,6+36*0,300,24,24,"1.aim_point_x = %lf", aim_point_x);
	LCD_printf(0,6+36*1,300,24,24,"2.aim_point_y = %lf", aim_point_y);
	LCD_printf(0,6+36*2,300,24,24,"3.aim_angle/deg = %lf",aim_angle);
	LCD_printf(0,6+36*3,300,24,24,"4.speed_sta = %lf",speed_sta);
	LCD_printf(0,6+36*4,300,24,24,"5.speed_max = %lf",speed_max);
	LCD_printf(0,6+36*5,300,24,24,"6.speed_end = %lf",speed_end);
	LCD_printf(0,6+36*6,300,24,24,"7.GPS Clear");
	LCD_printf(0,6+36*7,300,24,24,"8.Set line and go");
	
	while (!goline)
	{
		#ifndef USING_TOUCH_SCREEN
		if (USART_Key_Receive)
		{
			USART_Key_Receive = 0;
			key_value = Key_RxBuffer[0];
		#else
		tp_last = tp_dev.sta&TP_PRES_DOWN;
		tp_dev.scan(0); 		 
		if ((tp_dev.sta&TP_PRES_DOWN) && !tp_last)
		{			//触摸屏被按下	
		 	if (tp_dev.x[0]<lcddev.width&&tp_dev.y[0]<lcddev.height)
			{
				key_value = TP_Row_Judge(tp_dev.x[0], tp_dev.y[0]);
		#endif
				switch(key_value)
				{
					case key1:
						Input_DoubleValue(&aim_point_x,"aim_point_x");
						end_point.x = aim_point_x;
						break;
					case key2:
						Input_DoubleValue(&aim_point_y,"aim_point_y");
						end_point.y = aim_point_y;
						break;
					case key3:
						Input_DoubleValue(&aim_angle,"aim_angle");
						while (aim_angle > 180)
							aim_angle -= 180;
						while (aim_angle < -180)
							aim_angle += 180;
						aim_radian = aim_angle * DEG2RAD;
						break;
					case key4:
						Input_DoubleValue(&speed_sta,"speed_sta");
						break;
					case key5:
						Input_DoubleValue(&speed_max,"speed_max");
						break;
					case key6:
						Input_DoubleValue(&speed_end,"speed_end");
						break;
					case key7:
						GPS_Clear();
						break;
					case key8:
						SetLine(end_point, aim_radian, speed_sta, speed_max, speed_end);
						goline = 1;
						break;
					case keyback:
						SPEED_STOP;
						SetSpeed(Speed_X, Speed_Y, Speed_Rotation);
						return;
					case keyboardtab:
						KeyBoard_State = !KeyBoard_State;
						Show_Keyboard();
						break;
					default:
						break;
				}
				Show_Keyboard();
				POINT_COLOR=BLACK;
				LCD_printf(0,6+36*0,300,24,24,"1.aim_point_x = %lf", aim_point_x);
				LCD_printf(0,6+36*1,300,24,24,"2.aim_point_y = %lf", aim_point_y);
				LCD_printf(0,6+36*2,300,24,24,"3.aim_angle/deg = %lf",aim_angle);
				LCD_printf(0,6+36*3,300,24,24,"4.speed_sta = %lf",speed_sta);
				LCD_printf(0,6+36*4,300,24,24,"5.speed_max = %lf",speed_max);
				LCD_printf(0,6+36*5,300,24,24,"6.speed_end = %lf",speed_end);
				LCD_printf(0,6+36*6,300,24,24,"7.GPS Clear");
				LCD_printf(0,6+36*7,300,24,24,"8.Set line and go");
			#ifndef USING_TOUCH_SCREEN
			#else
			}
			#endif
		}
		else
			delay_ms(1);
	}
	
	while (!(GetLength(GPS_List[0].position, end_point) < 10.0 && fabs(GPS_List[0].angle - aim_angle) < 0.1))
		GoLine();
	SPEED_STOP;
	SetSpeed(Speed_X, Speed_Y, Speed_Rotation);
}

void route_Handle(void)
{
	u8 tp_last, key_value;
	
	LCD_Clear(WHITE);
	KeyBoard_State = 0;
	Show_Keyboard();
	POINT_COLOR=BLACK;
	LCD_printf(0,6+36*0,300,24,24,"%X, %X, %X, %X, %X, %X  ", Handle_RxBuffer[0],Handle_RxBuffer[1],Handle_RxBuffer[2],Handle_RxBuffer[3],Handle_RxBuffer[4],Handle_RxBuffer[5]);
	LCD_printf(0,6+36*1,300,24,24,"%X, %X, %X, %X, %X, %X  ", Handle_RxBuffer[6],Handle_RxBuffer[7],Handle_RxBuffer[8],Handle_RxBuffer[9],Handle_RxBuffer[10],Handle_RxBuffer[11]);
	LCD_printf(0,6+36*2,300,24,24,"Key 0x%X    ", Handle_Key_Value);
	
	while (1)
	{
		LCD_printf(0,6+36*0,300,24,24,"%X, %X, %X, %X, %X, %X  ", Handle_RxBuffer[0],Handle_RxBuffer[1],Handle_RxBuffer[2],Handle_RxBuffer[3],Handle_RxBuffer[4],Handle_RxBuffer[5]);
		LCD_printf(0,6+36*1,300,24,24,"%X, %X, %X, %X, %X, %X  ", Handle_RxBuffer[6],Handle_RxBuffer[7],Handle_RxBuffer[8],Handle_RxBuffer[9],Handle_RxBuffer[10],Handle_RxBuffer[11]);
		LCD_printf(0,6+36*2,300,24,24,"Key 0x%X    ", Handle_Key_Value);
		#ifndef USING_TOUCH_SCREEN
		if (USART_Key_Receive)
		{
			USART_Key_Receive = 0;
			key_value = Key_RxBuffer[0];
		#else
		tp_last = tp_dev.sta&TP_PRES_DOWN;
		tp_dev.scan(0); 		 
		if ((tp_dev.sta&TP_PRES_DOWN) && !tp_last)
		{			//触摸屏被按下	
		 	if (tp_dev.x[0]<lcddev.width&&tp_dev.y[0]<lcddev.height)
			{
				key_value = TP_Row_Judge(tp_dev.x[0], tp_dev.y[0]);
		#endif
				switch(key_value)
				{
					case keyback:
						return;
					case keyboardtab:
						KeyBoard_State = !KeyBoard_State;
						Show_Keyboard();
						break;
					default:
						break;
				}
				Show_Keyboard();
				LCD_printf(0,6+36*0,300,24,24,"%X, %X, %X, %X, %X, %X  ", Handle_RxBuffer[0],Handle_RxBuffer[1],Handle_RxBuffer[2],Handle_RxBuffer[3],Handle_RxBuffer[4],Handle_RxBuffer[5]);
				LCD_printf(0,6+36*1,300,24,24,"%X, %X, %X, %X, %X, %X  ", Handle_RxBuffer[6],Handle_RxBuffer[7],Handle_RxBuffer[8],Handle_RxBuffer[9],Handle_RxBuffer[10],Handle_RxBuffer[11]);
				LCD_printf(0,6+36*2,300,24,24,"Key 0x%X    ", Handle_Key_Value);
			#ifndef USING_TOUCH_SCREEN
			#else
			}
			#endif
		}
		else
			delay_ms(1);
	}
}

void route_Servo(void)
{
	u8 i, tp_last, key_value;
	double pwm_duty[4] = {0};
	u16 touch_pos[4] = {0};
	
	LCD_Clear(WHITE);
	KeyBoard_State = 0;
	Show_Keyboard();
	POINT_COLOR=BLACK;
	for (i = 0; i < 4; ++i)
	{
		LCD_printf(0,6+36*2*i,300,24,24,"%d.Servo%d Duty = %lf",i+1,i+1,Servo_PWM_GetDuty(i+1));
	}
	for (i = 0; i < 4; ++i)
	{
		LCD_DrawRectangle(0,6+36*(2*i+1)+12-2,300,6+36*(2*i+1)+12+2);
		LCD_DrawRectangle(touch_pos[i],6+36*(2*i+1),touch_pos[i]+20,6+36*(2*i+1)+24);
	}
	
	while (1)
	{
		for (i = 0; i < 4; ++i)
		{
			LCD_printf(0,6+36*2*i,300,24,24,"%d.Servo%d Duty = %lf",i+1,i+1,Servo_PWM_GetDuty(i+1));
		}
		for (i = 0; i < 4; ++i)
		{
			LCD_DrawRectangle(0,6+36*(2*i+1)+12-2,300,6+36*(2*i+1)+12+2);
			LCD_DrawRectangle(touch_pos[i],6+36*(2*i+1),touch_pos[i]+20,6+36*(2*i+1)+24);
		}
		#ifndef USING_TOUCH_SCREEN
		if (USART_Key_Receive)
		{
			USART_Key_Receive = 0;
			key_value = Key_RxBuffer[0];
		#else
		tp_last = tp_dev.sta&TP_PRES_DOWN;
		tp_dev.scan(0); 		 
		if ((tp_dev.sta&TP_PRES_DOWN) && !tp_last)
		{			//触摸屏被按下	
		 	if (tp_dev.x[0]<lcddev.width&&tp_dev.y[0]<lcddev.height)
			{
				key_value = TP_Row_Judge(tp_dev.x[0], tp_dev.y[0]);
		#endif
				switch(key_value)
				{
					case key1:
						Input_DoubleValue(&pwm_duty[0],"Servo1");
						if (pwm_duty[0] <= 100)
							Servo_PWM_SetDuty(1, pwm_duty[0]);
						break;
					case key2:
						if (tp_dev.x[0] < 300)
						{
							touch_pos[0] = tp_dev.x[0];
							pwm_duty[0] = tp_dev.x[0]*(50.0-10.0)/300.0+10.0;
							if (pwm_duty[0] <= 100)
								Servo_PWM_SetDuty(1, pwm_duty[0]);
						}
						break;
					case key3:
						Input_DoubleValue(&pwm_duty[1],"Servo2");
						if (pwm_duty[1] <= 100)
							Servo_PWM_SetDuty(2, pwm_duty[1]);
						break;
					case key4:
						if (tp_dev.x[0] < 300)
						{
							touch_pos[1] = tp_dev.x[0];
							pwm_duty[1] = tp_dev.x[0]*(50.0-10.0)/300.0+10.0;
							if (pwm_duty[1] <= 100)
								Servo_PWM_SetDuty(2, pwm_duty[1]);
						}
						break;
					case key5:
						Input_DoubleValue(&pwm_duty[2],"Servo3");
						if (pwm_duty[2] <= 100)
							Servo_PWM_SetDuty(3, pwm_duty[2]);
						break;
					case key6:
						if (tp_dev.x[0] < 300)
						{
							touch_pos[2] = tp_dev.x[0];
							pwm_duty[2] = tp_dev.x[0]*(50.0-10.0)/300.0+10.0;
							if (pwm_duty[2] <= 100)
								Servo_PWM_SetDuty(3, pwm_duty[2]);
						}
						break;
					case key7:
						Input_DoubleValue(&pwm_duty[3],"Servo4");
						if (pwm_duty[3] <= 100)
							Servo_PWM_SetDuty(4, pwm_duty[3]);
						break;
					case key8:
						if (tp_dev.x[0] < 300)
						{
							touch_pos[3] = tp_dev.x[0];
							pwm_duty[3] = tp_dev.x[0]*(32.0-15.0)/300.0+15.0;
							if (pwm_duty[3] <= 100)
								Servo_PWM_SetDuty(4, pwm_duty[3]);
						}
						break;
					case keyback:
						return;
					case keyboardtab:
						KeyBoard_State = !KeyBoard_State;
						Show_Keyboard();
						break;
					default:
						break;
				}
				Show_Keyboard();
				POINT_COLOR=BLACK;
				for (i = 0; i < 4; ++i)
				{
					LCD_printf(0,6+36*2*i,300,24,24,"%d.Servo%d Duty = %lf",i+1,i+1,Servo_PWM_GetDuty(i+1));
				}
				for (i = 0; i < 4; ++i)
				{
					LCD_DrawRectangle(0,6+36*(2*i+1)+12-2,300,6+36*(2*i+1)+12+2);
					LCD_DrawRectangle(touch_pos[i],6+36*(2*i+1),touch_pos[i]+20,6+36*(2*i+1)+24);
				}
			#ifndef USING_TOUCH_SCREEN
			#else
			}
			#endif
		}
		else
			delay_ms(1);
	}
}

void route_Motor(void)
{
	u8 i = 0, tp_last, key_value;
	u8 mecanum_cnt = 0, mecanum_state = 0;
	double cov = 60.0/(2*pi);//角速度和转速的转化系数
	double mecanum_gyro_radian_st[5] = {0}, mecanum_gyro_radian_ed[5] = {0}, mecanum_time[5] = {0}, wheel_speed[5] = {0};
	
	for (i = 0; i < 5; ++i)
	{
		KeyBoard_State = 0;
		Show_Keyboard();
		POINT_COLOR=BLACK;
		LCD_printf(0,6+36*0,300,24,24,"If sure, touch the screen");
		LCD_printf(0,6+36*1,300,24,24,"To rotate using %d%% PWM",10+i);
		
		while (1)
		{
			#ifndef USING_TOUCH_SCREEN
			if (USART_Key_Receive)
			{
				USART_Key_Receive = 0;
				key_value = Key_RxBuffer[0];
			#else
			tp_last = tp_dev.sta&TP_PRES_DOWN;
			tp_dev.scan(0); 		 
			if ((tp_dev.sta&TP_PRES_DOWN) && !tp_last)
			{			//触摸屏被按下	
				if (tp_dev.x[0]<lcddev.width&&tp_dev.y[0]<lcddev.height)
				{
					key_value = TP_Row_Judge(tp_dev.x[0], tp_dev.y[0]);
			#endif
					if (key_value == keyback)
						return;
					else if (key_value == keyboardtab)
					{
						KeyBoard_State = !KeyBoard_State;
						Show_Keyboard();
					}
					else
						break;
				#ifndef USING_TOUCH_SCREEN
				#else
				}
				#endif
			}
			else
				delay_ms(1);
		}
		
		TIM_Cmd(TIM12, DISABLE);
		mecanum_cnt = mecanum_state = 0;
		TIM12->CNT = 0;
		TIM12_OVERFLOW_CNT = 0;
		PWM_SetDuty(1, 10+i, -1);
		PWM_SetDuty(2, 10+i, -1);
		PWM_SetDuty(3, 10+i, -1);
		PWM_SetDuty(4, 10+i, -1);
		delay_ms(500);
		
		Show_Keyboard();
		while (-Gyro_Angle_Total > -90)
		{
//			LCD_printf(0,6+36*0,300,24,24,"1.Angle:%lf",Gyro_Angle_Total);
		}
		while (1)
		{
//			LCD_printf(0,6+36*0,300,24,24,"1.Angle:%lf",Gyro_Angle_Total);
			if (-Gyro_Angle_Total >= 90)
			{
				if (!mecanum_state)
				{
					TIM_Cmd(TIM12, ENABLE);
					++mecanum_cnt;
					mecanum_state = 1;
					mecanum_gyro_radian_st[i] = Gyro_Radian_Total;
					break;
				}
			}
		}
		
		Show_Keyboard();
		while (1)
		{
			if (-Gyro_Angle_Total >= 90)
			{
				if (!mecanum_state)
				{
					++mecanum_cnt;
					mecanum_state = 1;
				}
			}
			else if (-Gyro_Angle_Total <= -90)
			{
				if (mecanum_state)
				{
					++mecanum_cnt;
					mecanum_state = 0;
				}
			}
			if (mecanum_cnt > 10)
			{
				mecanum_gyro_radian_ed[i] = Gyro_Radian_Total;
				mecanum_time[i] = (TIM12_OVERFLOW_CNT * (1ULL + TIM12->ARR) + TIM12->CNT)*1.0 / (1ULL + TIM12->ARR);
				TIM_Cmd(TIM12, DISABLE);
				SPEED_STOP;
				SetSpeed(Speed_X,Speed_Y,Speed_Rotation);
				delay_ms(1000);
				break;
			}
//			LCD_printf(0,6+36*0,300,24,24,"1.Angle:%lf",Gyro_Angle_Total);
			LCD_printf(0,6+36*1,300,24,24,"2.Mecanum CNT:%u",mecanum_cnt);
//			LCD_printf(0,6+36*2,300,24,24,"3.Over CNT:%u",TIM12_OVERFLOW_CNT);
//			#ifndef USING_TOUCH_SCREEN
//			if (USART_Key_Receive)
//			{
//				USART_Key_Receive = 0;
//				key_value = Key_RxBuffer[0];
//			#else
//			tp_last = tp_dev.sta&TP_PRES_DOWN;
//			tp_dev.scan(0); 		 
//			if ((tp_dev.sta&TP_PRES_DOWN) && !tp_last)
//			{			//触摸屏被按下	
//				if (tp_dev.x[0]<lcddev.width&&tp_dev.y[0]<lcddev.height)
//				{
//					key_value = TP_Row_Judge(tp_dev.x[0], tp_dev.y[0]);
//			#endif
//					if (key_value == keyback)
//						return;
//					else if (key_value == keyboardtab)
//					{
//						KeyBoard_State = !KeyBoard_State;
//						Show_Keyboard();
//					}
//					else
//					{
//						SPEED_STOP;
//						SetSpeed(Speed_X,Speed_Y,Speed_Rotation);
//						break;
//					}
//				#ifndef USING_TOUCH_SCREEN
//				#else
//				}
//				#endif
//			}
		}
		wheel_speed[i] = (-10*pi+mecanum_gyro_radian_ed[i]-mecanum_gyro_radian_st[i])/mecanum_time[i]*Mec_Len_FL/Mec_wheel_r_FL*cov;
		
		KeyBoard_State = 0;
		Show_Keyboard();
		POINT_COLOR=BLACK;
		LCD_printf(0,6+36*0,300,24,24,"1.Speed/rpm:%lf",wheel_speed[i]);
		LCD_printf(0,6+36*1,300,24,24,"2.gyro_ed:%lf",mecanum_gyro_radian_ed[i]*RAD2DEG);
		LCD_printf(0,6+36*2,300,24,24,"3.gyro_st:%lf",mecanum_gyro_radian_st[i]*RAD2DEG);
		LCD_printf(0,6+36*3,300,24,24,"4.time:%lf",mecanum_time[i]);
		LCD_printf(0,6+36*4,300,24,24,"Touch to view next page");
		while (1)
		{
			#ifndef USING_TOUCH_SCREEN
			if (USART_Key_Receive)
			{
				USART_Key_Receive = 0;
				key_value = Key_RxBuffer[0];
			#else
			tp_last = tp_dev.sta&TP_PRES_DOWN;
			tp_dev.scan(0); 		 
			if ((tp_dev.sta&TP_PRES_DOWN) && !tp_last)
			{			//触摸屏被按下	
				if (tp_dev.x[0]<lcddev.width&&tp_dev.y[0]<lcddev.height)
				{
					key_value = TP_Row_Judge(tp_dev.x[0], tp_dev.y[0]);
			#endif
					if (key_value == keyback)
						return;
					else if (key_value == keyboardtab)
					{
						KeyBoard_State = !KeyBoard_State;
						Show_Keyboard();
					}
					else
						break;
				#ifndef USING_TOUCH_SCREEN
				#else
				}
				#endif
			}
			else
				delay_ms(1);
		}
	}
}
