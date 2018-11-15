#ifndef DC_VIEWER_COMMON_H
#define DC_VIEWER_COMMON_H

//DcCommon
#include "DCCore/BasicTypes.h"
#include "Eigen/Dense"

namespace DCView
	{

		//点
		typedef DCCore::Point_2 Point_2;
		typedef DCCore::Point_3 Point_3;
		typedef DCCore::Vector_3 Vector_3;

		//4*4矩阵
		typedef Eigen::Matrix4d Matrix_4_4;

		//投影模式
		enum ProjectionMode
		{
			eProjOrtho			=	0x00001000	,	//正投影
			eProjPerspective	=	0x00002000		//透视投影
		};

	}

#endif