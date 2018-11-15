#ifndef DC_GUI_CALCULATOR_H
#define DC_GUI_CALCULATOR_H

#include "DCGuiDLL.h"

//Qt
#include "QtWidgets/QWidget"

namespace DcGui
{
	class DCGUI_EXPORT Calculator : public QWidget
	{
		Q_OBJECT

	public:
		explicit Calculator(QWidget* pParent = nullptr);
		virtual ~Calculator();

	private:
		class CalculatorImpl;
		CalculatorImpl* m_pImpl;

	};
}

#endif