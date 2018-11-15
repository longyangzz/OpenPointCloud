#include "UnitManager.h"
#include <QSettings>
#include <QMessageBox>


UnitManager::UnitManager(QObject *parent)
	: QObject(parent)
	, m_PreviousUnit("")
{
	//将转换系数初始换为1
}

UnitManager::~UnitManager()
{

}
void UnitManager::SetUnit(QString unit)
{
	if (m_PreviousUnit == "")
	{
		InitUnit(unit);
	}

	m_pUnit = unit;

	UnitConversion();
}

void UnitManager::InitUnit(QString unit)
{
	m_PreviousUnit = unit;
	QSettings settings("DCLW", "MP");
	settings.beginGroup("MYUnit");
	settings.setValue("PreUnit",m_PreviousUnit);
	settings.endGroup();
	m_pScale = 1;
}

QString UnitManager::GetUnit()
{
	return m_pUnit;
}
//单位转换
void UnitManager::UnitConversion()
{
	QSettings settings("DCLW", "MP");
	settings.beginGroup("MYUnit");
	QString pre = settings.value("PreUnit").toString();
	settings.endGroup();
	QString cur = m_pUnit;
	if (cur != pre)
	{
		m_pScale = 1;
		/*********将原单位转化成米*************/
		if (pre == "毫米")
		{
			m_pScale = m_pScale * 0.001;
			
		}
		else if (pre == "厘米")
		{
			m_pScale = m_pScale * 0.01;
			
		}
		else if (pre == "英尺")
		{
			m_pScale = m_pScale * 0.30479999;
			
		}
		else if (pre == "尺")
		{
			m_pScale = m_pScale *0.33333333;
			
		}
		else if (pre == "米")
		{
			m_pScale = m_pScale * 1;
			m_EnUnit = "m";
		}
		/******************将米转换成现在的单位***********************/
		if (cur == "厘米")
		{
			m_pScale = m_pScale * 100;
			m_EnUnit = "cm";

		}
		else if (cur == "毫米")
		{
			m_pScale = m_pScale * 1000;
			
			m_EnUnit = "mm";
		}
		else if (cur == "英尺")
		{
			m_pScale = m_pScale * 3.2808399;
			m_EnUnit = "ft";
		}
		else if (cur == "尺")
		{
			m_pScale = m_pScale * 3;
			m_EnUnit = "chi";
		}
	}
	else
	{
		m_pScale = 1;
	}

}

double UnitManager::GetScale()
{
	return m_pScale;
}
void UnitManager::SetScale(double scale)
{
	m_pScale = scale;
}
QString UnitManager::GetENUnit()
{
	return m_EnUnit;
}