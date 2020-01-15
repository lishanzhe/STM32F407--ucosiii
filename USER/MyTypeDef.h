/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MYTYPEDEF_H
#define __MYTYPEDEF_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

//点结构体
struct Point
{
	double x;
	double y;
};

//路径信息结构体
struct Path_Point
{
	struct Point position;	//位置
	double speeddir;		//速度方向
	double selfdir;			//车体方向
	double speed;			//速度大小
};

//PID结构体
struct PID
{
	double p;
	double i;
	double d;
	double mend;
	double ep1;
	double ep2;
};

//每个轮子对应的状态
struct Mecanum_State
{
	uint8_t port;
	int8_t arg;				//方向系数
};

struct Color_State
{
	uint16_t rgb565;
	uint8_t r;
	uint8_t g;
	uint8_t b;
};

struct Encoder_State
{
    double dis;                //距离
    double convert1;        //正转系数
    double convert2;        //反转系数
    double r;                //车身自转时码盘的旋转半径
    double radian;  //改变正反转方向（如果是需要的方向则置1，否则置-1）
};


//CCD结构体
struct CCD_State
{	
	uint8_t ADC_Channel_Index;
	
	uint8_t Line_Left_Edge_Pos_Threshold;
	uint8_t Line_Right_Edge_Pos_Threshold;
	uint8_t Line_Median_Pos_Threshold;
	
	uint8_t Line_Left_Edge_Pos;
	uint8_t Line_Right_Edge_Pos;
	uint8_t Line_Median_Pos;
	
	uint16_t Line_ADC_Threshold;
	uint16_t Place_ADC_Threshold;
	uint16_t Difference_ADC_Threshold;
	
	uint16_t ADC_Max;
	uint16_t ADC_Min;
	uint16_t ADC_Median;
};

//方向
enum Route_Direction
{
	ROUTE_FRONT = 0,
	ROUTE_BACK,
	ROUTE_LEFT,
	ROUTE_RIGHT,
};

//按键值
enum M_KeyNumType
{
    key1 = 1,
    key2,
    key3,
    key4,
    keyback,//5
    emergency,//6
    
    key5,//7
    key6,//8
    key7,//9
    key8,//10
    keyempty1,//11
    keyempty2,//12
    
    key9,//13
    key0,
    keypoint,
    keysign,
    keydelete,
    keyOK,
	keyboardtab
};
#define pageup point
#define pagedown sign

typedef enum {
 		Type_Fp64 = 1 ,
 		Type_Int64,
 		Type_Fp32 ,
 		Type_Int32,
 		Type_Uint32,
 		Type_Int16,
 		Type_Uint16,
 		Type_Int8,
 		Type_Uint8,
 		Type_Gyro,
 		Type_Encoder,
 		Type_PID,
 		Type_Mecanum,
}SD_Value_Type;


typedef enum {
		TYPE_INT8	=  1,
		TYPE_UINT8	,
		TYPE_INT16	,
		TYPE_UINT16	,
		TYPE_INT32	,
		TYPE_UINT32	,
		TYPE_INT64	,
		TYPE_UINT64	,
		TYPE_FP32		,
		TYPE_FP64		,
//     TYPE_POINTFP32,
//     TYPE_POINTFP64,
}Value_Type;

//列表显示与更改
struct Input_ListValue
{
	void *value;
	Value_Type type;
	char *name;
};

#endif 

/**********************************END OF FILE*********************************/
