#ifndef DC_VIEW_VIEWERPARAMS_H
#define DC_VIEW_VIEWERPARAMS_H

//Eigen
#include "Common.h"

namespace DCView
	{
		//视窗的参数
		class ViewerParams
		{
		public:
			//Viewer的参数
			ViewerParams()
				: projectionMode(eProjPerspective)
				, objectCentric(true)
				, scale(1.0f)
				, pixeSize(1.0f)
				, fov(30.0f)
				, aspectWH(1.0f)
				, pivotPos(Point_3(0.0f, 0.0f, 0.0f))
				, cameraPos(Point_3(0.0f, 0.0f, 0.0f))
				, zNear(0)
				, zFar(0)
				, MAX_SCALE(10e6)
				, MIN_SCALE(10e-6)
				, AXIS_LENGTH(25.0f)
				, AXIS_RADIUS(1.0f)
				, PIVOT_RADIUS_PERCENT(0.8f)
				, matrixRotated(Matrix_4_4::Zero(4,4))
			{
				//单位化旋转矩阵
				matrixRotated.setIdentity();
			}


			//投影模式
			ProjectionMode projectionMode;
			//是否以对象为中心
			bool objectCentric;
			//缩放比例
			float scale;
			//像素大小
			float pixeSize;
			//旋转中心
			Point_3 pivotPos;
			//相机位置
			Point_3 cameraPos;
			//视野
			float fov;
			//宽高比例
			float aspectWH;
			//
			double zNear;
			double zFar;

			//旋转矩阵
			Matrix_4_4 matrixRotated;

			//矩阵大小
			static const int MATRIX_SIZE = 16;
			//投影矩阵
			double matrixProjection[MATRIX_SIZE];
			//模型视图矩阵
			double matrixModelView[MATRIX_SIZE];
			//视口矩阵
			int matrixViewport[4];

			//缩放比例的数值范围
			const float MAX_SCALE;
			const float MIN_SCALE;
			//坐标轴长度
			const float AXIS_LENGTH;
			//坐标轴半径
			const float AXIS_RADIUS;
			//旋转中心半径
			const float PIVOT_RADIUS_PERCENT;

		public:
			
		};
	}

#endif