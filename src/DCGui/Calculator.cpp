#include "Calculator.h"
using namespace DcGui;

//Impl
#include "Impl/Impl_Calculator.hxx"

Calculator::Calculator(QWidget* pParent /* = nullptr */)
	: QWidget(pParent, Qt::MSWindowsFixedSizeDialogHint)
{
	m_pImpl = new CalculatorImpl(this);
}

Calculator::~Calculator()
{
	delete m_pImpl;
}