#ifndef DC_COMMON_ALGORITHM_H
#define DC_COMMON_ALGORITHM_H

#include "DCCoreDLL.h"

//Self
#include "BasicTypes.h"

namespace DCCore
{
	//弧度转角度
	DCCORE_EXPORT float RadianToAngle(float radian);

	//角度转弧度
	DCCORE_EXPORT float AngleToRadian(float angle);

	//两点间的距离（二维）
	DCCORE_EXPORT float Distance(const Point_2& pt1, const Point_2& pt2);

	//两点间的距离（三维）
	DCCORE_EXPORT float Distance(const Point_3& pt1, const Point_3& pt2);

	//向量间的夹角(二维、弧度)
	DCCORE_EXPORT float VectorAngle(const Vector_2& v1, const Vector_2& v2);

	//向量间的夹角(三维、弧度)
	DCCORE_EXPORT float VectorAngle(const Vector_3& v1, const Vector_3& v2);

	//判断点在三角形内部（二维）
	DCCORE_EXPORT bool InsideTriangle(const Point_2& pt, const Triangulation_2& triangle);

	//判断点在三角形内部（三维）
	//DCCORE_EXPORT bool IsInTriangulation(const Point_3& pt, const Triangulation_3& triangle);

	//三角形的法向量（二维）
	DCCORE_EXPORT Vector_2 NormalOfTriangle(const Triangulation_2& triangle);

	//三角形的法向量（三维）
	DCCORE_EXPORT Vector_3 NormalOfTriangle(const Triangulation_3& triangle);
}

#endif