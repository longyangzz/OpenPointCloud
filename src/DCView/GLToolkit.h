#ifndef DC_VIEW_GLTOOLKIT_H
#define DC_VIEW_GLTOOLKIT_H

#include "ViewDLL.h"

//Qt
#include "QtCore/QObject"
#include "QtCore/QString"

//Eigen
#include "Eigen/Dense"

//DcCommon
#include "DCCore/BasicTypes.h"

namespace DCView
	{
		//4*4矩阵
		typedef Eigen::Matrix4d Matrix_4_4;
		//向量
		typedef DCCore::Vector_3 Vector_3;

		//OpenGL工具包
		class DC_VIEW_API GLToolkit: public QObject
		{
		public:
			//视图模式
			enum ViewMode
			{
				eTopViewMode				=	0x00000010	,	//顶视图模式
				eBottomViewMode				=	0x00000020	,	//底视图模式
				eFrontViewMode				=	0x00000040	,	//前视图模式
				eBackViewMode				=	0x00000080	,	//后视图模式
				eLeftViewMode				=	0x00000100	,	//左视图模式
				eRightViewMode				=	0x00000200	,	//右视图模式
			};

		public:
			static void RenderTexture2D(unsigned uTextureID, int iWidth, int iHeight);

			static void RenderTexture2DCorner(unsigned uTextureID, int iWidth, int iHeight);

			//初始化glew
			static bool InitGlew();

			//检测扩展是否有效
			static bool IsValidExtension(const char* pExtName);

			//检测shader的有效性
			static bool CheckShadersAvailability();

			//检测FBO是否有效
			static bool CheckFBOAvailability();

			//捕获最近的OpenGL错误
			static QString CatchError();

			//计算旋转矩阵
			static Matrix_4_4 CalculateRotationMatrix(const Vector_3& vSource, 
				const Vector_3& vDest);
			static Matrix_4_4 CalculateRotationMatrix(const Vector_3& vAxis, 
				float fAngle);

			//生成视图矩阵
			static Matrix_4_4 GenerateViewMatrix(ViewMode mode);
		};
	}

#endif
