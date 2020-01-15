#include "string.h"
#include "exfuns.h"
#include "fattester.h"	
#include "malloc.h"
#include "usart.h"
#include "stdio.h"
#include "stdarg.h"
#include "stdlib.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//FATFS 扩展代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/15
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
#define FILE_MAX_TYPE_NUM		7	//最多FILE_MAX_TYPE_NUM个大类
#define FILE_MAX_SUBT_NUM		4	//最多FILE_MAX_SUBT_NUM个小类

 //文件类型列表
u8*const FILE_TYPE_TBL[FILE_MAX_TYPE_NUM][FILE_MAX_SUBT_NUM]=
{
{"BIN"},			//BIN文件
{"LRC"},			//LRC文件
{"NES"},			//NES文件
{"TXT","C","H"},	//文本文件
{"WAV","MP3","APE","FLAC"},//支持的音乐文件
{"BMP","JPG","JPEG","GIF"},//图片文件
{"AVI"},			//视频文件
};
///////////////////////////////公共文件区,使用malloc的时候////////////////////////////////////////////
FATFS *fs[_VOLUMES];//逻辑磁盘工作区.	 
FIL *file;	  		//文件1
FIL *ftemp;	  		//文件2.
UINT br,bw;			//读写变量
FILINFO fileinfo;	//文件信息
DIR dir;  			//目录

u8 *fatbuf;			//SD卡数据缓存区
///////////////////////////////////////////////////////////////////////////////////////
//为exfuns申请内存
//返回值:0,成功
//1,失败
u8 exfuns_init(void)
{
	u8 i;
	for(i=0;i<_VOLUMES;i++)
	{
		fs[i]=(FATFS*)mymalloc(SRAMIN,sizeof(FATFS));	//为磁盘i工作区申请内存	
		if(!fs[i])break;
	}
	file=(FIL*)mymalloc(SRAMIN,sizeof(FIL));		//为file申请内存
	ftemp=(FIL*)mymalloc(SRAMIN,sizeof(FIL));		//为ftemp申请内存
	fatbuf=(u8*)mymalloc(SRAMIN,512);				//为fatbuf申请内存
	if(i==_VOLUMES&&file&&ftemp&&fatbuf)return 0;  //申请有一个失败,即失败.
	else return 1;	
}

//将小写字母转为大写字母,如果是数字,则保持不变.
u8 char_upper(u8 c)
{
	if(c<'A')return c;//数字,保持不变.
	if(c>='a')return c-0x20;//变为大写.
	else return c;//大写,保持不变
}	      
//报告文件的类型
//fname:文件名
//返回值:0XFF,表示无法识别的文件类型编号.
//		 其他,高四位表示所属大类,低四位表示所属小类.
u8 f_typetell(u8 *fname)
{
	u8 tbuf[5];
	u8 *attr='\0';//后缀名
	u8 i=0,j;
	while(i<250)
	{
		i++;
		if(*fname=='\0')break;//偏移到了最后了.
		fname++;
	}
	if(i==250)return 0XFF;//错误的字符串.
 	for(i=0;i<5;i++)//得到后缀名
	{
		fname--;
		if(*fname=='.')
		{
			fname++;
			attr=fname;
			break;
		}
  	}
	strcpy((char *)tbuf,(const char*)attr);//copy
 	for(i=0;i<4;i++)tbuf[i]=char_upper(tbuf[i]);//全部变为大写 
	for(i=0;i<FILE_MAX_TYPE_NUM;i++)	//大类对比
	{
		for(j=0;j<FILE_MAX_SUBT_NUM;j++)//子类对比
		{
			if(*FILE_TYPE_TBL[i][j]==0)break;//此组已经没有可对比的成员了.
			if(strcmp((const char *)FILE_TYPE_TBL[i][j],(const char *)tbuf)==0)//找到了
			{
				return (i<<4)|j;
			}
		}
	}
	return 0XFF;//没找到		 			   
}	 

//得到磁盘剩余容量
//drv:磁盘编号("0:"/"1:")
//total:总容量	 （单位KB）
//free:剩余容量	 （单位KB）
//返回值:0,正常.其他,错误代码
u8 exf_getfree(u8 *drv,u32 *total,u32 *free)
{
	FATFS *fs1;
	u8 res;
    u32 fre_clust=0, fre_sect=0, tot_sect=0;
    //得到磁盘信息及空闲簇数量
    res =(u32)f_getfree((const TCHAR*)drv, (DWORD*)&fre_clust, &fs1);
    if(res==0)
	{											   
	    tot_sect=(fs1->n_fatent-2)*fs1->csize;	//得到总扇区数
	    fre_sect=fre_clust*fs1->csize;			//得到空闲扇区数	   
#if _MAX_SS!=512				  				//扇区大小不是512字节,则转换为512字节
		tot_sect*=fs1->ssize/512;
		fre_sect*=fs1->ssize/512;
#endif	  
		*total=tot_sect>>1;	//单位为KB
		*free=fre_sect>>1;	//单位为KB 
 	}
	return res;
}	

static const char mon_name[12][3] = {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

static void get_date(char date[], u16 *year, u8 *month, u8 *day)
{
	u8 i;
	
	*year = (date[7]-'0')*1000 + (date[8]-'0')*100 + (date[9]-'0')*10 + (date[10]-'0');
	for (i = 0; i < 12; ++i)
		if (strncmp(date, mon_name[i], 3) == 0)
		{
			*month = i + 1;
			break;
		}
	*day = (date[4]-'0')*10 + (date[5]-'0');
}
static void get_time(char time[], u8 *hour, u8 *minute, u8 *second)
{
	*hour = (time[0]-'0')*10 + (time[1]-'0');
	*minute = (time[3]-'0')*10 + (time[4]-'0');
	*second = (time[6]-'0')*10 + (time[7]-'0');
}

void Get_Program_Date(u16 *year, u8 *month, u8 *day)
{
	get_date(__DATE__, year, month, day);
}

void Get_Program_Time(u8 *hour, u8 *minute, u8 *second)
{
	get_time(__TIME__, hour, minute, second);
}

FRESULT f_timestamp(const TCHAR* path)
{
	u16 f_year;
	u8 f_month, f_day, f_hour, f_minute, f_second;
	
	Get_Program_Date(&f_year, &f_month, &f_day);
	Get_Program_Time(&f_hour, &f_minute, &f_second);
	
	return f_set_timestamp(path, f_year, f_month, f_day, f_hour, f_minute, f_second);
}

FRESULT f_set_timestamp(const TCHAR* path, u16 f_year, u8 f_month, u8 f_day, u8 f_hour, u8 f_minute, u8 f_second)    /* Pointer to the file name */
{
	FILINFO fno;
	
	fno.fdate = (WORD)(((f_year - 1980) * 512U) | f_month * 32U | f_day);
	fno.ftime = (WORD)(f_hour * 2048U | f_minute * 32U | f_second / 2U);

	return f_utime(path, &fno);
}

FRESULT f_mknewdir(TCHAR* path)
{
	FRESULT res;
    FILINFO fno;
    u32 i = 0;
    char buf[255], ch[32];
	
    strcpy(buf, path);
	while (i < 0xFFFFFFFF)
	{
		sprintf(ch, " %d", i);
		strcpy(path, buf);
		strcat(path, ch);
		res = f_stat(path, &fno);
		if (res != FR_OK)
			break;
		++i;
	}
	if (i == 0xFFFFFFFF)
	{
		sprintf(ch, " %d", 0);
		strcpy(path, buf);
		strcat(path, ch);
	}
    return f_mkdir((const char *)path);
}

FRESULT f_record_init(TCHAR* path)
{
	FRESULT f_res;
	
	f_res = f_mknewdir(path);
	if (f_res == FR_OK)
	{
		f_res = f_timestamp(path);
		f_res = f_opendir(&dir, path);
		f_res = f_closedir(&dir);
		f_res = f_chdir(path);
	}
	
	return f_res;
}

FRESULT f_record(const TCHAR* path, FIL* fp, const TCHAR* fmt,	...)
{
	FRESULT f_res;
	va_list ap;
	char string[128];	
	
	f_res = f_open(fp, path, FA_OPEN_ALWAYS | FA_READ | FA_WRITE);    //FA_OPEN_ALWAYS就是如果文件不存在，就创建
	if (f_res == FR_OK)
	{
		va_start(ap,fmt);
		vsprintf(string,fmt,ap);
		va_end(ap);
		
		f_res = f_lseek(fp, fp->fsize);   // 接着往下写，不是从头写
		f_printf(fp, string);
		f_res = f_close(fp);                   //只有有了关闭操作，文件才会完整
		f_res = f_timestamp(path);
	}
	
	return f_res;
}








