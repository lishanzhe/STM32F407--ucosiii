#include "all.h"
#include <stdio.h>

int intabs(int a){return a>0?a:-a;}

moto_measure_t moto_chassis[4];
int set_spd[4]={0,0,0,0};
int64_t set_loc[4]={0,0,0,0};
pid_t pid_spd[4];
uint16_t set_angle[4];
pid_t pid_spd[4];
pid_t pid_loc[4];

pid_t pid_x_spd;
pid_t pid_y_spd;
pid_t pid_x_loc;
pid_t pid_y_loc;

int set_x_spd=0;
int set_y_spd=0;
int set_x_loc;
int set_y_loc;


int Mode[4] = {	3,//main
								3,//angle
								Location_Mode,//side1
								Location_Mode};//side2

void abs_limit(float *a, float ABS_MAX){
    if(*a > ABS_MAX)
        *a = ABS_MAX;
    if(*a < -ABS_MAX)
        *a = -ABS_MAX;
}

static void pid_param_init(
    pid_t *pid, 
    uint32_t mode,
    uint32_t maxout,
    uint32_t intergral_limit,
    float 	kp, 
    float 	ki, 
    float 	kd){
    
    pid->IntegralLimit = intergral_limit;
    pid->MaxOutput = maxout;
    pid->pid_mode = mode;
    
    pid->p = kp;
    pid->i = ki;
    pid->d = kd;
    
}


float PID_DJI(pid_t *pid, float get, float set){
	pid->get[NOW] = get;
	pid->set[NOW] = set;
	pid->err[NOW] = set - get;	//set - measure
	
	
	pid->pout = pid->p * (pid->err[NOW] - pid->err[LAST]);
	pid->iout = pid->i * pid->err[NOW];
	pid->dout = pid->d * (pid->err[NOW] - 2*pid->err[LAST] + pid->err[LLAST]);

	abs_limit(&(pid->iout), pid->IntegralLimit);//???
	pid->delta_u = pid->pout + pid->iout + pid->dout;
	pid->delta_out = pid->last_delta_out + pid->delta_u;
	abs_limit(&(pid->delta_out), pid->MaxOutput);
	pid->last_delta_out = pid->delta_out;	//update last time
	
	
	pid->err[LLAST] = pid->err[LAST];
	pid->err[LAST] = pid->err[NOW];
	pid->get[LLAST] = pid->get[LAST];
	pid->get[LAST] = pid->get[NOW];
	pid->set[LLAST] = pid->set[LAST];
	pid->set[LAST] = pid->set[NOW];
	
	
	return pid->delta_out;	
}

/**
*@bref GET电机上电的相对角度
	*/
void get_total_angle(moto_measure_t *p){
	
	int res1, res2, delta;
	if(p->angle < p->last_angle){			
		res1 = p->angle + 8192 - p->last_angle;	//正转,delta=+
		res2 = p->angle - p->last_angle;				//反转	delta=-
	}else{	//angle > last
		res1 = p->angle - 8192 - p->last_angle ;//反转	delta -
		res2 = p->angle - p->last_angle;				//正转	delta +
	}
	
	if(ABS(res1)<ABS(res2))
		delta = res1;
	else
		delta = res2;

	p->total_angle += delta;
	p->last_angle = p->angle;
}

void PID_struct_init(
    pid_t* pid,
    uint32_t mode,
    uint32_t maxout,
    uint32_t intergral_limit,
    
    float 	kp, 
    float 	ki, 
    float 	kd){
    /*init function pointer*/
    pid->f_param_init = pid_param_init;
//    pid->f_pid_reset = pid_reset;
//	pid->f_cal_pid = pid_calc;	
//	pid->f_cal_sp_pid = pid_sp_calc;	//addition
		
    /*init pid param */
    pid->f_param_init(pid, mode, maxout, intergral_limit, kp, ki, kd);
}	
void set_moto_current( s16 iq1, s16 iq2, s16 iq3, s16 iq4)
{
		CtrlCurrent.u8_data[0] = iq1 >> 8;
		CtrlCurrent.u8_data[1] = iq1;
		CtrlCurrent.u8_data[2] = iq2 >> 8;
		CtrlCurrent.u8_data[3] = iq2;
		CtrlCurrent.u8_data[4] = iq3 >> 8;
		CtrlCurrent.u8_data[5] = iq3;
		CtrlCurrent.u8_data[6] = iq4 >> 8;
		CtrlCurrent.u8_data[7] = iq4;
		Write_Database(ControlMotorID);
}	
void get_moto_chassis(int i)
{
		moto_chassis[i].angle = (uint16_t)(Motor2Stm[i].u8_data[0] << 8 | Motor2Stm[i].u8_data[1]);
		moto_chassis[i].speed_rpm = (int16_t)(Motor2Stm[i].u8_data[2] << 8 | Motor2Stm[i].u8_data[3]);
		moto_chassis[i].real_current = (int16_t)(Motor2Stm[i].u8_data[4] << 8 | Motor2Stm[i].u8_data[5]);
		moto_chassis[i].temperature = (uint8_t) Motor2Stm[i].u8_data[6];
}


long int l_out;
long int ll_loc;
long int rl_loc;
float v_out;


void PID_DJI_Handler(int i)
{
	
	get_moto_chassis(i);
	get_total_angle(&moto_chassis[i]);
	if(Mode[i] == Speed_Mode)
	{	
		PID_DJI(&pid_spd[i], moto_chassis[i].speed_rpm, set_spd[i]);
	}
	else if(Mode[i] == Location_Mode)
	{
			PID_DJI(&pid_loc[i], moto_chassis[i].total_angle, set_loc[i]);
			PID_DJI(&pid_spd[i], moto_chassis[i].speed_rpm, pid_loc[i].delta_out);
	}
	set_moto_current(pid_spd[0].delta_out, 
										pid_spd[1].delta_out,
										pid_spd[2].delta_out,
										pid_spd[3].delta_out);
}


void PID_01(void){PID_DJI_Handler(0);}
void PID_02(void){PID_DJI_Handler(1);}
void PID_03(void){PID_DJI_Handler(2);}
void PID_04(void){PID_DJI_Handler(3);}

void PID_X(void)
{
	PID_DJI(&pid_x_loc, ball_x_loc, set_x_loc);
	PID_DJI(&pid_x_spd, ball_x_spd, pid_x_loc.delta_out);
	set_loc[2]=pid_x_spd.delta_out;
}

void PID_Y(void)
{
	PID_DJI(&pid_y_loc, ball_y_loc, set_y_loc);
	PID_DJI(&pid_y_spd, ball_y_spd, pid_y_loc.delta_out);
	set_loc[3]=pid_y_spd.delta_out;
}

void PID_Init(void)
{	
	PID_struct_init(&pid_x_spd, DELTA_PID, 8000, 16384,
								5.0f,	0.1f,	0.00f	);  //4 motos angular rate closeloop.  0.80f,	0.03f,	0.0015f	(超调)
	PID_struct_init(&pid_x_loc, DELTA_PID, 9600, 9600,
								0.80f,	0.003f,	0.000f	);  //4 motos angular location closeloop.}
	PID_struct_init(&pid_y_spd, DELTA_PID, 8000, 16384,
								5.0f,	0.1f,	0.00f	);  //4 motos angular rate closeloop.  0.80f,	0.03f,	0.0015f	(超调)
	PID_struct_init(&pid_y_loc, DELTA_PID, 9600, 9600,
								0.80f,	0.003f,	0.000f	);  //4 motos angular location closeloop.}
	
	PID_struct_init(&pid_spd[2], DELTA_PID, 8000, 16384,
								24.0f,	1.2f,	0.02f	);  //4 motos angular rate closeloop.  0.80f,	0.03f,	0.0015f	(超调)
	PID_struct_init(&pid_loc[2], DELTA_PID, 9600, 9600,
								0.80f,	0.003f,	0.000f	);  //4 motos angular location closeloop.}

	PID_struct_init(&pid_spd[3], DELTA_PID, 8000, 16384,
								24.0f,	1.2f,	0.02f	);  //4 motos angular rate closeloop.  0.80f,	0.03f,	0.0015f	(超调)
	PID_struct_init(&pid_loc[3], DELTA_PID, 9600, 9600,
								0.80f,	0.003f,	0.000f	);  //4 motos angular location closeloop.}	
}

void leg_act(int aim_loc_2,int aim_loc_3){
	int 
	//i,loop,
	sta_loc_2;
	
	sta_loc_2=set_loc[2];
	
	if(aim_loc_2-sta_loc_2>0)// small param
	{
		pid_loc[2].MaxOutput=6000;
		pid_loc[3].MaxOutput=6000;
		pid_loc[2].p=0.80;
		pid_loc[3].p=0.80;
		pid_loc[2].i=0.006;
		pid_loc[3].i=0.006;
	}	
	else //large param
	{
		pid_loc[2].MaxOutput=8000;
		pid_loc[3].MaxOutput=8000;
		pid_loc[2].p=0.80;
		pid_loc[3].p=0.80;
		pid_loc[2].i=0.009;
		pid_loc[3].i=0.009;
	}
	set_loc[2]=aim_loc_2;
	set_loc[3]=aim_loc_3;
	
	while(intabs(aim_loc_3-moto_chassis[3].total_angle)>5000)
		delay_ms(10);
}
