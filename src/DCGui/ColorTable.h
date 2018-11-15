#ifndef DC_GUI_COLORTABLE_H
#define DC_GUI_COLORTABLE_H

#include "DCGuiDLL.h"

//Qt
#include "QtGui/QColor"
class QString;

namespace DcGui
{
	class DCGUI_EXPORT ColorTable
	{
	public:
		static QColor GetColor(const QString& strName);
		//ªÒ»°—’…´√Ë ˆ
		static QString GetColorDescription(const QColor& color);
	};
}

#endif