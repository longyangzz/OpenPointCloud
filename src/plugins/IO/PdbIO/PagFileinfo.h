#pragma once

#include "QString"
#include "QStringList"
#include "QFileInfo"
#include "QDir"

struct FileInfo
{
	//! 文件名字实例
	//! changshu_L0_X0_Y0.pfa

	FileInfo(const QString& inFilename)
		: levelID("-1")
		, xID("-1")
		, yID("-1")
		, zID("-1")
		, suffix(".pdb")
	{
		//根据传入文件名，初始化各参数的值
		PraseFile(inFilename);
	}

	QStringList GenNameByIndex(int level, QString index)
	{
		QStringList list;

		if (fAlg == QuadTree)
		{
			//行列号
			unsigned column = index.split("_").at(0).toInt() * 2;
			unsigned row = index.split("_").at(1).toInt() * 2;

			//! 跟路径 + 分页文件格式名 + 后缀名
			list.push_back(absoultFilePath + "\\" + QString("%1_L%2_X%3_Y%4").arg(rootFileName).arg(level).arg(column).arg(row) + suffix);
			list.push_back(absoultFilePath + "\\" + QString("%1_L%2_X%3_Y%4").arg(rootFileName).arg(level).arg(column + 1).arg(row) + suffix);
			list.push_back(absoultFilePath + "\\" + QString("%1_L%2_X%3_Y%4").arg(rootFileName).arg(level).arg(column).arg(row + 1) + suffix);
			list.push_back(absoultFilePath + "\\" + QString("%1_L%2_X%3_Y%4").arg(rootFileName).arg(level).arg(column + 1).arg(row + 1) + suffix);

			/*list << "F:\\testbig\\process\\changshu_pfa\\changshu_pagefile\\changshu_L1_X0_Y0.pfa"
			<< "F:\\testbig\\process\\changshu_pfa\\changshu_pagefile\\changshu_L1_X1_Y0.pfa"
			<< "F:\\testbig\\process\\changshu_pfa\\changshu_pagefile\\changshu_L1_X0_Y1.pfa"
			<< "F:\\testbig\\process\\changshu_pfa\\changshu_pagefile\\changshu_L1_X1_Y1.pfa";*/
		}
		else if (fAlg == Octree)
		{
			//行列号
			QStringList iList = index.split("_");
			Q_ASSERT(iList.size() == 3);
			unsigned column = iList[0].toInt() * 2;
			unsigned row = iList[1].toInt() * 2;
			unsigned layer = iList[2].toInt() * 2;

			list.push_back(QString("%1_L%2_X%3_Y%4_Z%5").arg(level).arg(column).arg(row));
			list.push_back(QString("%1_L%2_X%3_Y%4_Z%5").arg(level).arg(column + 1).arg(row));
			list.push_back(QString("%1_L%2_X%3_Y%4_Z%5").arg(level).arg(column).arg(row + 1));
			list.push_back(QString("%1_L%2_X%3_Y%4_Z%5").arg(level).arg(column + 1).arg(row + 1));

			list.push_back(QString("%1_L%2_X%3_Y%4_Z%5").arg(level).arg(column).arg(row));
			list.push_back(QString("%1_L%2_X%3_Y%4_Z%5").arg(level).arg(column + 1).arg(row));
			list.push_back(QString("%1_L%2_X%3_Y%4_Z%5").arg(level).arg(column).arg(row + 1));
			list.push_back(QString("%1_L%2_X%3_Y%4_Z%5").arg(level).arg(column + 1).arg(row + 1));
		}

		return list;
	}

	void PraseFile(const QString& inFilename)
	{
		//判断文件名的basename中是否包含_字符
		QFileInfo fInfo(inFilename);
		QString baseName = fInfo.baseName();
		absoultFilePath = QDir::toNativeSeparators(fInfo.absolutePath());
		rootFileName = baseName.split("_").at(0);

		bool hasUnderline = baseName.contains("_");
		int underLineNum = baseName.count("_");

		//! 设置issubfile
		isSubfile = hasUnderline;

		//设置文件分割算法，用来确定子文件个数
		if (underLineNum == 3)
		{
			fAlg = QuadTree;

			//! 初始化ID值及list
			//! 分割basename
			QStringList bnList = baseName.split("_");
			levelID = bnList[1].remove(0, 1);
			xID = bnList[2].remove(0, 1);
			yID = bnList[3].remove(0, 1);

			//添加subfile,根据当前的id号生成filename
			QString index = QString("%1_%2").arg(xID).arg(yID);
			subFileList = GenNameByIndex(levelID.toInt() + 1, index);
		}
		else if (underLineNum == 4)
		{
			fAlg = Octree;
			//! 初始化ID值及list
			//! 分割basename
			QStringList bnList = baseName.split("_");
			levelID = bnList[1].remove(0, 1);
			xID = bnList[2].remove(0, 1);
			yID = bnList[3].remove(0, 1);
			zID = bnList[4].remove(0, 1);

		}
		else
		{
			fAlg = Unknown;
		}

	}

	enum FileAlgorithm
	{
		QuadTree = 0,
		Octree = 1,
		Unknown = 2
	};

	//文件名字参数
	QString absoultFilePath;
	QString rootFileName;
	QString suffix;

	QString levelID;
	QString xID;
	QString yID;
	QString zID;
	QStringList subFileList;

	FileAlgorithm fAlg;

	bool isSubfile;
};
