#include "asciiio.h"

//C++
#include <array>

//Qt标准库
#include "QFile"
#include "QTime"
#include "QFileInfo"
#include "QTextStream"
#include "QtWidgets/QApplication"
#include "QDebug"
#include "QtWidgets/QApplication"

//DCCore
#include "DCCore/Logger.h"
#include "DCCore/CoordinatesShiftManager.h"

//DcGp
#include "DCGp/Custom/GpScalarField.h"
#include "DCGp/Custom/GpPointCloud.h"

#include "DlgSamplingRate.h"

using namespace DcIo;





DCCore::IoPlugin* AsciiIO::s_instance = 0;
QStringList AsciiIO::SUPPORT_EXTENSION = QStringList() << "txt" << "pts" << "asc" << "xyz";

DCCore::IoPlugin* LoadPlugin(const char* extension)
{
	return AsciiIO::Instance(extension);
}

void UnloadPlugin()
{
	//AsciiIO::UnloadInstance();
}

DCCore::IoPlugin* AsciiIO::Instance(const char* extension)
{
	bool status = false;
	for (auto i = 0; i < SUPPORT_EXTENSION.size(); ++i)
	{
		if (QString(extension).toUpper() == SUPPORT_EXTENSION.at(i).toUpper())
		{
			status = true;
		}
	}

	if (status)
	{
		if (!s_instance)
		{
			s_instance = new AsciiIO();
		}
		return s_instance;
	}
	else
	{
		return nullptr;
	}
}

void AsciiIO::UnloadInstance()
{
	if (s_instance)
	{
		delete s_instance;
		s_instance = 0;
	}
}

AsciiIO::AsciiIO()
	: DCCore::IoPlugin()
{
	
}

AsciiIO::~AsciiIO()
{
	
}

QString AsciiIO::GetName()
{
	return QString("ASCIIIO");
}

//加载文件
DcGp::DcGpEntity* AsciiIO::LoadFile(const QString& filename, 
	                                            DCCore::CallBackPos* cb, 
												bool alwaysDisplayLoadDialog /* = true */, 
												bool* coordinatesShiftEnabled /* = 0 */, 
												double* coordinatesShift /* = 0 */ ,
												QString kind /*= ""*/)
{
	QFile file(filename);

	//文件不存在
	if (!file.exists())
	{
		DCCore::Logger::Error(QObject::tr("File [%1] doesn\'t exist.").arg(filename));
		return nullptr;
	}

	unsigned long fileSize = file.size();
	//文件为空
	if (fileSize == 0)
	{
		DCCore::Logger::Message(QObject::tr("File [%1] is empty.").arg(filename));
	}


	//! 参数初始化
	DataColumnType columnTypes;
	QChar separator;
	unsigned numberOfLines;
	unsigned maxCloudSize;
	int sampleRate;
	unsigned skippedLines;

	//! 由于在线程中不支持qeidget类，所以在线程中使用这个io库的话，需要屏蔽掉qwidget部分
	if (alwaysDisplayLoadDialog)
	{
		//column attribution dialog
		DlgOpenAsciiFile dlg(filename, QApplication::activeWindow());

		//如果数据列数少于3则返回错误的文件类型
		if (dlg.GetDataColumnNumber() < 3)
		{
			DCCore::Logger::Error(QObject::tr("File [%1] is illegal.").arg(filename));
		}

		if (alwaysDisplayLoadDialog) // && dlg.GetDataColumnNumber() >  3|| !AsciiOpenDlg::CheckOpenSequence(dlg.getOpenSequence(),dummyStr))
		{
			if (!dlg.exec())
			{
				DCCore::Logger::Message(QObject::tr("Canceled By User"));
				return nullptr;
			}
		}


		columnTypes = dlg.GetDataColumnType();
		separator = dlg.GetSeparator();
		numberOfLines = dlg.GetDataLines();
		maxCloudSize = dlg.GetMaxCloudSize();
		sampleRate = dlg.GetSampleRate();
		skippedLines = dlg.GetSkippedLines();

		connect(this, SIGNAL(Notify(unsigned)), QApplication::activeWindow(), SLOT(OnNotifyProgressBar(unsigned)));
	}
	else   //! 默认的在线程中加载的时候，只支持默认参数情况
	{
		columnTypes.xCoordIndex = 0;
		columnTypes.yCoordIndex = 1;
		columnTypes.zCoordIndex = 2;

		separator = QChar(',');
		numberOfLines = 10;
		maxCloudSize = 0;
		sampleRate = 1;
		skippedLines = 0;
	}
	
	return LoadCloudFromFormatedAsciiFile(  filename,
											columnTypes,
											separator,
											numberOfLines,
											fileSize,
											maxCloudSize,
											cb,
											sampleRate,
											skippedLines,
											alwaysDisplayLoadDialog,
											coordinatesShiftEnabled,
											coordinatesShift);
}

//从文件中加载数据
DcGp::DcGpEntity* AsciiIO::LoadCloudFromFormatedAsciiFile(const QString& fileName,
																		const DataColumnType columnType,
																		QChar separator,
																		unsigned numberOfLines,
																		qint64 fileSize,
																		unsigned maxCloudSize,
																		DCCore::CallBackPos* cb, 
																		int sampleRate,
																		unsigned skippedLines,
																		bool alwaysDisplayLoadDialog,
																		bool* coordinatesShiftEnabled,
																		double* coordinatesShift)
{
	QFile file(fileName);

	//文件不存在
	if (!file.exists())
	{
		DCCore::Logger::Error(QObject::tr("File [%1] doesn\'t exist.").arg(fileName));
		return false;
	}

	fileSize = file.size();
	//文件为空
	if (fileSize == 0)
	{
		DCCore::Logger::Message(QObject::tr("File [%1] is empty.").arg(fileName));
	}

	//打开文件;
	file.open(QIODevice::ReadOnly | QIODevice::Text);
	//通过计数，略过忽略的行数;
	long count = 0;
	char currentLine[500];
	//读入点云数据的最大数目
	long maxSize = (numberOfLines - skippedLines);

	//获取采样率
	int samplingRate = 1;
	if (alwaysDisplayLoadDialog)
	{
		samplingRate = sampleRate; //DlgSamplingRate::Rate();
		if (samplingRate < 0)
		{
			return false;
		}
	}

	//创建一个点云对象,并赋予其最大数目
	DcGp::DcGpPointCloud* pointCloud = new DcGp::DcGpPointCloud(QFileInfo(fileName).baseName());
	//判断是否需要提前分配颜色等参数的大小
	if (columnType.HasRGBColors())
	{
		pointCloud->SetColorSize(maxSize);
	}

	//判断是否需要提前分配法向量参数的大小
	if (columnType.HasNormals())
	{
		pointCloud->SetNormalSize(maxSize);
	}

	PerLineData record;
	Point_3D point(0,0,0);
	Point_3D shift(0,0,0);
	//点的默认颜色
	PointColor color = {255, 255, 255};

	//点的法向量
	DCVector3D normal(0,0,0);

	//标量场
	for (unsigned i = 0; i < columnType.scalarIndex.size(); ++i)
	{
		DcGp::DcGpScalarFieldPtr scalarField(new DcGp::DcGpScalarField(QObject::tr("Scalar Field #%1").arg(i)));
		pointCloud->AddScalarField(scalarField);
	}
	//判断是否为标量场分配空间
	/*if (columnType.scalarIndex.size() > 0)
	{
		for(int i = 0; i < columnType.scalarIndex.size(); ++i)
		{
			pointCloud->GetScalarFieldByIndex(i)->SetScaleFieldSize(maxSize);
		}
	}*/  
	//是否识别科学计数法
	bool scientificNotation = true;

	const char* loadingStr = "Loading";

	while(file.readLine(currentLine, 500) > 0)
	{
		if (++count <= skippedLines)
		{
			continue;
		}
		//
// 		if (count >= maxSize + skippedLines + 1)
// 		{
// 			break;
// 		}

		//如果存在空行则忽略
		if ('\n' == currentLine[0])
		{
			continue;
		}

		//读取文件的每行内容;
		QStringList line = QString(currentLine).split(separator, QString::SkipEmptyParts);

		//如果所读行的数据不符合所选的类型，则忽略
		if (line.size() < columnType.ValidTypeNumber())
		{
			continue;
		}

		//读取XYZ坐标
		if (columnType.xCoordIndex >= 0)
		{
			point[0] = line[columnType.xCoordIndex].toDouble(&scientificNotation);
		} 
		if (columnType.yCoordIndex >= 0)
		{
			point[1] = line[columnType.yCoordIndex].toDouble(&scientificNotation);
		} 
		if (columnType.zCoordIndex >= 0)
		{
			point[2] = line[columnType.zCoordIndex].toDouble(&scientificNotation);
		} 
		//如果是读入第一个数据，判断是否需要
		if (pointCloud->Size() == 0 && !coordinatesShiftEnabled)
		{
			if (DCCore::CoordinatesShiftManager::Handle(point, shift))
			{
				pointCloud->SetShift(shift);
			}
		}
		else if (pointCloud->Size() == 0 && coordinatesShiftEnabled)
		{
			shift[0] = coordinatesShift[0];
			shift[1] = coordinatesShift[1];
			shift[2] = coordinatesShift[2];
			pointCloud->SetShift(shift);
		}
		//;
		//将数据读取出来;
		pointCloud->AddPoint(DCVector3D(point[0] + shift[0], point[1] + shift[1], point[2] + shift[2]));

		if (columnType.HasRGBColors())
		{
			if (columnType.rgbRedIndex >= 0)
			{
				color[0] = line[columnType.rgbRedIndex].toFloat();
			}  
			if (columnType.rgbGreenIndex >= 0)
			{
				color[1] = line[columnType.rgbGreenIndex].toFloat();
			}  
			if (columnType.rgbBlueIndex >= 0)
			{
				color[2] = line[columnType.rgbBlueIndex].toFloat();
			}  
			if (columnType.grayIndex >= 0)
			{
				record.gray = line[columnType.grayIndex].toFloat();
			}  
			pointCloud->AddColor(color);
		}
		
		if (columnType.HasNormals())
		{
			if (columnType.xNormalIndex >= 0)
			{
				normal[0] = line[columnType.xNormalIndex].toFloat();
			}  
			if (columnType.yNormalIndex >= 0)
			{
				normal[1] = line[columnType.yNormalIndex].toFloat();
			}  
			if (columnType.zNormalIndex >= 0)
			{
				normal[2] = line[columnType.zNormalIndex].toFloat();
			}  
			pointCloud->AddNormal(normal);
		}

		if (columnType.scalarIndex.size() > 0)
		{
			for(int i = 0; i < columnType.scalarIndex.size(); ++i)
			{
				pointCloud->GetScalarFieldByIndex(i)->AddData(line[columnType.scalarIndex[i]].toDouble());
			}
		}  

		/*if (!nprogress.OneStep())
		{
			DCCore::Logger::Message(QObject::tr("Canceled By User"));
			break;
		}*/

		
		//应用采样率
		for (unsigned i = 0; i < samplingRate - 1; ++i)
		{
			file.readLine(currentLine, 500);
			count++;
			int percent = count / (maxSize / 100.0);
			if (cb)
			{
				(*cb)(percent, loadingStr);
			}
			
		}

		int percent = count / (maxSize / 100.0);
		if (cb)
		{
			(*cb)(percent, loadingStr);
		}
		
	}

	file.close();
	if (cb)
	{
		(*cb)(99, "Finish");
	}
	

	return pointCloud;
}

bool AsciiIO::SaveToFile(DcGp::DcGpEntity* entity, QString filename)
{
	//写出数据
	QFile file(filename);
	if (!file.open(QFile::WriteOnly | QFile::Truncate))
	{
		return false;
	}
	QTextStream stream(&file);
	//设置格式控制
	stream.setRealNumberNotation(QTextStream::FixedNotation);
	stream.setRealNumberPrecision(3);

	DcGp::DcGpPointCloud* pointCloud = 0;
	if (entity && entity->IsA("DcGpPointCloud"))
	{
		pointCloud = static_cast<DcGp::DcGpPointCloud*>(entity);
	}
	else
	{
		return false;
	}


	//获取个数及颜色法向量等信息
	unsigned long number = pointCloud->Size();
	bool writeColors = pointCloud->HasColors();
	bool writeNormals = pointCloud->HasNormals();
	bool writeField = false;
	if (pointCloud->GetCurrentScalarField())
	{
		writeField = true;
	}
	

	//数据偏移量
	Point_3D shift = pointCloud->GetShift();

	//写出数据
	for (unsigned long i = 0; i != number; ++i)
	{
		//定义一个当前行变量
		QString line;

		//写入当前坐标
		Point_3D point = Point_3D::FromArray(pointCloud->GetPoint(i).u);
		line.append(QString("%1").arg(point.x - shift.x, 0, 'f', 3));
		line.append(" "); //添加分隔符
		line.append(QString("%1").arg(point.y - shift.y, 0, 'f', 3));
		line.append(" "); //添加分隔符
		line.append(QString("%1").arg(point.z - shift.z, 0, 'f', 3));
		line.append(" "); //添加分隔符

		//定义一个当前行变量存储颜色值
		QString color;
		if (writeColors)
		{
			//添加rgb颜色
			PointColor rgbColor = pointCloud->GetPointColor(i);
			color.append(" ");
			color.append(QString::number(rgbColor[0]));
			color.append(" ");
			color.append(QString::number(rgbColor[1]));
			color.append(" ");
			color.append(QString::number(rgbColor[2]));

			line.append(color);
		}

		//定义一个当前行变量存储法量值
		QString normal;
		if (writeNormals)
		{
			//添加normal
			DCVector3D pNormal = pointCloud->GetPointNormal(i);
			normal.append(" ");
			normal.append(QString::number(pNormal[0]));
			normal.append(" ");
			normal.append(QString::number(pNormal[1]));
			normal.append(" ");
			normal.append(QString::number(pNormal[2]));

			line.append(normal);
		}

		//定义一个当前行存储field
		QString field;
		if (writeField)
		{
			//添加当前field值
			//获取当前点对应的标量场值
			double value = pointCloud->GetCurrentScalarField()->GetPointScalarValue(i);
			field.append(" ");
			field.append(QString::number(value));

			line.append(field);
		}

		stream << line << "\n";
	}

	file.close();
	return true;
}