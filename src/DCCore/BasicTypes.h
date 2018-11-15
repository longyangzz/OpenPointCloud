#ifndef DCCORE_BASICTYPES_H
#define DCCORE_BASICTYPES_H

//C++
#include <cmath>
#include <climits>
#include <vector>

//Self
#include "Vector.h"

#include "Matrix2.h"
#include "Matrix3.h"
#include "Matrix3.h"
#include "Matrix4.h"

namespace DCCore
{
	//圆周率PI
	static const double M_pi = 3.1415926;
	static const double M_pi_2 = 6.2831852;

	//零
	static const float Zero = 1.0e-7;

	//二维点
	typedef Vector2DF Point_2;
	//二维向量
	typedef Vector2DF Vector_2;
	//二维点簇
	typedef std::vector<Point_2> PointSet_2;

	//三维点
	typedef Vector3DF Point_3;
	//三维向量
	typedef Vector3DF Vector_3;
	//三维点簇
	typedef std::vector<Point_3> PointSet_3;

	//三维法向量
	typedef Vector_3 Normal_3;
	//三维法向量簇
	typedef std::vector<Normal_3> NormalSet_3;

	//二维三角形
	struct Triangulation_2
	{
		Point_2 VertexA;
		Point_2 VertexB;
		Point_2 VertexC;
	};

	//三维三角形
	struct Triangulation_3
	{
		Point_3 VertexA;
		Point_3 VertexB;
		Point_3 VertexC;
	};

	typedef double real;
}

//基础版兼容考虑---------------------------------------------------------
typedef DCCore::Vector3DF DCVector3D;
typedef Vector3D<double> Point_3D;

//默认二维向量类型
typedef DCCore::Vector2DF DCVector2D;

//坐标类型
typedef float PointCoordinateType;

//无符号字符型
typedef unsigned char uchar;

//标量类型
typedef float ScalarType;
//基础版兼容考虑---------------------------------------------------------

#endif