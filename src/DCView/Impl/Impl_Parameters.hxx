#ifndef DC_VIEW_IMPL_PARAMETERS_H
#define DC_VIEW_IMPL_PARAMETERS_H

#include "DCView/Parameters.h"

//Qt
#include "QtCore/QMap"

#include "QtGui/QColor"
#include "QtGui/QLinearGradient"
#include "QtGui/QBrush"

namespace DCView
	{
		//操作类型
		typedef GlobalParameters::OperationFlag OperationFlag;
		//组合键
		typedef GlobalParameters::CompositeKey CompositeKey;
		//背景规则
		typedef GlobalParameters::BackgroundRole BackgroundRole;

		//全局参数实现类
		class GlobalParameters::GlobalParametersImpl
		{
		public:
			GlobalParametersImpl(GlobalParameters* pInterface);

			//设置组合键
			void SetCompositeKey(OperationFlag flag, 
				CompositeKey keys);

			//获取组合键
			CompositeKey GetCompositeKey(OperationFlag flag) const;

			//设置背景色
			void SetBackground(const QColor& rBckFill);
			void SetBackground(const QLinearGradient& rBckGradient);

			//获取背景规则
			BackgroundRole GetBackgroundRole() const;
			//获取背景
			QBrush GetBackground() const;

		private:
			//接口
			GlobalParameters* m_pInterface;
			//组合键注册表
			QMap<OperationFlag, CompositeKey> m_compositeKeys;

			//背景
			struct
			{
				//背景规则
				BackgroundRole role;
				//颜色
				QColor color;
				//渐变
				QLinearGradient gradient;
			}m_background;
		};
	}

#endif