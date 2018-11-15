#ifndef _DC_COORDINATES_SHIFT_MANAGER_H_
#define _DC_COORDINATES_SHIFT_MANAGER_H_

//C++标准库
#include <array>

//DCCore
#include "BasicTypes.h"

//可接受的最大坐标值
#define MAX_COORDINATE_ABS_VALUE ((PointCoordinateType)1.0e6)

//加载数据时，处理坐标平移
namespace DCCore
{
	class CoordinatesShiftManager
	{
	public:

		//! Handles coordinates shift/scale given the first 3D point and current related parameters
		static bool Handle(const Point_3D& P, Point_3D& shifCoordinates)
		{
			//is shift necessary?
			if (   fabs(P[0]) >= MAX_COORDINATE_ABS_VALUE
				|| fabs(P[1]) >= MAX_COORDINATE_ABS_VALUE
				|| fabs(P[2]) >= MAX_COORDINATE_ABS_VALUE)
			{
				shifCoordinates[0] = -P[0];
				shifCoordinates[1] = -P[1];
				shifCoordinates[2] = -P[2];

				return true;
			}
			return false;
		}
	};
}


#endif