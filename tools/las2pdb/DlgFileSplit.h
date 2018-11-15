#ifndef DLGFILESPLIT_H
#define DLGFILESPLIT_H

#include <QDialog>
#include "ui_DlgFileSplit.h"

class DlgFileSplit : public QDialog
{
	Q_OBJECT

public:
	DlgFileSplit(QWidget *parent = 0);
	~DlgFileSplit();

private:
	void CreateConnection();

private slots:
	void OpenInputFile();

	void OpenOutDir();

	void Ok();
private:
	Ui::DlgFileSplit ui;
};

#endif // DLGFILESPLIT_H
