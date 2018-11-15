//Qt
#include "QtWidgets/QApplication"
#include "DlgSamplingRate.h"

static DlgSamplingRate* s_dlgSamplingRate;

DlgSamplingRate::DlgSamplingRate(QWidget *parent)
	: QDialog(parent)
	, m_rate(1)
{
	ui.setupUi(this);

	//设置默认采样率为1/2
	ui.cmbxSamplingRate->setCurrentIndex(1);
	m_rate = 2;

	CreateConnentions();
}

DlgSamplingRate::~DlgSamplingRate()
{

}

//创建信号与槽的连接
void DlgSamplingRate::CreateConnentions()
{
	//
	connect(ui.cmbxSamplingRate, SIGNAL(currentIndexChanged(int)), this, SLOT(SetRate(int)));

	//点击确定
	connect(ui.btnxAffirm, SIGNAL(accepted()), this, SLOT(accept()));

	//点击取消关闭对话框
	connect(ui.btnxAffirm, SIGNAL(rejected()), this, SLOT(reject()));
}

//设置采样率
void DlgSamplingRate::SetRate(int rate)
{
	m_rate = rate + 1;
}

//获取采样率
int DlgSamplingRate::Rate()
{
	s_dlgSamplingRate = new DlgSamplingRate(QApplication::activeWindow());

	if (s_dlgSamplingRate->exec())
	{
		return s_dlgSamplingRate->GetRate();
	}
	else
	{
		return -1;
	}
}