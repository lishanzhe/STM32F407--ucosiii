/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MYMATH_H
#define __MYMATH_H

/* Includes ------------------------------------------------------------------*/
#include "sys.h"
#include "MyTypeDef.h"
#include <math.h>
/* Exported macro ------------------------------------------------------------*/
#define Max(a,b) (a>b?a:b)
#define Min(a,b) (a<b?a:b)

#define Abs(a)   ((a)>0?(a):-(a))

#define pi 3.14159265358979
#define DEG2RAD		0.017453293		/* 度转弧度 π/180 */
#define RAD2DEG		57.29578		/* 弧度转度 180/π */
/* Exported variables --------------------------------------------------------*/
/* Exported function prototype -----------------------------------------------*/

double MaxFour(double a, double b, double c, double d);
double GetLength(struct Point a, struct Point b);
double GetLineAngle(struct Point sta, struct Point end);

double GetDis_P2L(struct Point p,double a,double b,double c);
double GetDis_P2L_PK(struct Point p,struct Point p0,double k);
double GetDis_P2L_PP(struct Point p,struct Point p1,struct Point p2);

struct Point GetFoot_P2L(struct Point p,double a,double b,double c);
struct Point GetFoot_P2L_PK(struct Point p,struct Point p0,double k);
struct Point GetFoot_P2L_PP(struct Point p,struct Point p1,struct Point p2);

#endif 

/**********************************END OF FILE*********************************/
