#ifndef UNITMANAGER_H
#define UNITMANAGER_H

#include <QObject>

struct UnitInfo
{
	QString PreUnit;
};

class UnitManager : public QObject
{
	Q_OBJECT

public:
	UnitManager(QObject *parent);
	~UnitManager();

	QString GetUnit();
	void SetUnit(QString unit);

	QString GetENUnit();

	void SetScale(double scale);
	double GetScale();
private:
	//初始化
	void InitUnit(QString unit);
	//单位转换
	void UnitConversion();
	//记录先前的单位
	void RecordPreUnit();

private:
	QString m_PreviousUnit;
	QString m_pUnit;
	double m_pScale;
	QString m_EnUnit;
	UnitInfo uninfo;
};



#endif // UNITMANAGER_H
