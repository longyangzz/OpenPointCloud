// -*-c++-*-

/*
 * $Id: ReaderWriterTXT.cpp 11536 2010-05-28 17:08:30Z robert $
 *
 * STL importer for OpenSceneGraph.
 * Copyright (c)2004 Ulrich Hertlein <u.hertlein@sandbox.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "QFileInfo"
#include "QFile"
#include "QDir"

#include <OSG/Notify>
#include <OSG/Endian>

#include <OSGDb/Registry>
#include <OSGDb/ReadFile>
#include <OSGDb/FileNameUtils>
#include <OSGDb/FileUtils>

#include <OsgUtil/TriStripVisitor>
#include <OsgUtil/SmoothingVisitor>
#include <OSG/TriangleFunctor>

#include <OSG/Geode>
#include <OSG/Geometry>
#include "OSG/PagedLOD"

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <string.h>

#include "QTextStream"
#include "QFile"
#include "QRegExp"
#include "QStringList"


struct FileInfo
{
	//! 文件名字实例
	//! changshu_L0_X0_Y0.pfa

	FileInfo(const QString& inFilename)
		: levelID("-1")
		, xID("-1")
		, yID("-1")
		, zID("-1")
		, suffix(".txt")
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
			list.push_back(absoultFilePath + "\\" + QString("%1_L%2_X%3_Y%4").arg(rootFileName).arg(level).arg(column+1).arg(row) + suffix);
			list.push_back(absoultFilePath + "\\" + QString("%1_L%2_X%3_Y%4").arg(rootFileName).arg(level).arg(column).arg(row+1) + suffix);
			list.push_back(absoultFilePath + "\\" + QString("%1_L%2_X%3_Y%4").arg(rootFileName).arg(level).arg(column+1).arg(row+1) + suffix);

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
			list.push_back(QString("%1_L%2_X%3_Y%4_Z%5").arg(level).arg(column+1).arg(row));
			list.push_back(QString("%1_L%2_X%3_Y%4_Z%5").arg(level).arg(column).arg(row+1));
			list.push_back(QString("%1_L%2_X%3_Y%4_Z%5").arg(level).arg(column+1).arg(row+1));

			list.push_back(QString("%1_L%2_X%3_Y%4_Z%5").arg(level).arg(column).arg(row));
			list.push_back(QString("%1_L%2_X%3_Y%4_Z%5").arg(level).arg(column+1).arg(row));
			list.push_back(QString("%1_L%2_X%3_Y%4_Z%5").arg(level).arg(column).arg(row+1));
			list.push_back(QString("%1_L%2_X%3_Y%4_Z%5").arg(level).arg(column+1).arg(row+1));
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
			levelID = bnList[1].remove(0,1);
			xID = bnList[2].remove(0,1);
			yID = bnList[3].remove(0,1);

			//添加subfile,根据当前的id号生成filename
			QString index = QString("%1_%2").arg(xID).arg(yID);
			subFileList = GenNameByIndex(levelID.toInt()+1, index);
		}
		else if (underLineNum == 4)
		{
			fAlg = Octree;
			//! 初始化ID值及list
			//! 分割basename
			QStringList bnList = baseName.split("_");
			levelID = bnList[1].remove(0,1);
			xID = bnList[2].remove(0,1);
			yID = bnList[3].remove(0,1);
			zID = bnList[4].remove(0,1);

		}
		else
		{
			fAlg = Unknown;
		}

	}

	enum FileAlgorithm
	{
		QuadTree			=	0,
		Octree				=	1,
		Unknown				=	2
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
/**
 * txt importer for OpenSceneGraph.
 */
class ReaderWriterTXT : public osgDB::ReaderWriter
{
public:
    ReaderWriterTXT()
    {
        //supportsExtension("txt","STL binary format");
        supportsExtension("txt","TXT ASCII format");
        //supportsOption("smooth", "run SmoothingVisitor");
        //supportsOption("separateFiles", "Save every geode in a different file. Can be a Huge amount of Files!!!");
    }

    virtual const char* className() const {
        return "TXT Reader";
    }

    virtual ReadResult readNode(const std::string& fileName, const osgDB::ReaderWriter::Options*) const;
    virtual WriteResult writeNode(const osg::Node& /*node*/,const std::string& /*fileName*/,const Options* =NULL) const ;
private:
	osg::PagedLOD* CreatePageLodByfile(QString&  qFilename, const osgDB::ReaderWriter::Options* options, bool isRootFile = false) const;

private:

    struct ReaderObject
    {
        ReaderObject():
            _generateNormal(true),
            _numFacets(0) {}

        bool _generateNormal;
        unsigned int _numFacets;

        osg::ref_ptr<osg::Vec3Array> _vertex;
        osg::ref_ptr<osg::Vec3Array> _normal;
        osg::ref_ptr<osg::Vec4Array> _color;

        bool readTXTAscii(QFile& file);
        //bool readStlBinary(FILE* fp);
    };

  class CreateStlVisitor : public osg::NodeVisitor {
  public:

    CreateStlVisitor( std::string const & fout, const osgDB::ReaderWriter::Options* options = 0): osg::NodeVisitor( osg::NodeVisitor::TRAVERSE_ACTIVE_CHILDREN ), counter(0), m_fout(fout), m_options(options)
    {
      if (options && (options->getOptionString() == "separateFiles"))
      {
        OSG_INFO << "ReaderWriterTXT::writeNode: Files are seperated written" << std::endl;
      } else {
        m_f = new osgDB::ofstream(m_fout.c_str());        
        *m_f << "solid " << counter << std::endl;
      }
    };

    std::string i2s( int i) {
      char buf[16];  // -2^31 == -2147483648 needs 11 chars + \0  -> 12 (+4 for security ;-)
      sprintf(buf,"%d",i);
      return buf;
    }

    virtual void apply(  osg::Geode& node ){ 
      osg::Matrix mat = osg::computeLocalToWorld( getNodePath() );
      
      if (m_options && (m_options->getOptionString() == "separateFiles")) {
        std::string sepFile = m_fout + i2s(counter);
        m_f = new osgDB::ofstream(sepFile.c_str());
        *m_f << "solid " << std::endl;
      }
      
      for ( unsigned int i = 0; i < node.getNumDrawables(); ++i ) {
        osg::TriangleFunctor<PushPoints> tf;
        tf.m_stream = m_f;
        tf.m_mat = mat;
        node.getDrawable( i )->accept( tf );
      }
      
      if (m_options && (m_options->getOptionString() == "separateFiles")) {
        *m_f << "endsolid " << std::endl;
        m_f->close();
        delete m_f;
      }
      
      ++counter;
      traverse(node);
      
    }
    //        nHandle->SetLocation( Frame( mat ) );
    ~CreateStlVisitor() {
      if (m_options && (m_options->getOptionString() == "separateFiles")) {
        OSG_INFO << "ReaderWriterTXT::writeNode: " << counter-1 << "Files were written" << std::endl;
      } else {
        *m_f << "endsolid " << std::endl;
        m_f->close();
        delete m_f;
      }
    }
    
    const std::string& getErrorString() const { return m_ErrorString; }
    
  private:
    int counter;
    std::ofstream* m_f;
    std::string m_fout;
    osgDB::ReaderWriter::Options const * m_options;
    std::string m_ErrorString;
    
    
    struct PushPoints {
      std::ofstream* m_stream;
      osg::Matrix m_mat;
      inline void operator () ( const osg::Vec3& _v1, const osg::Vec3& _v2, const osg::Vec3& _v3, bool treatVertexDataAsTemporary ) {
        osg::Vec3 v1 = _v1 * m_mat;
        osg::Vec3 v2 = _v2 * m_mat;
        osg::Vec3 v3 = _v3 * m_mat;
        osg::Vec3 vV1V2 = v2-v1;
        osg::Vec3 vV1V3 = v3-v1;
        osg::Vec3 vNormal = vV1V2.operator ^(vV1V3);
        *m_stream << "facet normal " << vNormal[0] << " " << vNormal[1] << " " << vNormal[2] << std::endl;
        *m_stream << "outer loop" << std::endl;
        *m_stream << "vertex " << v1[0] << " " << v1[1] << " " << v1[2] << std::endl;
        *m_stream << "vertex " << v2[0] << " " << v2[1] << " " << v2[2] << std::endl;
        *m_stream << "vertex " << v3[0] << " " << v3[1] << " " << v3[2] << std::endl;
        *m_stream << "endloop" << std::endl;
        *m_stream << "endfacet " << std::endl;
      }
      
    };
    
    
  };
  
  
       
};

osg::PagedLOD* ReaderWriterTXT::CreatePageLodByfile(QString&  qFilename, const osgDB::ReaderWriter::Options* options, bool isRootFile/* = false*/) const
{
		QFile inFile(qFilename);
		if (!inFile.open(QIODevice::ReadOnly))
		{
			return nullptr;
		}

		char currentLine[500];
		char boxLine[500];
		//1.判断文件头是否存在对应的boxfile
		QFileInfo boxFileinfo(qFilename);
		QString absultPath = boxFileinfo.absolutePath();
		QString basename = boxFileinfo.baseName();
		
		//根据isRootFile确定关联文件名
		QString assioFilename;
		if (isRootFile)
		{
			//changshu_L0_X0_Y0.txt
			assioFilename = QString("%1_root\\%2_L0_X0_Y0.txt").arg(basename).arg(basename);
		}
		else
		{
			assioFilename = basename+".txt";
		}

		QString boxFilename = QString("%1\\%2_box.txt").arg(absultPath).arg(basename);
		QFile boxFile(boxFilename);
		if (!boxFile.exists())
		{
			return nullptr;
		}

		//根据数据文件名和box文件名解析参数并，创建pagelod节点
		osg::PagedLOD* pageLod = new osg::PagedLOD;
		QFileInfo info(qFilename);
		QString dataPath = info.absolutePath() + "\\";
		pageLod->setDatabasePath(dataPath.toStdString());

		//从boxfile中取出box边界盒坐标值--指定pagelod参数
		if (!boxFile.open(QIODevice::ReadOnly))
		{
			return nullptr;
		}
		boxFile.readLine(boxLine, 500);
		QStringList boxLineList = QString(boxLine).split(QRegExp(",|\\s+"), QString::SkipEmptyParts);
		Q_ASSERT(boxLineList.size() == 6 || boxLineList.size() == 9);
		osg::Vec3d minCorner(boxLineList[0].toDouble(), boxLineList[1].toDouble(), boxLineList[2].toDouble());
		osg::Vec3d maxCorner(boxLineList[3].toDouble(), boxLineList[4].toDouble(), boxLineList[5].toDouble());

		//计算range
		double rangeValue = sqrt(0.25*((maxCorner-minCorner).length2())) * 7;

		//设置center
		osg::Vec3d center = (maxCorner+minCorner)*0.5;
		pageLod->setCenter(center);

		//默认设置mode
		pageLod->setRangeMode(osg::LOD::RangeMode(0));

		//设置rangelist
		osg::LOD::RangeList rangeList;
		{
			osg::LOD::MinMaxPair minmax0(rangeValue, 10000000000);
			rangeList.push_back(minmax0);

			osg::LOD::MinMaxPair minmax1(0, rangeValue);
			rangeList.push_back(minmax1);
		}
		pageLod->setRangeList(rangeList);

		//设置rangFilename
		pageLod->setFileName(0, "");
		pageLod->setFileName(1, assioFilename.toStdString());

		//接下来解析坐标数据
		//添加drawable数据
		ReaderObject readerObject;
		readerObject.readTXTAscii(inFile);

		boxFile.close();
		inFile.close();
		/*
		 * setup geometry
		 */
		osg::Geometry* geom = new osg::Geometry;
		geom->setVertexArray(readerObject._vertex.get());

		geom->setNormalArray(readerObject._normal.get());
		geom->setNormalBinding(osg::Geometry::BIND_PER_PRIMITIVE);

		if (readerObject._color.valid()) {
			OSG_INFO << "TXT file with color" << std::endl;
			geom->setColorArray(readerObject._color.get());
			geom->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE);
		}

		geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS, 0 , readerObject._vertex->size()));

		osg::Geode* geode = new osg::Geode;
		geode->addDrawable(geom);
    
		if (options && (options->getOptionString() == "smooth")) {
			osgUtil::SmoothingVisitor smooter;
			geode->accept(smooter);
		}

		//添加实体对象
		pageLod->addChild(geode);

		return pageLod;
}


// Read node
osgDB::ReaderWriter::ReadResult ReaderWriterTXT::readNode(const std::string& file, const osgDB::ReaderWriter::Options* options) const
{
    std::string ext = osgDB::getLowerCaseFileExtension(file);
    if (!acceptsExtension(ext)) return ReadResult::FILE_NOT_HANDLED;

    std::string fileName = osgDB::findDataFile( file, options );
    if (fileName.empty()) return ReadResult::FILE_NOT_FOUND;

	//打开文件读
	QString qFilename(fileName.c_str());

	//根据文件名提取文件标号
	FileInfo fLabel(qFilename);

	//2.根据文件标号创建什么类型节点（单个pagelod还是 Group）
	//一个pagelod(文件名中不带_分割)
	if(!fLabel.isSubfile)
	{
		osg::PagedLOD* pageLod = CreatePageLodByfile(qFilename, options, true);

		return pageLod;
	}
	else
	{
		//主要是从fLabel中取出下一级关联的文件，可能八个也可能四个
		QStringList ftList = fLabel.subFileList;
		/*fList << "F:\\testbig\\process\\changshu_pfa\\changshu_pagefile\\changshu_L1_X0_Y0.pfa"
		<< "F:\\testbig\\process\\changshu_pfa\\changshu_pagefile\\changshu_L1_X1_Y0.pfa"
		<< "F:\\testbig\\process\\changshu_pfa\\changshu_pagefile\\changshu_L1_X0_Y1.pfa"
		<< "F:\\testbig\\process\\changshu_pfa\\changshu_pagefile\\changshu_L1_X1_Y1.pfa";*/
		osg::Group* group = new osg::Group;
		for (auto i = 0; i != ftList.size(); ++i)
		{
			//根据文件名生成一个group节点，包含四个子节点
			//根据文件名，创建pagelod节点
			try
			{
				osg::PagedLOD* pageLod = CreatePageLodByfile(ftList[i], options);
				group->addChild(pageLod);
				
			}
			catch (...)
			{
				auto aa = 5;
			}
			
		}
		if (group->getNumChildren() == 0)
		{
			return nullptr;
		}
		
		return group;

	}

    return ReadResult::FILE_NOT_HANDLED;;
}


/**********************************************************************
 *
 * Private
 *
 **********************************************************************/

bool ReaderWriterTXT::ReaderObject::readTXTAscii(QFile& file)
{
	char currentLine[500];

	while(file.readLine(currentLine, 500) > 0)
	{
		//QString line = stream.readLine();
		QStringList list = QString(currentLine).split(QRegExp(",|\\s+"), QString::SkipEmptyParts);

		if (!_vertex.valid())
			_vertex = new osg::Vec3Array;

		if (list.size() >= 3) 
		{
			float vx = list[0].toFloat();
			float vy = list[1].toFloat();
			float vz = list[2].toFloat();
			_vertex->push_back(osg::Vec3(vx,vy,vz));
		}
	}

	return true;
}

osgDB::ReaderWriter::WriteResult ReaderWriterTXT::writeNode(const osg::Node& node,const std::string& fileName, const Options* opts) const
{
    if (fileName.empty()) return WriteResult::FILE_NOT_HANDLED;

    std::string ext = osgDB::getLowerCaseFileExtension(fileName);
    if (ext != "stl" )
    {
        // sta - extension implies STL-Binary...
        OSG_INFO << "ReaderWriterTXT::writeNode: Only STL-ASCII-files supported'" << std::endl;
        return WriteResult::FILE_NOT_HANDLED;
    }
  
    CreateStlVisitor createStlVisitor( fileName, opts );
    const_cast<osg::Node&>(node).accept( createStlVisitor );

    if (createStlVisitor.getErrorString().empty())
    {
        return WriteResult::FILE_SAVED;
    }
    else
    {
        OSG_NOTICE<<"Error: "<<createStlVisitor.getErrorString()<<std::endl;
        return WriteResult::ERROR_IN_WRITING_FILE;
    }
}

// Register with Registry to instantiate the above reader/writer.
REGISTER_OSGPLUGIN(txt, ReaderWriterTXT)