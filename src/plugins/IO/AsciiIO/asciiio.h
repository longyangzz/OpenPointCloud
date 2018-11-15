#ifndef ASCIIIO_H
#define ASCIIIO_H

/*#include <vld.h>*/

//Qt
#include "QString"
#include "QStringList"
#include "QThread"
#include "QChar"

//IO
#include "DCCore/IoPlugin.h"

#include "DlgOpenAsciiFile.h"

class DataColumnType;

//定义一个每行数据的结构;
struct PerLineData
{
	//X、Y、Z坐标;
	double xCoord;
	double yCoord;
	double zCoord;

	//RGB值;
	int rgbRed;
	int rgbGreen;
	int rgbBlue;

	//灰度值;
	int gray;

	//未定义标量;
	QStringList undefineScalar;

	//忽略的值;
	QStringList ignoreValue;
};


namespace DcIo
{
class DCCORE_EXPORT AsciiIO : public DCCore::IoPlugin
{
	Q_OBJECT
public:
	AsciiIO();

	static IoPlugin* Instance(const char* extension);

	static QStringList SUPPORT_EXTENSION;

	QString GetName();

	virtual DcGp::DcGpEntity* LoadFile(const QString& filename,
		                                DCCore::CallBackPos* cb = 0, 
										bool alwaysDisplayLoadDialog = true,
										bool* coordinatesShiftEnabled = 0,
										double* coordinatesShift = 0,
										QString kind = ""
										);

	DcGp::DcGpEntity* LoadCloudFromFormatedAsciiFile(const QString& fileName,
													const DataColumnType type,
													QChar separator,
													unsigned approximateNumberOfLines,
													qint64 fileSize,
													unsigned maxCloudSize,
													 DCCore::CallBackPos* cb = 0, 
													int sampleRate = 1,
													unsigned skipLines=0,
													bool alwaysDisplayLoadDialog=true,
													bool* coordinatesShiftEnabled=0,
													double* coordinatesShift=0);

	virtual bool SaveToFile(DcGp::DcGpEntity* entity, QString filename);

	virtual void UnloadInstance();
protected:
	~AsciiIO();

signals:
	void Notify(unsigned);
private:
	static DCCore::IoPlugin* s_instance;
};

}

DCCORE_EXPORT DCCore::IoPlugin* LoadPlugin(const char* extension);
DCCORE_EXPORT void UnloadPlugin();
#endif // ASCIIIO_H
