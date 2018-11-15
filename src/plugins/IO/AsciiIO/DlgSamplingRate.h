#ifndef DLGSAMPLINGRATE_H
#define DLGSAMPLINGRATE_H

#include <QtWidgets/QDialog>
#include "ui_DlgSamplingRate.h"

class DlgSamplingRate : public QDialog
{
	Q_OBJECT

public:
	DlgSamplingRate(QWidget *parent = 0);
	~DlgSamplingRate();

	static int Rate();

	int GetRate() const
	{
		return m_rate;
	}

private:
	Ui::DlgSamplingRate ui;

	//采样率
	int m_rate;

	//创建信号与槽的连接
	void CreateConnentions();

private slots:
	//更新采样率
	void SetRate(int rate);
};

#endif // DLGSAMPLINGRATE_H
