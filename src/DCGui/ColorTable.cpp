#include "ColorTable.h"
using namespace DcGui;

//Qt
#include "QtCore/QString"

//Impl
#include "Impl/Impl_ColorTable.hxx"

//获取颜色
QColor ColorTable::GetColor(const QString& strName)
{
	return ColorTableImpl::GetColor(strName);
}

//获取颜色描述
QString ColorTable::GetColorDescription(const QColor& color)
{
	return ColorTableImpl::GetColorDescription(color);
}