#include "Impl_ColorTable.hxx"
using namespace DcGui;

//Qt
#include "QtCore/QFile"
#include "QtCore/QXmlStreamReader"
#include "QtCore/QStringList"

//实体
ColorTableImpl ColorTableImpl::s_instance;

//获取颜色
QColor ColorTableImpl::GetColor(const QString& strName)
{
	ColorItem item;

	if (s_instance.FindColor(strName, item))
	{
		return QColor(item.hex);
	}

	return QColor();
}

//获取颜色描述
QString ColorTableImpl::GetColorDescription(const QColor& color)
{
	QString strColorName = color.name();

	ColorItem item;

	if (s_instance.FindColor(strColorName, item))
	{
		return item.desc;
	}
	else
	{
		QString strDec = QString("(%1,%2,%3)")
			.arg(color.red()).arg(color.green()).arg(color.blue());
		return strDec;
	}
}

//查找颜色
bool ColorTableImpl::FindColor(const QString& strName,
	ColorItem& item)
{
	//颜色表文件句柄
	QFile fColorTable(":/Files/Resources/colortable.xml");

	//打开文件，若打开失败返回false
	if (!fColorTable.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		return false;
	}

	//创建XML读取器
	QXmlStreamReader iReader(&fColorTable);

	//读取XML文件
	while (!iReader.atEnd())
	{
		//读取记录
		QXmlStreamReader::TokenType record = iReader.readNext();

		//
		if (record == QXmlStreamReader::StartElement)
		{
			//读取颜色记录
			if ("Color" == iReader.name())
			{
				QString strColorName = iReader.attributes().value("Name").toString();
				QString strHex = iReader.attributes().value("Hex").toString();
				QString strDesc = iReader.attributes().value("Description").toString();

				//如果查询到所要找的颜色
				if (strName.compare(strColorName, Qt::CaseInsensitive) == 0
					|| strName.compare(strHex, Qt::CaseInsensitive) == 0
					|| strName.compare(strDesc, Qt::CaseInsensitive) == 0)
				{
					item.name = strColorName;
					item.desc = iReader.attributes().value("Description").toString();
					item.hex = strHex;

					QString strRGB = iReader.attributes().value("RGB").toString();
					QStringList list = strRGB.split(",", QString::SkipEmptyParts);
					item.red = list[0].toInt();
					item.green = list[1].toInt();
					item.blue = list[2].toInt();

					fColorTable.close();

					return true;
				}
			}
		}
	}

	return false;
}