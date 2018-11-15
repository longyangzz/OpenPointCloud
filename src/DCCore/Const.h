#ifndef DCCORE_CONST_H
#define DCCORE_CONST_H


//C++标准库
#include <cfloat>
#include <limits>
#include <math.h>

namespace DCCore
{
	//圆周率
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

	//! Pi/2
#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923
#endif

	//3的平方根
#ifndef SQRT_3
#define SQRT_3 1.7320508075688772935274463415059
#endif

	//弧度转变为角度的因子
#ifndef DC_RAD_TO_DEG
#define DC_RAD_TO_DEG (180.0/M_PI)
#endif

	//角度转为弧度的因子
#ifndef DC_DEG_TO_RAD
#define DC_DEG_TO_RAD (M_PI/180.0)
#endif

	//约定0的近似值
#ifndef ZERO_TOLERANCE
#define ZERO_TOLERANCE (1.0e-8)
#endif

const float NAN_VALUE = std::numeric_limits<float>::quiet_NaN();

// 点的显示状态
const unsigned char POINT_HIDDEN						=	 0;				
const unsigned char POINT_VISIBLE						=	 1;				
const unsigned char POINT_OUT_OF_RANGE					=	 2;				
const unsigned char POINT_OUT_OF_FOV					=	 4;				

//! Chamfer distances types
enum DC_CHAMFER_DISTANCE_TYPE 
{	CHAMFER_111			=	0,		
	CHAMFER_345			=	1			
};

	
enum DC_LOCAL_MODEL_TYPES 
{	NO_MODEL				=	0,			
	LS					=	1,				
	TRI					=	2,				
	HF					=	3				
};

	
const unsigned DC_LOCAL_MODEL_MIN_SIZE[] = 
{	1,				
	3,				
	3,				
	6,				
};

const float MAX_POINTCOORD = 1.0e10;
const float MIN_POINTCOORD = -1.0e10;//1.0e-10;

const unsigned MAX_POINTS_OF_PERCLOUD = 20000000;
const unsigned long MAX_LOD_POINTS_NUMBER = 1.0e7;

#ifdef _DEBUG
	const unsigned MAX_FACE_NUM = 20000;
	const unsigned MAX_ROTATE_POINT_NUM = 1000;
#else
	const unsigned MAX_FACE_NUM = 2000000;
	const unsigned MAX_ROTATE_POINT_NUM = 100000;
#endif

}

#endif
