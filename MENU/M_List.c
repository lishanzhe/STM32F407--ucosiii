/* Includes ------------------------------------------------------------------*/
#include "M_List.h"
#include "M_Func.h"
#include "M_Sys.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/*菜单*/
struct M_Type MainMenu[M_NUM];
struct M_Type M_System[M_NUM];
struct M_Type *CurrentMenu = MainMenu;//初始化当前菜单为主菜单
//主菜单下的子菜单
struct M_Type MainMenu[M_NUM]=
{	/*Name              菜单属性                函数                节点*/
	{"1.System",        M_UFunc,                sys_dog,            {MainMenu, NULL} },
	{"2.FastDog",       M_Empty,                NULL,               {MainMenu, NULL} },
	{"3.Gyro",          M_UFunc,                NULL,               {MainMenu, NULL} },
	{"4.SRAM",          M_UFunc,                sys_SRAM,           {MainMenu, NULL} },
	{"5.M3508",         M_SMenu,                NULL,               {MainMenu, M_FastDog} },
	{"6.Servo",         M_UFunc,                sys_servo,          {MainMenu, NULL} },
	{"7.GPS ",          M_SMenu,                NULL,               {MainMenu, M_System} },
	{" ",               M_Empty,                NULL,               {MainMenu, NULL} },
};
//用来放系统配置函数
struct M_Type M_System[M_NUM]=
{	/*Name              菜单属性                函数                节点*/
  {"1.Gyro",          M_UFunc,            sys_Gyro,               {MainMenu, NULL} }, //陀螺仪
  {"2.Encoder",       M_UFunc,            sys_Encoder,            {MainMenu, NULL} }, //码盘
  {"3.GPS",           M_UFunc,            sys_GPS,                {MainMenu, NULL} }, //GPS
	{" ",               M_Empty,                NULL,               {MainMenu, NULL} },
	{" ",               M_Empty,                NULL,               {MainMenu, NULL} },
	{" ",               M_Empty,                NULL,               {MainMenu, NULL} },
	{" ",               M_Empty,                NULL,               {MainMenu, NULL} },
	{" ",               M_Empty,                NULL,               {MainMenu, NULL} },
};
struct M_Type M_FastDog[M_NUM]=
{	/*Name              菜单属性                函数                节点*/
	{"1.Location",      M_UFunc,                sys_loc3508,               {MainMenu, NULL} },
	{"2.Speed",      		M_UFunc,                sys_spd3508,               {MainMenu, NULL} },
	{" ",               M_Empty,                NULL,               {MainMenu, NULL} },
	{" ",               M_Empty,                NULL,               {MainMenu, NULL} },
	{" ",               M_Empty,                NULL,               {MainMenu, NULL} },
	{" ",               M_Empty,                NULL,               {MainMenu, NULL} },
	{" ",               M_Empty,                NULL,               {MainMenu, NULL} },
	{" ",               M_Empty,                NULL,               {MainMenu, NULL} },
};

/********************************    END OF FILE    ***************************/
