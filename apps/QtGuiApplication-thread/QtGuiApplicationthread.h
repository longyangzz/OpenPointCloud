#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QtGuiApplicationthread.h"

class QtGuiApplicationthread : public QMainWindow
{
	Q_OBJECT

public:
	QtGuiApplicationthread(QWidget *parent = Q_NULLPTR);

private:
	Ui::QtGuiApplicationthreadClass ui;
};
