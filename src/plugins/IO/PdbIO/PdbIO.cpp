#include "PdbIO.h"

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
#include "DCGp/Custom/PageLodEntity.h"

#include "PagFileinfo.h"


using namespace DcIo;





DCCore::IoPlugin* PdbIO::s_instance = 0;
QStringList PdbIO::SUPPORT_EXTENSION = QStringList() << "pdb";

DCCore::IoPlugin* LoadPlugin(const char* extension)
{
	return PdbIO::Instance(extension);
}

void UnloadPlugin()
{
	//PdbIO::UnloadInstance();
}

DCCore::IoPlugin* PdbIO::Instance(const char* extension)
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
			s_instance = new PdbIO();
		}
		return s_instance;
	}
	else
	{
		return nullptr;
	}
}

void PdbIO::UnloadInstance()
{
	if (s_instance)
	{
		delete s_instance;
		s_instance = 0;
	}
}

PdbIO::PdbIO()
	: DCCore::IoPlugin()
{
	
}

PdbIO::~PdbIO()
{
	
}

QString PdbIO::GetName()
{
	return QString("PDBIO");
}

void PdbIO::SetBoxInfo(DcGp::PageLodEntity* pointCloud, QString qFilename, bool isRootFile/* = false*/) const
{
	if (!pointCloud)
	{
		return;
	}

	//1.判断文件头是否存在对应的boxfile
	QFileInfo boxFileinfo(qFilename);
	QString absultPath = boxFileinfo.absolutePath();
	QString basename = boxFileinfo.baseName();

	//根据isRootFile确定关联文件名
	QString assioFilename;
	if (isRootFile)
	{
		//changshu_L0_X0_Y0.txt
		assioFilename = QString("%1\\%2_L0_X0_Y0.pdb").arg(basename).arg(basename);
	}
	else
	{
		assioFilename = basename + ".pdb";
	}

	QString boxFilename = QString("%1\\%2_box.pdb").arg(absultPath).arg(basename);
	QFile boxFile(boxFilename);
	if (!boxFile.exists())
	{
		return;
	}

	//根据数据文件名和box文件名解析参数并，创建pagelod节点
	QString dataPath = absultPath + "\\";
	pointCloud->SetDatabasePath(dataPath);

	//从boxfile中取出box边界盒坐标值--指定pagelod参数
	if (!boxFile.open(QIODevice::ReadOnly))
	{
		return;
	}
	char boxLine[500];
	boxFile.readLine(boxLine, 500);
	QStringList boxLineList = QString(boxLine).split(QRegExp(",|\\s+"), QString::SkipEmptyParts);
	Q_ASSERT(boxLineList.size() == 6 || boxLineList.size() == 9);
	DCVector3D minCorner(boxLineList[0].toDouble(), boxLineList[1].toDouble(), boxLineList[2].toDouble());
	DCVector3D maxCorner(boxLineList[3].toDouble(), boxLineList[4].toDouble(), boxLineList[5].toDouble());

	//计算range
	float rangeValue = sqrt(0.25*((maxCorner - minCorner).LengthSquared())) * 7;

	//设置center
	DCVector3D center = (maxCorner + minCorner)*0.5;
	pointCloud->SetCenter(center + DCVector3D(pointCloud->GetShift().x, pointCloud->GetShift().y, pointCloud->GetShift().z));


	//设置rangelist
	DcGp::Range range(0.0f, rangeValue);
	pointCloud->SetRange(range);

	//设置rangFilename
	pointCloud->SetFilename(0, assioFilename);
}

//加载文件
DcGp::DcGpEntity* PdbIO::LoadFile(const QString& filename, 
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

	//! 默认的在线程中加载的时候，只支持默认参数情况
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

	//! 根据文件信息判断
	//根据文件名提取文件标号
	FileInfo fLabel(filename);


	//2.根据文件标号创建什么类型节点（单个pagelod还是 Group）
	//一个pagelod(文件名中不带_分割)
	if (!fLabel.isSubfile)
	{
		DcGp::PageLodEntity* pagEntity = LoadCloudFromFormatedAsciiFile(filename,
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

		if (pagEntity)
		{
			SetBoxInfo(pagEntity, filename, true);

			return pagEntity;
		}
		
		return nullptr;
	}
	else
	{
		//主要是从fLabel中取出下一级关联的文件，可能八个也可能四个
		QStringList ftList = fLabel.subFileList;
		/*fList << "F:\\testbig\\process\\changshu_pfa\\changshu_pagefile\\changshu_L1_X0_Y0.pfa"
		<< "F:\\testbig\\process\\changshu_pfa\\changshu_pagefile\\changshu_L1_X1_Y0.pfa"
		<< "F:\\testbig\\process\\changshu_pfa\\changshu_pagefile\\changshu_L1_X0_Y1.pfa"
		<< "F:\\testbig\\process\\changshu_pfa\\changshu_pagefile\\changshu_L1_X1_Y1.pfa";*/
		
		//!返回实体组
		DcGp::DcGpEntity* group = new DcGp::DcGpEntity("group");
		for (auto i = 0; i != ftList.size(); ++i)
		{
			//根据文件名生成一个group节点，包含四个子节点
			//根据文件名，创建pagelod节点
			try
			{

				DcGp::PageLodEntity* pagEntity = LoadCloudFromFormatedAsciiFile(ftList[i],
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
				if (pagEntity)
				{
					SetBoxInfo(pagEntity, ftList[i], false);

					group->AddChild(pagEntity);
				}
				

			}
			catch (...)
			{
				auto aa = 5;
			}

		}

		if (group->GetChildrenNumber())
		{
			return group;
		}
		else
		{
			delete group;
			group = nullptr;
			return nullptr;
		}
		
	}
	
	
}

//从文件中加载数据
DcGp::PageLodEntity* PdbIO::LoadCloudFromFormatedAsciiFile(const QString& fileName,
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
	DcGp::PageLodEntity* pointCloud = new DcGp::PageLodEntity(QFileInfo(fileName).baseName());

	PerLineData record;
	Point_3D point(0,0,0);
	Point_3D shift(0,0,0);
	//点的默认颜色
	PointColor color = {255, 255, 255};


	//是否识别科学计数法
	bool scientificNotation = true;

	const char* loadingStr = "Loading";

	while(file.readLine(currentLine, 500) > 0)
	{
		if (++count <= skippedLines)
		{
			continue;
		}

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
		
	}

	file.close();
	

	return pointCloud;
}

bool PdbIO::SaveToFile(DcGp::DcGpEntity* entity, QString filename)
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