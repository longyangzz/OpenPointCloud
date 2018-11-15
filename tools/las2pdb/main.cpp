#include "DlgFileSplit.h"
#include <QtWidgets/QApplication>

#include "QTextCodec"

int main(int argc, char *argv[])
{


	QApplication a(argc, argv);
	DlgFileSplit dlg;
	if (!dlg.exec())
	{
		return 0;
	}
}
