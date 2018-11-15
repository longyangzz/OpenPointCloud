#ifndef DC_GUI_IMPL_COLORTABLE_H
#define DC_GUI_IMPL_COLORTABLE_H

//Qt
#include "QtGui/QColor"
#include "QtCore/QString"

namespace DcGui
{
	class ColorTableImpl
	{
	public:
		static QColor GetColor(const QString& strName);
		//获取颜色描述
		static QString GetColorDescription(const QColor& color);

	private:
		struct ColorItem
		{
			QString name;
			QString desc;
			QString hex;
			int red;
			int green;
			int blue;
		};
		//查找颜色
		bool FindColor(const QString& strName, ColorItem& item);

	private:
		static ColorTableImpl s_instance;
	};
}

#endif