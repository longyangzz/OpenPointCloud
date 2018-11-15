#ifndef DC_GUI_IMPL_CALCULATOR_H
#define DC_GUI_IMPL_CALCULATOR_H

//Qt
#include "QtCore/QObject"

#include "../Calculator.h"

namespace Ui
{
	class Calculator;
}

namespace DcGui
{
	//计算器实现类
	class Calculator::CalculatorImpl : public QObject
	{
		Q_OBJECT
	public:
		explicit CalculatorImpl(Calculator* pInterface);
		~CalculatorImpl();

	private:
		//初始化UI
		void InitUI();

		//创建信号与槽的连接
		void CreateConnections();

	private slots:
		//切换模式
		void SwitchMode(bool bState);

	private:
		//接口
		Calculator* m_pInterface;

		//ui
		Ui::Calculator* m_pUi;
	};
}

#endif