#ifndef DC_VIEW_PARAMETERS_H
#define DC_VIEW_PARAMETERS_H

#include "ViewDLL.h"

//Qt
#include "QtCore/QString"
class QColor;
class QLinearGradient;
class QBrush;

namespace DCView
	{
		//
		class DC_VIEW_API GlobalParameters
		{
		public:
			//操作类型
			enum OperationFlag
			{
				eTranslateOperation				=	0x00000001	,	//平移
				eRotateOperation				=	0x00000002	,	//旋转
				eScaleOperation					=	0x00000004  ,	//缩放
				eZoomOnRegion					=	0x00000008  ,		//区域缩放
				ePicking						=	0x00000012
			};
			//组合键
			struct CompositeKey
			{
				Qt::MouseButtons buttons;
				Qt::KeyboardModifiers keys;

				CompositeKey()
					: buttons(Qt::NoButton)
					, keys(Qt::NoModifier)
				{}
			};

			//背景规则
			enum BackgroundRole
			{
				eBckFill			=	0x00001000	,	//填充背景
				eBckGradient		=	0x00002000		//渐变背景
			};

		public:
			GlobalParameters();
			~GlobalParameters();

			//设置背景色
			static void SetBackground(const QColor& rBckFill);
			static void SetBackground(const QLinearGradient& rBckGradient);

			//设置环境光
			static void SetAmbientLight(const QColor& ambient);

			//获取背景规则
			static BackgroundRole GetBackgroundRole();
			//获取背景
			static QBrush GetBackground();

			//设置操作的组合键
			//默认组合：（旋转-鼠标左键；缩放-转动滚轴；平移-鼠标中键）
			static void SetCompositeKey(OperationFlag flag, 
				Qt::MouseButtons buttons, 
				Qt::KeyboardModifiers keys);

			//获取组合键
			static CompositeKey GetCompositeKey(OperationFlag flag);

			//Shaders路径
			static QString ShadersPath();

			static float* DiffuseLight();
			static float* AmbientLight();
			static float* SpecularLight();
			static unsigned char* BoundBoxColor();
		private:
			static GlobalParameters s_globalParams;

			class GlobalParametersImpl;
			GlobalParametersImpl* m_pImpl;
		};
	}



 #endif
