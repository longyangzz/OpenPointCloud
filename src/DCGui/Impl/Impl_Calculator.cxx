#include "Impl_Calculator.hxx"
using namespace DcGui;

//Self
#include "ui_Calculator.h"

Calculator::CalculatorImpl
	::CalculatorImpl(Calculator* pInterface)
	: m_pInterface(pInterface)
	, m_pUi(nullptr)
{
	//初始化UI
	InitUI();

	//创建信号与槽的连接
	CreateConnections();
}

Calculator::CalculatorImpl
	::~CalculatorImpl()
{
	delete m_pUi;
}

//初始化UI
void Calculator::CalculatorImpl
	::InitUI()
{
	//加载UI文件
	m_pUi = new Ui::Calculator;
	m_pUi->setupUi(m_pInterface);

	//设置默认为标准模式
	m_pUi->rbtnStandard->setChecked(true);
}

//创建信号与槽的连接
void Calculator::CalculatorImpl
	::CreateConnections()
{
	//切换计算器类型
	connect(m_pUi->rbtnStandard, SIGNAL(toggled(bool)),
		this, SLOT(SwitchMode(bool)));
	connect(m_pUi->rbtnScientific, SIGNAL(toggled(bool)),
		this, SLOT(SwitchMode(bool)));
}

//切换模式
void Calculator::CalculatorImpl
	::SwitchMode(bool bState)
{
	//信号发送者
	QObject* pSender = sender();

	//标准模式
	if (pSender == m_pUi->rbtnStandard)
	{
		if (bState)
		{
			m_pUi->wgtScientific->setVisible(false);
			m_pInterface->setFixedWidth(m_pUi->wgtStandard->width() + 20);
		}
	}
	//科学模式
	else if (pSender == m_pUi->rbtnScientific)
	{
		if (bState)
		{
			m_pUi->wgtScientific->setVisible(true);
			m_pInterface->setFixedWidth(m_pUi->wgtStandard->width()
				+ m_pUi->wgtScientific->width() + 20);
		}
	}
}