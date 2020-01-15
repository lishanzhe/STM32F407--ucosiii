#ifndef __PID_H
#define __PID_H
#include "all.h"
#define FILTER_BUF_LEN		5
#define ABS(x)	( (x>0) ? (x) : (-x) )
#define Speed_Mode 0
#define Location_Mode 1 
					
typedef struct{
	int16_t	 	speed_rpm;
  int16_t  	real_current;
  int16_t  	given_current;
	uint8_t  	hall;
	uint16_t 	angle;				//abs angle range:[0,8191]
	uint16_t 	last_angle;	//abs angle range:[0,8191]
	uint16_t	offset_angle;
	int32_t		round_cnt;
	int64_t		total_angle;
	u8			buf_idx;
	u16			angle_buf[FILTER_BUF_LEN];
	u16			fited_angle;
	u32			msg_cnt;
	uint8_t temperature;
}moto_measure_t;
extern int Mode[];
enum{
    LLAST	= 0,
    LAST 	= 1,
    NOW 	= 2,
    
    POSITION_PID,
    DELTA_PID,
};
typedef struct __pid_t
{
    float p;
    float i;
    float d;
    
    float set[3];			
    float get[3];			
    float err[3];			
	
    
    float pout;							
    float iout;						
    float dout;							
    
    float pos_out;					
    float last_pos_out;				
    float delta_u;						
    float delta_out;					
    float last_delta_out;
    
	  float max_err;
	  float deadband;				
    uint32_t pid_mode;
    uint32_t MaxOutput;				
    uint32_t IntegralLimit;		
    
    void (*f_param_init)(struct __pid_t *pid,  
                    uint32_t pid_mode,
                    uint32_t maxOutput,
                    uint32_t integralLimit,
                    float p,
                    float i,
                    float d);
    void (*f_pid_reset)(struct __pid_t *pid, float p, float i, float d);		

}pid_t;
extern moto_measure_t moto_chassis[4];
extern int set_v,set_spd[4];
extern pid_t pid_spd[];
extern pid_t pid_loc[];

void PID_DJI_Handler(int i);
void PID_01(void);
void PID_02(void);
void PID_03(void);
void PID_04(void);
void PID_struct_init(
    pid_t* pid,
    uint32_t mode,
    uint32_t maxout,
    uint32_t intergral_limit,
    
    float 	kp, 
    float 	ki, 
    float 	kd);
extern int set_spd[];
extern int64_t set_loc[];
		

void PID_Init(void);		

#endif

