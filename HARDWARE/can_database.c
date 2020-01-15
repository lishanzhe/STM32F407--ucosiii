
#include "all.h"

uint8_t Can_Data_Num=0;
union fi64_to_u8 CtrlCurrent;
union fi64_to_u8 Motor2Stm[4];
union fi32_to_u8 Angle;
union u16_to_u8 Angle_up;
union u16_to_u8 Angle_down;
union fi64_to_u8 MISO_GYRO_RAD;
uint8_t MR2_StartFlag;
uint8_t MR2_RiseFlag;
uint8_t Cylinder_Flag=0;
float Ang_up;
float Ang_down;
uint8_t MOSI_CMD;
uint8_t MISO_CMD;
#define CAMERA_LENGTH 8


union fi64_to_u8 MOSI_GPS_X;
union fi64_to_u8 MOSI_GPS_Y;
union fi64_to_u8 MOSI_GPS_ANG;//发过去的其实是弧度
union fi64_to_u8 MOSI_GYRO_C1;
union fi64_to_u8 MOSI_GYRO_C2;
union fi64_to_u8 MOSI_ENC_COEFF;

union fi64_to_u8 MISO_GPS_X;
union fi64_to_u8 MISO_GPS_Y;
union fi64_to_u8 MISO_GPS_ANG;//接收的也是弧度
union fi64_to_u8 MISO_GYRO_C1;
union fi64_to_u8 MISO_GYRO_C2;
union fi64_to_u8 MISO_GYRO_RAD;
union fi64_to_u8 MISO_ENC1_C1;
union fi64_to_u8 MISO_ENC1_C2;
union fi64_to_u8 MISO_ENC1_RAD;
union fi64_to_u8 MISO_ENC1_ANG;
union fi64_to_u8 MISO_ENC1_DIS;
union fi64_to_u8 MISO_ENC2_C1;
union fi64_to_u8 MISO_ENC2_C2;
union fi64_to_u8 MISO_ENC2_RAD;
union fi64_to_u8 MISO_ENC2_ANG;
union fi64_to_u8 MISO_ENC2_DIS;
union fi64_to_u8 MISO_GYRO_FLOAT;
union fi64_to_u8 MISO_GPS_ERROR;


union fi64_to_u8 M0SI_GPS;

//SLAVER--GPS
//新加ID时，在ID_NUMDEF中定义相应ID的意义
/**/
void White_handler1(void){}
void White_handler2(void){}
void White_handler3(void){}
void White_handler4(void){}
void PID_Ang (void){}
void PID_Ang_up(void)
{
	Ang_up = (float)((Angle_up.u8_data[1] << 8) | (Angle_up.u8_data[0]))/32768*180;
	if(Ang_up > 180.0f)
			Ang_up = Ang_up - 360.0f;
}
void PID_Ang_down(void)
{

	Ang_down = (float)((Angle_down.u8_data[1] << 8) | (Angle_down.u8_data[0]))/32768*180;
	if(Ang_down > 180.0f)
			Ang_down = Ang_down - 360.0f;
}
void DOG_GO(void){}
void BLANK_FUNC(void){}
int BALL_X_SHIFTED=0;
int BALL_Y_SHIFTED=0;
void BALL_X_HANDLER(void){BALL_X_SHIFTED=1;}
void BALL_Y_HANDLER(void){BALL_Y_SHIFTED=1;}
uint8_t RestartFlag = 0;
Can_Data Can_Database[]=
{
		{WRITE_ONLY,    ControlMotorID,		(uint8_t*)(&CtrlCurrent.u8_data),  			8,         	NULL,            2,        CAN_Filter_FIFO0},
		{READ_ONLY,     MOTOR1,            	(uint8_t*)(&Motor2Stm[0].u8_data), 			8,          PID_01,          2,        CAN_Filter_FIFO1},
		{READ_ONLY,     MOTOR2,            	(uint8_t*)(&Motor2Stm[1].u8_data), 			8,          PID_02,          2,        CAN_Filter_FIFO0},
		{READ_ONLY,     MOTOR3,            	(uint8_t*)(&Motor2Stm[2].u8_data), 			8,          PID_03,          2,        CAN_Filter_FIFO1},
		{READ_ONLY,     MOTOR4,            	(uint8_t*)(&Motor2Stm[3].u8_data),			8,          PID_04,          2,        CAN_Filter_FIFO0},
	
		{WRITE_ONLY,        M_G_CMD_ID,                         (uint8_t *)(&MOSI_CMD),                         1,                  NULL,                   1,          CAN_Filter_FIFO0},
    {WRITE_ONLY,        W_GPS_X_ID,                         (uint8_t *)(MOSI_GPS_X.u8_data),                8,                  NULL,                   1,          CAN_Filter_FIFO1},
    {WRITE_ONLY,        W_GPS_Y_ID,                         (uint8_t *)(MOSI_GPS_Y.u8_data),                8,                  NULL,                   1,          CAN_Filter_FIFO0},
    {WRITE_ONLY,        W_GPS_ANG_ID,                       (uint8_t *)(MOSI_GPS_ANG.u8_data),              8,                  NULL,                   1,          CAN_Filter_FIFO1},
    {WRITE_ONLY,        W_GYRO_C1_ID,                       (uint8_t *)(MOSI_GYRO_C1.u8_data),              8,                  NULL,                   1,          CAN_Filter_FIFO0},
    {WRITE_ONLY,        W_GYRO_C2_ID,                       (uint8_t *)(MOSI_GYRO_C2.u8_data),              8,                  NULL,                   1,          CAN_Filter_FIFO1},
    {WRITE_ONLY,        W_ENC_COEFF_ID,                     (uint8_t *)(MOSI_ENC_COEFF.u8_data),            8,                  NULL,                   1,          CAN_Filter_FIFO1},
                                                    
    {READ_ONLY,         G_M_CMD_ID,                         (uint8_t *)(&MISO_CMD),                         1,                  G_M_CMD_FUNC,           1,          CAN_Filter_FIFO0},
    {READ_ONLY,         S_GPS_X_ID,                         (uint8_t *)(MISO_GPS_X.u8_data),                8,                  S_GPS_X_FUNC,           1,          CAN_Filter_FIFO1},
    {READ_ONLY,         S_GPS_Y_ID,                         (uint8_t *)(MISO_GPS_Y.u8_data),                8,                  S_GPS_Y_FUNC,           1,          CAN_Filter_FIFO0},
    {READ_ONLY,         S_GPS_ANG_ID,                       (uint8_t *)(MISO_GPS_ANG.u8_data),              8,                  S_GPS_ANG_FUNC,         1,          CAN_Filter_FIFO1},
    //陀螺仪
		{READ_ONLY,         S_GYRO_C1_ID,                       (uint8_t *)(MISO_GYRO_C1.u8_data),              8,                  LedOn,         1,          CAN_Filter_FIFO0},
    {READ_ONLY,         S_GYRO_C2_ID,                       (uint8_t *)(MISO_GYRO_C2.u8_data),              8,                  S_GYRO_C2_FUNC,         1,          CAN_Filter_FIFO1},
    {READ_ONLY,         S_GYRO_RAD_ID,                      (uint8_t *)(MISO_GYRO_RAD.u8_data),             8,                  S_GYRO_RAD_FUNC,        1,          CAN_Filter_FIFO0},
    //码盘1
		{READ_ONLY,         S_ENC1_C1_ID,                       (uint8_t *)(MISO_ENC1_C1.u8_data),              8,                  S_ENC1_C1_FUNC,         1,          CAN_Filter_FIFO1},
    {READ_ONLY,         S_ENC1_C2_ID,                       (uint8_t *)(MISO_ENC1_C2.u8_data),              8,                  S_ENC1_C2_FUNC,         1,          CAN_Filter_FIFO0},
    {READ_ONLY,         S_ENC1_RAD_ID,                      (uint8_t *)(MISO_ENC1_RAD.u8_data),             8,                  S_ENC1_RAD_FUNC,        1,          CAN_Filter_FIFO1},
    {READ_ONLY,         S_ENC1_ANG_ID,                      (uint8_t *)(MISO_ENC1_ANG.u8_data),             8,                  S_ENC1_ANG_FUNC,        1,          CAN_Filter_FIFO0},
    {READ_ONLY,         S_ENC1_DIS_ID,                      (uint8_t *)(MISO_ENC1_DIS.u8_data),             8,                  S_ENC1_DIS_FUNC,        1,          CAN_Filter_FIFO1},
    //码盘2
		{READ_ONLY,         S_ENC2_C1_ID,                       (uint8_t *)(MISO_ENC2_C1.u8_data),              8,                  S_ENC2_C1_FUNC,         1,          CAN_Filter_FIFO0},
    {READ_ONLY,         S_ENC2_C2_ID,                       (uint8_t *)(MISO_ENC2_C2.u8_data),              8,                  S_ENC2_C2_FUNC,         1,          CAN_Filter_FIFO1},
    {READ_ONLY,         S_ENC2_RAD_ID,                      (uint8_t *)(MISO_ENC2_RAD.u8_data),             8,                  S_ENC2_RAD_FUNC,        1,          CAN_Filter_FIFO0},
    {READ_ONLY,         S_ENC2_ANG_ID,                      (uint8_t *)(MISO_ENC2_ANG.u8_data),             8,                  S_ENC2_ANG_FUNC,        1,          CAN_Filter_FIFO1},
    {READ_ONLY,         S_ENC2_DIS_ID,                      (uint8_t *)(MISO_ENC2_DIS.u8_data),             8,                  S_ENC2_DIS_FUNC,        1,          CAN_Filter_FIFO0},
    
		{READ_ONLY,         S_GYRO_FLOAT_ID,                    (uint8_t *)(MISO_GYRO_FLOAT.u8_data),           8,                  S_GYRO_FLOAT_FUNC,      1,          CAN_Filter_FIFO1},
    {READ_ONLY,         S_GPS_ERROR_ID,                     (uint8_t *)(MISO_GPS_ERROR.u8_data),            8,                  S_GYRO_ERROR_FUNC,      1,          CAN_Filter_FIFO1},        
        
//	{READ_ONLY,     BALL_X,       	(uint8_t*)(&ball_x.u8_data),        		2,          BALL_X_HANDLER,      2,        CAN_Filter_FIFO1},
//	{READ_ONLY,     BALL_Y,      		(uint8_t*)(&ball_y.u8_data),        	2,          BALL_Y_HANDLER,    2,        CAN_Filter_FIFO1},

	//{READ_ONLY,		MISO_START_ID,				(uint8_t*)(&Start_Flag),								1,					DOG_GO						1,				CAN_Filter_FIFO0},

};

//HASH_TABLE初始值为255，认为Can_Data_Num值小于256，即设备数少于256个
uint16_t HASH_TABLE[1000]={999};

void Hash_Table_init(void)
{
	int i;
	
	Can_Data_Num = sizeof(Can_Database) / sizeof(Can_Database[0]);
	for(i=0;i<1000;i++)
	{
		HASH_TABLE[i] = 999;
	}
	for(i=0;i<Can_Data_Num;i++)
	{
		HASH_TABLE[Can_Database[i].Data_ID] = i;
	}
}

void Write_Database(ID_NUMDEF ID_NUM)
{
	uint8_t j;
	CanTxMsg TxMessage;
	/* Check the parameters */
	if((HASH_TABLE[ID_NUM] >= Can_Data_Num)&&(Can_Database[HASH_TABLE[ID_NUM]].Data_type!=WRITE_ONLY))
	{
// 		LED4_on;
		return;
	}
	
	TxMessage.StdId=Can_Database[HASH_TABLE[ID_NUM]].Data_ID;
	TxMessage.ExtId=0;
	TxMessage.IDE=CAN_ID_STD;
	TxMessage.RTR=CAN_RTR_DATA;
	TxMessage.DLC=Can_Database[HASH_TABLE[ID_NUM]].Data_length;
	
	for(j=0;j<Can_Database[HASH_TABLE[ID_NUM]].Data_length;j++)
	{
		TxMessage.Data[j]=(*(Can_Database[HASH_TABLE[ID_NUM]].Data_ptr+j));
	}
	
	Can_SendData(&TxMessage,Can_Database[HASH_TABLE[ID_NUM]].Channel);
	
}
