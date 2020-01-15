#include "lcd.h"
#include "touch.h"
#include "M_Func.h"
#include "delay.h"
#include "MyTypeDef.h"
#include "key.h"
#include "usart.h"
#include <math.h>

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//触摸屏驱动（支持ADS7843/7846/UH7843/7846/XPT2046/TSC2046/OTT2001A等） 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/7
//版本：V1.2
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									   
//********************************************************************************
//修改说明
//V1.1 20140721
//修正MDK在-O2优化时,触摸屏数据无法读取的bug.在TP_Write_Byte函数添加一个延时,解决问题.
//V1.2 20141130 
//电容触摸屏增加FT5206的支持
//////////////////////////////////////////////////////////////////////////////////

_m_tp_dev tp_dev=
{
	TP_Init,
	TP_Scan,
	TP_Adjust,
	0,
	0, 
	0,
	0,
	0,
	0,	  	 		
	0,
	0,	  	 		
};					
//默认为touchtype=0的数据.
u8 CMD_RDX=0XD0;
u8 CMD_RDY=0X90;

u8 use_touch_adj_data = 1;
 	 			    					   
//SPI写数据
//向触摸屏IC写入1byte数据    
//num:要写入的数据
void TP_Write_Byte(u8 num)    
{  
	u8 count=0;   
	for(count=0;count<8;count++)  
	{ 	  
		if(num&0x80)TDIN=1;  
		else TDIN=0;   
		num<<=1;    
		TCLK=0; 
		delay_us(1);
		TCLK=1;		//上升沿有效	        
	}		 			    
} 		 
//SPI读数据 
//从触摸屏IC读取adc值
//CMD:指令
//返回值:读到的数据	   
u16 TP_Read_AD(u8 CMD)	  
{ 	 
	u8 count=0; 	  
	u16 Num=0; 
	TCLK=0;		//先拉低时钟 	 
	TDIN=0; 	//拉低数据线
	TCS=0; 		//选中触摸屏IC
	TP_Write_Byte(CMD);//发送命令字
	delay_us(6);//ADS7846的转换时间最长为6us
	TCLK=0; 	     	    
	delay_us(1);    	   
	TCLK=1;		//给1个时钟，清除BUSY
	delay_us(1);    
	TCLK=0; 	     	    
	for(count=0;count<16;count++)//读出16位数据,只有高12位有效 
	{ 				  
		Num<<=1; 	 
		TCLK=0;	//下降沿有效  	    	   
		delay_us(1);    
 		TCLK=1;
 		if(DOUT)Num++; 		 
	}  	
	Num>>=4;   	//只有高12位有效.
	TCS=1;		//释放片选	 
	return(Num);   
}
//读取一个坐标值(x或者y)
//连续读取READ_TIMES次数据,对这些数据升序排列,
//然后去掉最低和最高LOST_VAL个数,取平均值 
//xy:指令（CMD_RDX/CMD_RDY）
//返回值:读到的数据
#define READ_TIMES 5 	//读取次数
#define LOST_VAL 1	  	//丢弃值
u16 TP_Read_XOY(u8 xy)
{
	u16 i, j;
	u16 buf[READ_TIMES];
	u16 sum=0;
	u16 temp;
	for(i=0;i<READ_TIMES;i++)buf[i]=TP_Read_AD(xy);		 		    
	for(i=0;i<READ_TIMES-1; i++)//排序
	{
		for(j=i+1;j<READ_TIMES;j++)
		{
			if(buf[i]>buf[j])//升序排列
			{
				temp=buf[i];
				buf[i]=buf[j];
				buf[j]=temp;
			}
		}
	}	  
	sum=0;
	for(i=LOST_VAL;i<READ_TIMES-LOST_VAL;i++)sum+=buf[i];
	temp=sum/(READ_TIMES-2*LOST_VAL);
	return temp;   
} 
//读取x,y坐标
//最小值不能少于100.
//x,y:读取到的坐标值
//返回值:0,失败;1,成功。
u8 TP_Read_XY(u16 *x,u16 *y)
{
	u16 xtemp,ytemp;			 	 		  
	xtemp=TP_Read_XOY(CMD_RDX);
	ytemp=TP_Read_XOY(CMD_RDY);	  												   
	//if(xtemp<100||ytemp<100)return 0;//读数失败
	*x=xtemp;
	*y=ytemp;
	return 1;//读数成功
}
//连续2次读取触摸屏IC,且这两次的偏差不能超过
//ERR_RANGE,满足条件,则认为读数正确,否则读数错误.	   
//该函数能大大提高准确度
//x,y:读取到的坐标值
//返回值:0,失败;1,成功。
#define ERR_RANGE 50 //误差范围 
u8 TP_Read_XY2(u16 *x,u16 *y) 
{
	u16 x1,y1;
 	u16 x2,y2;
 	u8 flag;    
    flag=TP_Read_XY(&x1,&y1);   
    if(flag==0)return(0);
    flag=TP_Read_XY(&x2,&y2);	   
    if(flag==0)return(0);   
    if(((x2<=x1&&x1<x2+ERR_RANGE)||(x1<=x2&&x2<x1+ERR_RANGE))//前后两次采样在+-50内
    &&((y2<=y1&&y1<y2+ERR_RANGE)||(y1<=y2&&y2<y1+ERR_RANGE)))
    {
        *x=(x1+x2)/2;
        *y=(y1+y2)/2;
        return 1;
    }else return 0;	  
}  
//////////////////////////////////////////////////////////////////////////////////		  
//与LCD部分有关的函数  
//画一个触摸点
//用来校准用的
//x,y:坐标
//color:颜色
void TP_Drow_Touch_Point(u16 x,u16 y,u16 color)
{
	POINT_COLOR=color;
	LCD_DrawLine(x-12,y,x+13,y);//横线
	LCD_DrawLine(x,y-12,x,y+13);//竖线
	LCD_DrawPoint(x+1,y+1);
	LCD_DrawPoint(x-1,y+1);
	LCD_DrawPoint(x+1,y-1);
	LCD_DrawPoint(x-1,y-1);
	LCD_Draw_Circle(x,y,6);//画中心圈
}	  
//画一个大点(2*2的点)		   
//x,y:坐标
//color:颜色
void TP_Draw_Big_Point(u16 x,u16 y,u16 color)
{	    
	POINT_COLOR=color;
	LCD_DrawPoint(x,y);//中心点 
	LCD_DrawPoint(x+1,y);
	LCD_DrawPoint(x,y+1);
	LCD_DrawPoint(x+1,y+1);	 	  	
}						  
//////////////////////////////////////////////////////////////////////////////////		  
//触摸按键扫描
//tp:0,屏幕坐标;1,物理坐标(校准等特殊场合用)
//返回值:当前触屏状态.
//0,触屏无触摸;1,触屏有触摸
u8 TP_Scan(u8 tp)
{			   
	if(PEN==0)//有按键按下
	{
		if(tp)TP_Read_XY2(&tp_dev.x[0],&tp_dev.y[0]);//读取物理坐标
		else if(TP_Read_XY2(&tp_dev.x[0],&tp_dev.y[0]))//读取屏幕坐标
		{
	 		tp_dev.x[0]=tp_dev.xfac*tp_dev.x[0]+tp_dev.xoff;//将结果转换为屏幕坐标
			tp_dev.y[0]=tp_dev.yfac*tp_dev.y[0]+tp_dev.yoff;  
	 	} 
		if((tp_dev.sta&TP_PRES_DOWN)==0)//之前没有被按下
		{		 
			tp_dev.sta=TP_PRES_DOWN|TP_CATH_PRES;//按键按下  
			tp_dev.x[4]=tp_dev.x[0];//记录第一次按下时的坐标
			tp_dev.y[4]=tp_dev.y[0];  	   			 
		}			   
	}else
	{
		if(tp_dev.sta&TP_PRES_DOWN)//之前是被按下的
		{
			tp_dev.sta&=~(1<<7);//标记按键松开	
		}else//之前就没有被按下
		{
			tp_dev.x[4]=0;
			tp_dev.y[4]=0;
			tp_dev.x[0]=0xffff;
			tp_dev.y[0]=0xffff;
		}	    
	}
	return tp_dev.sta&TP_PRES_DOWN;//返回当前的触屏状态
}	  
//////////////////////////////////////////////////////////////////////////	 
////保存在EEPROM里面的地址区间基址,占用13个字节(RANGE:SAVE_ADDR_BASE~SAVE_ADDR_BASE+12)
//#define SAVE_ADDR_BASE 40
////保存校准参数										    
//void TP_Save_Adjdata(void)
//{
//	s32 temp;			 
//	//保存校正结果!		   							  
//	temp=tp_dev.xfac*100000000;//保存x校正因素      
//    AT24CXX_WriteLenByte(SAVE_ADDR_BASE,temp,4);   
//	temp=tp_dev.yfac*100000000;//保存y校正因素    
//    AT24CXX_WriteLenByte(SAVE_ADDR_BASE+4,temp,4);
//	//保存x偏移量
//    AT24CXX_WriteLenByte(SAVE_ADDR_BASE+8,tp_dev.xoff,2);		    
//	//保存y偏移量
//	AT24CXX_WriteLenByte(SAVE_ADDR_BASE+10,tp_dev.yoff,2);	
//	//保存触屏类型
//	AT24CXX_WriteOneByte(SAVE_ADDR_BASE+12,tp_dev.touchtype);	
//	temp=0X0A;//标记校准过了
//	AT24CXX_WriteOneByte(SAVE_ADDR_BASE+13,temp); 
//}
//得到保存在EEPROM里面的校准值
//返回值：1，成功获取数据
//        0，获取失败，要重新校准
//u8 TP_Get_Adjdata(void)
//{					  
//	s32 tempfac;
//	tempfac=AT24CXX_ReadOneByte(SAVE_ADDR_BASE+13);//读取标记字,看是否校准过！ 		 
//	if(tempfac==0X0A)//触摸屏已经校准过了			   
//	{    												 
//		tempfac=AT24CXX_ReadLenByte(SAVE_ADDR_BASE,4);		   
//		tp_dev.xfac=(float)tempfac/100000000;//得到x校准参数
//		tempfac=AT24CXX_ReadLenByte(SAVE_ADDR_BASE+4,4);			          
//		tp_dev.yfac=(float)tempfac/100000000;//得到y校准参数
//	    //得到x偏移量
//		tp_dev.xoff=AT24CXX_ReadLenByte(SAVE_ADDR_BASE+8,2);			   	  
// 	    //得到y偏移量
//		tp_dev.yoff=AT24CXX_ReadLenByte(SAVE_ADDR_BASE+10,2);				 	  
// 		tp_dev.touchtype=AT24CXX_ReadOneByte(SAVE_ADDR_BASE+12);//读取触屏类型标记
//		if(tp_dev.touchtype)//X,Y方向与屏幕相反
//		{
//			CMD_RDX=0X90;
//			CMD_RDY=0XD0;	 
//		}else				   //X,Y方向与屏幕相同
//		{
//			CMD_RDX=0XD0;
//			CMD_RDY=0X90;	 
//		}		 
//		return 1;	 
//	}
//	return 0;
//}	 
//提示字符串
u8* const TP_REMIND_MSG_TBL="Please use the stylus click the cross on the screen.The cross will always move until the screen adjustment is completed.";
 					  
//提示校准结果(各个参数)
void TP_Adj_Info_Show(u16 x0,u16 y0,u16 x1,u16 y1,u16 x2,u16 y2,u16 x3,u16 y3,u16 fac)
{	  
	POINT_COLOR=RED;
	LCD_ShowString(40,160,lcddev.width,lcddev.height,16,"x1:");
 	LCD_ShowString(40+80,160,lcddev.width,lcddev.height,16,"y1:");
 	LCD_ShowString(40,180,lcddev.width,lcddev.height,16,"x2:");
 	LCD_ShowString(40+80,180,lcddev.width,lcddev.height,16,"y2:");
	LCD_ShowString(40,200,lcddev.width,lcddev.height,16,"x3:");
 	LCD_ShowString(40+80,200,lcddev.width,lcddev.height,16,"y3:");
	LCD_ShowString(40,220,lcddev.width,lcddev.height,16,"x4:");
 	LCD_ShowString(40+80,220,lcddev.width,lcddev.height,16,"y4:");  
 	LCD_ShowString(40,240,lcddev.width,lcddev.height,16,"fac is:");     
	LCD_ShowNum(40+24,160,x0,4,16);		//显示数值
	LCD_ShowNum(40+24+80,160,y0,4,16);	//显示数值
	LCD_ShowNum(40+24,180,x1,4,16);		//显示数值
	LCD_ShowNum(40+24+80,180,y1,4,16);	//显示数值
	LCD_ShowNum(40+24,200,x2,4,16);		//显示数值
	LCD_ShowNum(40+24+80,200,y2,4,16);	//显示数值
	LCD_ShowNum(40+24,220,x3,4,16);		//显示数值
	LCD_ShowNum(40+24+80,220,y3,4,16);	//显示数值
 	LCD_ShowNum(40+56,240,fac,3,16); 	//显示数值,该数值必须在95~105范围之内.

}

void TP_Adj_Parameters_Show(void)//显示校准参数
{
	POINT_COLOR=BLACK;
	
	LCD_printf(30,40,200,24,24,"xfac:%.10lf",tp_dev.xfac);
	LCD_printf(30,70,200,24,24,"yfac:%.10lf",tp_dev.yfac);
	LCD_printf(30,100,200,24,24,"xoff:%d",tp_dev.xoff);
	LCD_printf(30,130,200,24,24,"yoff:%d",tp_dev.yoff);
	LCD_printf(30,160,200,24,24,"touchtype:%d",tp_dev.touchtype);
}

void TP_Adj_Parameters_Set(void)//设置校准参数
{
//	tp_dev.xfac = -0.085261873;
//	tp_dev.yfac = -0.127167627;
//	tp_dev.xoff = 335;
//	tp_dev.yoff = 497;    //正
	tp_dev.xfac = 0.0851581544;
	tp_dev.yfac = 0.1263278723;
	tp_dev.xoff = -18;
	tp_dev.yoff = -17;      //反
	tp_dev.touchtype = 0;
	
	if(tp_dev.touchtype)//X,Y方向与屏幕相反
	{
		CMD_RDX=0X90;
		CMD_RDY=0XD0;	 
	}else				   //X,Y方向与屏幕相同
	{
		CMD_RDX=0XD0;
		CMD_RDY=0X90;	 
	}
}
		 
//触摸屏校准代码
//得到四个校准参数
void TP_Adjust(void)
{								 
	u16 pos_temp[4][2];//坐标缓存值
	u8  cnt=0;	
	u16 d1,d2;
	u32 tem1,tem2;
	double fac; 	
	u16 outtime=0;
 	cnt=0;				
	POINT_COLOR=BLUE;
	BACK_COLOR =WHITE;
	LCD_Clear(WHITE);//清屏   
	POINT_COLOR=RED;//红色 
	LCD_Clear(WHITE);//清屏 	   
	POINT_COLOR=BLACK;
	LCD_ShowString(40,40,160,100,16,(u8*)TP_REMIND_MSG_TBL);//显示提示信息
	TP_Drow_Touch_Point(20,20,RED);//画点1 
	tp_dev.sta=0;//消除触发信号 
	tp_dev.xfac=0;//xfac用来标记是否校准过,所以校准之前必须清掉!以免错误	 
	while(1)//如果连续10秒钟没有按下,则自动退出
	{
		tp_dev.scan(1);//扫描物理坐标
		if((tp_dev.sta&0xc0)==TP_CATH_PRES)//按键按下了一次(此时按键松开了.)
		{	
			outtime=0;		
			tp_dev.sta&=~(1<<6);//标记按键已经被处理过了.
						   			   
			pos_temp[cnt][0]=tp_dev.x[0];
			pos_temp[cnt][1]=tp_dev.y[0];
			cnt++;	  
			switch(cnt)
			{			   
				case 1:						 
					TP_Drow_Touch_Point(20,20,WHITE);				//清除点1 
					TP_Drow_Touch_Point(lcddev.width-20,20,RED);	//画点2
					break;
				case 2:
 					TP_Drow_Touch_Point(lcddev.width-20,20,WHITE);	//清除点2
					TP_Drow_Touch_Point(20,lcddev.height-20,RED);	//画点3
					break;
				case 3:
 					TP_Drow_Touch_Point(20,lcddev.height-20,WHITE);			//清除点3
 					TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,RED);	//画点4
					break;
				case 4:	 //全部四个点已经得到
	    		    //对边相等
					tem1=abs(pos_temp[0][0]-pos_temp[1][0]);//x1-x2
					tem2=abs(pos_temp[0][1]-pos_temp[1][1]);//y1-y2
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//得到1,2的距离
					
					tem1=abs(pos_temp[2][0]-pos_temp[3][0]);//x3-x4
					tem2=abs(pos_temp[2][1]-pos_temp[3][1]);//y3-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//得到3,4的距离
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05||d1==0||d2==0)//不合格
					{
						cnt=0;
 				    	TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,WHITE);	//清除点4
   	 					TP_Drow_Touch_Point(20,20,RED);								//画点1
 						TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//显示数据   
 						continue;
					}
					tem1=abs(pos_temp[0][0]-pos_temp[2][0]);//x1-x3
					tem2=abs(pos_temp[0][1]-pos_temp[2][1]);//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//得到1,3的距离
					
					tem1=abs(pos_temp[1][0]-pos_temp[3][0]);//x2-x4
					tem2=abs(pos_temp[1][1]-pos_temp[3][1]);//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//得到2,4的距离
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05)//不合格
					{
						cnt=0;
 				    	TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,WHITE);	//清除点4
   	 					TP_Drow_Touch_Point(20,20,RED);								//画点1
 						TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//显示数据   
						continue;
					}//正确了
								   
					//对角线相等
					tem1=abs(pos_temp[1][0]-pos_temp[2][0]);//x1-x3
					tem2=abs(pos_temp[1][1]-pos_temp[2][1]);//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//得到1,4的距离
	
					tem1=abs(pos_temp[0][0]-pos_temp[3][0]);//x2-x4
					tem2=abs(pos_temp[0][1]-pos_temp[3][1]);//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//得到2,3的距离
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05)//不合格
					{
						cnt=0;
 				    	TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,WHITE);	//清除点4
   	 					TP_Drow_Touch_Point(20,20,RED);								//画点1
 						TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//显示数据   
						continue;
					}//正确了
					//计算结果
					tp_dev.xfac=(float)(lcddev.width-40)/(pos_temp[1][0]-pos_temp[0][0]);//得到xfac		 
					tp_dev.xoff=(lcddev.width-tp_dev.xfac*(pos_temp[1][0]+pos_temp[0][0]))/2;//得到xoff
						  
					tp_dev.yfac=(float)(lcddev.height-40)/(pos_temp[2][1]-pos_temp[0][1]);//得到yfac
					tp_dev.yoff=(lcddev.height-tp_dev.yfac*(pos_temp[2][1]+pos_temp[0][1]))/2;//得到yoff  
					if(abs(tp_dev.xfac)>2||abs(tp_dev.yfac)>2)//触屏和预设的相反了.
					{
						cnt=0;
 				    	TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,WHITE);	//清除点4
   	 					TP_Drow_Touch_Point(20,20,RED);								//画点1
						LCD_ShowString(40,26,lcddev.width,lcddev.height,16,"TP Need readjust!");
						tp_dev.touchtype=!tp_dev.touchtype;//修改触屏类型.
						if(tp_dev.touchtype)//X,Y方向与屏幕相反
						{
							CMD_RDX=0X90;
							CMD_RDY=0XD0;	 
						}else				   //X,Y方向与屏幕相同
						{
							CMD_RDX=0XD0;
							CMD_RDY=0X90;	 
						}			    
						continue;
					}		
					POINT_COLOR=BLUE;
					LCD_Clear(WHITE);//清屏
					LCD_ShowString(35,110,lcddev.width,lcddev.height,16,"Touch Screen Adjust OK!");//校正完成
					delay_ms(1000);
//					TP_Save_Adjdata();  
 					LCD_Clear(WHITE);//清屏   
					return;//校正完成				 
			}
		}
		delay_ms(10);
		outtime++;
		if(outtime>10000)
		{
//			TP_Get_Adjdata();
			break;
	 	} 
 	}
}	
//触摸屏初始化  		    
//返回值:0,没有进行校准
//       1,进行过校准
u8 TP_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;	
	
//	if(lcddev.id==0X5510)		//电容触摸屏
//	{
//		if(GT9147_Init()==0)	//是GT9147
//		{ 
//			tp_dev.scan=GT9147_Scan;	//扫描函数指向GT9147触摸屏扫描
//		}else
//		{
//			OTT2001A_Init();
//			tp_dev.scan=OTT2001A_Scan;	//扫描函数指向OTT2001A触摸屏扫描
//		}
//		tp_dev.touchtype|=0X80;	//电容屏 
//		tp_dev.touchtype|=lcddev.dir&0X01;//横屏还是竖屏 
//		return 0;
//	}else if(lcddev.id==0X1963)
//	{
//		FT5206_Init();
//		tp_dev.scan=FT5206_Scan;		//扫描函数指向GT9147触摸屏扫描		
//		tp_dev.touchtype|=0X80;			//电容屏 
//		tp_dev.touchtype|=lcddev.dir&0X01;//横屏还是竖屏 
//		return 0;
//	}else
//	{
		
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOF, ENABLE);//使能GPIOB,C,F时钟

    //GPIOB1,2初始化设置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;//PB1/PB2 设置为上拉输入
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//输入模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
    GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//PB0设置为推挽输出
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//输出模式
	  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;//PC13设置为推挽输出
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//输出模式
	  GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化	
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PF11设置推挽输出
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//输出模式
	  GPIO_Init(GPIOF, &GPIO_InitStructure);//初始化			
		
   
		TP_Read_XY(&tp_dev.x[0],&tp_dev.y[0]);//第一次读取初始化	 
//		AT24CXX_Init();		//初始化24CXX
//		if(TP_Get_Adjdata())return 0;//已经校准
//		else			   //未校准?
//		{ 										    
//			LCD_Clear(WHITE);//清屏
//			TP_Adjust();  	//屏幕校准 
//			TP_Save_Adjdata();	 
//		}			
//		TP_Get_Adjdata();
		if (use_touch_adj_data)
		{
			TP_Adj_Parameters_Set();
		}
		else
		{
			LCD_Clear(WHITE);//清屏
			TP_Adjust();  	//屏幕校准
			TP_Adj_Parameters_Show();
			delay_ms(1000);
			delay_ms(1000);
			delay_ms(1000);
		}
//	}
	return 1; 									 
}

u8 TP_MainMenu_Judge(u16 x, u16 y)
{
	if (KeyBoard_State)
	{
		if (x < lcddev.width/2)
		{
			if (y < 36) return key1;
			else if (y < 36*2) return key2;
			else if (y < 36*3) return key3;
			else if (y < 36*4) return key4;
		}
		else if (x < lcddev.width)
		{
			if (y < 36) return key5;
			else if (y < 36*2) return key6;
			else if (y < 36*3) return key7;
			else if (y < 36*4) return key8;
			else
			{
				if (x > 3*lcddev.width/4)
				{
					if (y > lcddev.height-1-48)
						return keyboardtab;
					else if (y > lcddev.height-1-2*48)
						return keyback;
				}
			}
		}
	}
	else
	{
		if (x < lcddev.width/2)
		{
			if (y < 36) return key1;
			else if (y < 36*2) return key2;
			else if (y < 36*3) return key3;
			else if (y < 36*4) return key4;
			else if (y > lcddev.height-1-48) return keyback;
		}
		else if (x < lcddev.width)
		{
			if (y < 36) return key5;
			else if (y < 36*2) return key6;
			else if (y < 36*3) return key7;
			else if (y < 36*4) return key8;
			else if (y > lcddev.height-1-48) return keyboardtab;
		}
	}
	return keyempty1;
}

u8 TP_Keyboard_Judge(u16 x, u16 y)
{
	if (KeyBoard_State)
	{
		if (x < lcddev.width/4)
		{
			if (y > lcddev.height-1-48) return keysign;
			else if (y > lcddev.height-1-48*2) return key1;
			else if (y > lcddev.height-1-48*3) return key4;
			else if (y > lcddev.height-1-48*4) return key7;
		}
		else if (x < lcddev.width/2)
		{
			if (y > lcddev.height-1-48) return key0;
			else if (y > lcddev.height-1-48*2) return key2;
			else if (y > lcddev.height-1-48*3) return key5;
			else if (y > lcddev.height-1-48*4) return key8;
		}
		else if (x < lcddev.width*3/4)
		{
			if (y > lcddev.height-1-48) return keypoint;
			else if (y > lcddev.height-1-48*2) return key3;
			else if (y > lcddev.height-1-48*3) return key6;
			else if (y > lcddev.height-1-48*4) return key9;
		}
		else if (x < lcddev.width)
		{
			if (y > lcddev.height-1-48) return keyboardtab;
			else if (y > lcddev.height-1-48*2) return keyback;
			else if (y > lcddev.height-1-48*3) return keydelete;
			else if (y > lcddev.height-1-48*4) return keyOK;
		}
	}
	else
	{
		if (x < lcddev.width/2)
		{
			if (y > lcddev.height-1-48) return keyback;
		}
		else if (x < lcddev.width)
		{
			if (y > lcddev.height-1-48) return keyboardtab;
		}
	}
	return keyempty1;
}

u8 TP_Row_Judge(u16 x, u16 y)
{	
	if (KeyBoard_State)
	{
		if (x < lcddev.width/4)
		{
			if (y > lcddev.height-1-48) return keysign;
			else if (y > lcddev.height-1-48*2) return key1;
			else if (y > lcddev.height-1-48*3) return key4;
			else if (y > lcddev.height-1-48*4) return key7;
		}
		else if (x < lcddev.width/2)
		{
			if (y > lcddev.height-1-48) return key0;
			else if (y > lcddev.height-1-48*2) return key2;
			else if (y > lcddev.height-1-48*3) return key5;
			else if (y > lcddev.height-1-48*4) return key8;
		}
		else if (x < lcddev.width*3/4)
		{
			if (y > lcddev.height-1-48) return keypoint;
			else if (y > lcddev.height-1-48*2) return key3;
			else if (y > lcddev.height-1-48*3) return key6;
			else if (y > lcddev.height-1-48*4) return key9;
		}
		else if (x < lcddev.width)
		{
			if (y > lcddev.height-1-48) return keyboardtab;
			else if (y > lcddev.height-1-48*2) return keyback;
			else if (y > lcddev.height-1-48*3) return keydelete;
			else if (y > lcddev.height-1-48*4) return keyOK;
		}
	}
	else
	{
		if (y < 36) return key1;
		else if (y < 36*2) return key2;
		else if (y < 36*3) return key3;
		else if (y < 36*4) return key4;
		else if (y < 36*5) return key5;
		else if (y < 36*6) return key6;
		else if (y < 36*7) return key7;
		else if (y < 36*8) return key8;
		else if (y < 36*9) return key9;
		else if (y > lcddev.height-1-48 && x < lcddev.width/2) return keyback;
		else if (y > lcddev.height-1-48 && x < lcddev.width) return keyboardtab;
	}
	return keyempty1;
}

void Input_IntValue(int *address,char *name)//整型输入
{
	u8 tp_last, key_value, is_key = 0;
	u8 pre_KeyBoard_State = KeyBoard_State;
    
	char str[20]={'\0'};
	int temp;
	int i,j;

	str[0]=' ';

	i=1;
	
	KeyBoard_State = 1;
	Show_Keyboard();
	LCD_ShowString(0,6,300,24,24,(u8*)name);
	LCD_printf(0,6+36,300,24,24,"old:%d",*address);
	LCD_printf(0,6+2*36,300,24,24,"new:%s",str);
	for(;;)
	{		
		#ifdef BSP_USING_TOUCH_SCREEN
		tp_last = tp_dev.sta&TP_PRES_DOWN;
		tp_dev.scan(0);
		if ((tp_dev.sta&TP_PRES_DOWN) && !tp_last)
		{
			key_value = TP_Keyboard_Judge(tp_dev.x[0], tp_dev.y[0]);
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
				case key1://1
					if(i<14)
						str[i++]='1';
					break;
				case key2://2
					if(i<14)
						str[i++]='2';
					break;
				case key3://3
					if(i<14)
						str[i++]='3';
					break;
				case key4://4
					if(i<14)
						str[i++]='4';
					break;
				case key5://5
					if(i<14)
						str[i++]='5';
					break;
				case key6://6
					if(i<14)
						str[i++]='6';
					break;
				case key7://7
					if(i<14)
						str[i++]='7';
					break;
				case key8://8
					if(i<14)
						str[i++]='8';
					break;
				case key9://9
					if(i<14)
						str[i++]='9';
					break;
				case key0://0
					if(i<14)
						str[i++]='0';
					break;
				case keysign://-
					if(str[0]==' ')
						str[0]='-';
					else
						str[0]=' ';
					break;
				case keydelete://del
					if(i>1)
					{
						str[--i]='\0';
					}
					break;
				case keyOK://ok
					temp=0;
					for(j=1;j<i;j++)//计算输入值
					{
						temp=temp*10+str[j]-'0';
					}
					if(str[0]=='-')
							temp=-temp;
					*address=temp;
					KeyBoard_State = pre_KeyBoard_State;
					return;
				case keyback://cancel
					KeyBoard_State = pre_KeyBoard_State;
					return;
				case keyboardtab:
					KeyBoard_State = !KeyBoard_State;
					Show_Keyboard();
					LCD_ShowString(0,6,300,24,24,(u8*)name);
					LCD_printf(0,6+36,300,24,24,"old:%d",*address);
					LCD_printf(0,6+2*36,300,24,24,"new:");
					break;
			}
			LCD_Fill(4*12,6+2*36,lcddev.width-1,6+2*36+24,WHITE);
			LCD_printf(4*12,6+2*36,300,24,24,"%s",str);
		}
		else
			delay_ms(1);
	}
}

void Input_FloatValue(float * address,char *name)//浮点输入
{
	u8 tp_last, key_value, is_key = 0;
	u8 pre_KeyBoard_State = KeyBoard_State;
	float point=0;
	char str[20]={'\0'};
	float temp;
	int i,j;

	str[0]=' ';
	
	i=1;//从1开始，0位正负号
	point=0;
	KeyBoard_State = 1;
	Show_Keyboard();
	LCD_ShowString(0,6,300,24,24,(u8*)name);
	LCD_printf(0,6+36,300,24,24,"old:%lf",*address);
	LCD_printf(0,6+2*36,300,24,24,"new:%s",str);
	
	for(;;)
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
				case key1://1
					if(i<14)
						str[i++]='1';
					break;
				case key2://2
					if(i<14)
						str[i++]='2';
					break;
				case key3://3
					if(i<14)
						str[i++]='3';
					break;
				case key4://4
					if(i<14)
						str[i++]='4';
					break;
				case key5://5
					if(i<14)
						str[i++]='5';
					break;
				case key6://6
					if(i<14)
						str[i++]='6';
					break;
				case key7://7
					if(i<14)
						str[i++]='7';
					break;
				case key8://8
					if(i<14)
						str[i++]='8';
					break;
				case key9://9
					if(i<14)
						str[i++]='9';
					break;
				case key0://0
					if(i<14)
						str[i++]='0';
					break;
				case keypoint://.
					if(point==0&&i<14)
					{
						str[i++]='.';
						point=1;
					}
					break;
				case keysign://-
					if(str[0] == ' ')
						str[0] = '-';
					else
						str[0] = ' ';
					break;
				case keydelete://del
					if(i>1)
					{
						if(str[i-1]=='.')
							point=0;
						str[--i]='\0';
					}
					break;
				case keyOK://ok
					temp=0;
					point=0;
					for(j=1;j<i;j++)// 
					{
						if(str[j]=='.')
						{
							point=10;
						}
						else if(point==0)
						{
							temp=temp*10+str[j]-'0';
						}
						else
						{
							temp+=(str[j]-'0')/(float)point;
							point=point*10;
						}
					}
					if(str[0]=='-')
						temp=-temp;
					*address=temp;
					KeyBoard_State = pre_KeyBoard_State;
					return;
				case keyback://cancel
					KeyBoard_State = pre_KeyBoard_State;
					return;
				case keyboardtab:
					KeyBoard_State = !KeyBoard_State;
					Show_Keyboard();
					LCD_ShowString(0,6,300,24,24,(u8*)name);
					LCD_printf(0,6+36,300,24,24,"old:%d",*address);
					LCD_printf(0,6+2*36,300,24,24,"new:");
					break;
			}
			LCD_Fill(4*12,6+2*36,lcddev.width-1,6+2*36+24,WHITE);
			LCD_printf(4*12,6+2*36,300,24,24,"%s",str);
		}
		else
			delay_ms(1);
	}
}

void Input_DoubleValue(double * address,char *name)//浮点输入
{
	u8 tp_last, key_value, is_key = 0;
	u8 pre_KeyBoard_State = KeyBoard_State;
	double point=0;
	char str[20]={'\0'};
	double temp;
	int i,j;

	str[0]=' ';
	
	i=1;//从1开始，0位正负号
	point=0;
	KeyBoard_State = 1;
	Show_Keyboard();
	LCD_ShowString(0,6,300,24,24,(u8*)name);
	LCD_printf(0,6+36,300,24,24,"old:%lf",*address);
	LCD_printf(0,6+2*36,300,24,24,"new:%s",str);
	
	for(;;)
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
				case key1://1
					if(i<14)
						str[i++]='1';
					break;
				case key2://2
					if(i<14)
						str[i++]='2';
					break;
				case key3://3
					if(i<14)
						str[i++]='3';
					break;
				case key4://4
					if(i<14)
						str[i++]='4';
					break;
				case key5://5
					if(i<14)
						str[i++]='5';
					break;
				case key6://6
					if(i<14)
						str[i++]='6';
					break;
				case key7://7
					if(i<14)
						str[i++]='7';
					break;
				case key8://8
					if(i<14)
						str[i++]='8';
					break;
				case key9://9
					if(i<14)
						str[i++]='9';
					break;
				case key0://0
					if(i<14)
						str[i++]='0';
					break;
				case keypoint://.
					if(point==0&&i<14)
					{
						str[i++]='.';
						point=1;
					}
					break;
				case keysign://-
					if(str[0] == ' ')
						str[0] = '-';
					else
						str[0] = ' ';
					break;
				case keydelete://del
					if(i>1)
					{
						if(str[i-1]=='.')
							point=0;
						str[--i]='\0';
					}
					break;
				case keyOK://ok
					temp=0;
					point=0;
					for(j=1;j<i;j++)// 
					{
						if(str[j]=='.')
						{
							point=10;
						}
						else if(point==0)
						{
							temp=temp*10+str[j]-'0';
						}
						else
						{
							temp+=(str[j]-'0')/(double)point;
							point=point*10;
						}
					}
					if(str[0]=='-')
						temp=-temp;
					*address=temp;
					KeyBoard_State = pre_KeyBoard_State;
					return;
				case keyback://cancel
					KeyBoard_State = pre_KeyBoard_State;
					return;
				case keyboardtab:
					KeyBoard_State = !KeyBoard_State;
					Show_Keyboard();
					LCD_ShowString(0,6,300,24,24,(u8*)name);
					LCD_printf(0,6+36,300,24,24,"old:%d",*address);
					LCD_printf(0,6+2*36,300,24,24,"new:");
					break;
			}
			LCD_Fill(4*12,6+2*36,lcddev.width-1,6+2*36+24,WHITE);
			LCD_printf(4*12,6+2*36,300,24,24,"%s",str);
		}
		else
			delay_ms(1);
	}
}
