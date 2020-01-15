/* Includes ------------------------------------------------------------------*/
#include "mymath.h"

/*
 * 函数名: MaxFour
 * 描  述: 求四个数中的最大值
 * 输  入: -a -b -c -d 四个数据
 * 输  出: 四个数中的最大值
 * 调  用: 外部调用
 */
double MaxFour(double a,double b,double c,double d)
{
	double maxtemp1,maxtemp2;
	maxtemp1=Max(a,b);
	maxtemp2=Max(c,d);
	return Max(maxtemp1,maxtemp2);
}

/*
 * 函数名: GetLength
 * 描  述: 求两个点之间的距离
 * 输  入: -a -b 两个点
 * 输  出: 两个点之间的距离
 * 调  用: 外部调用
 */
double GetLength(struct Point a, struct Point b)
{
	double length;
	double dx,dy;
	dx = (a.x-b.x)*(a.x-b.x);
	dy = (a.y-b.y)*(a.y-b.y);
	length = sqrt(dx+dy);
	return length;
}

/*
 * 函数名: GetLineAngle
 * 描  述: 求两个点之间连线的倾角(弧度)
 * 输  入: -sta 起始点
 *		   -end 结束点
 * 输  出: 两个点连线的倾角(弧度-PI~PI)
 * 调  用: 外部调用
 */
double GetLineAngle(struct Point sta,struct Point end)
{
	double radian;
	
	radian = atan2(end.y-sta.y, end.x-sta.x);
	
	return radian;
}

/****************************各种点到直线方程求距离******************************/

/*
 * 函数名: GetDis_P2L
 * 描  述: 求点到直线的距离(已知一般式: ax+by+c=0)
 * 输  入: -p 点
 *		   -a -b -c 直线方程的系数
 * 输  出: 点到直线的距离
 * 调  用: 外部调用
 */
double GetDis_P2L(struct Point p,double a,double b,double c)
{
	return Abs(a*p.x+b*p.y+c)/(sqrt(a*a+b*b));
}

/*
 * 函数名: GetDis_P2L_PP
 * 描  述: 求点到直线的距离(已知两点式: (y-y1)(x2-x1)=(y2-y1)(x-x1))
 * 输  入: -p  点
 *		   -p1 -p2 直线上两点
 * 输  出: 点到直线的距离
 * 调  用: 外部调用
 */
double GetDis_P2L_PP(struct Point p,struct Point p1,struct Point p2)
{
	double a,b,c;
	a=p2.y-p1.y;
	b=p1.x-p2.x;
	c=p2.x*p1.y-p1.x*p2.y;
	
	return GetDis_P2L(p,a,b,c);
}

/*
 * 函数名: GetDis_P2L_PK
 * 描  述: 求点到直线的距离(已知点斜式: y-y0=k(x-x0))
 * 输  入: -p  点
 *		   -p0 直线上一点
		   -k  直线方程的斜率
 * 输  出: 点到直线的距离
 * 调  用: 外部调用
 */
double GetDis_P2L_PK(struct Point p,struct Point p0,double k)
{
	double a,b,c;
	a=k;
	b=-1;
	c=-k*p0.x+p0.y;
	
	return GetDis_P2L(p,a,b,c);
}

/****************************各种点到直线方程求垂足******************************/

/*
 * 函数名: GetFoot_P2L
 * 描  述: 求点到直线的垂足(已知一般式: ax+by+c=0)
 * 输  入: -p 点
 *		   -a -b -c 直线方程的系数
 * 输  出: 垂足
 * 调  用: 外部调用
 */
struct Point GetFoot_P2L(struct Point p,double a,double b,double c)
{
	struct Point foot;
	foot.x=(b*b*p.x-a*b*p.y-a*c)/(a*a+b*b);
	foot.y=(a*a*p.y-a*b*p.x-b*c)/(a*a+b*b);
	
	return foot;
}

/*
 * 函数名: GetFoot_P2L_PP
 * 描  述: 求点到直线的垂足(已知两点式: (y-y1)(x2-x1)=(y2-y1)(x-x1))
 * 输  入: -p  点
 *		   -p1 -p2 直线上两点
 * 输  出: 垂足
 * 调  用: 外部调用
 */
struct Point GetFoot_P2L_PP(struct Point p,struct Point p1,struct Point p2)
{
	struct Point foot;
	double a,b,c;
	a=p2.y-p1.y;
	b=p1.x-p2.x;
	c=p2.x*p1.y-p1.x*p2.y;
	
	foot = GetFoot_P2L(p,a,b,c);
	return foot;
}

/*
 * 函数名: GetFoot_P2L_PK
 * 描  述: 求点到直线的垂足(已知点斜式: y-y0=k(x-x0))
 * 输  入: -p  点
 *		   -p0 直线上一点
		   -k  直线方程的斜率
 * 输  出: 垂足
 * 调  用: 外部调用
 */
struct Point GetFoot_P2L_PK(struct Point p,struct Point p0,double k)
{
	struct Point foot;
	double a,b,c;
	a=k;
	b=-1;
	c=-k*p0.x+p0.y;
	
	foot = GetFoot_P2L(p,a,b,c);
	return foot;
}

/**********************************END OF FILE*********************************/

