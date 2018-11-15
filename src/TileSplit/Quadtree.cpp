//Qt
#include "QFileInfo"
#include "QStringList"
#include "QTextStream"
#include "QDebug"

#include <math.h>

#include "Quadtree.h"
#include "Quadnode.h"
#include "BoundBox.h"
#include "NodeDebug.h"

#include "DCCore/Logger.h"

using namespace DC::Pyra;

double loadnum = 0;

Quadtree::Quadtree(int depth, BoundBox* box, QString fileName)
	: m_root(nullptr)
	, m_depth(depth)
	, m_boundBox(box)
	, m_nodeDebug(new NodeDebug)
	, m_isDisInited(false)
	, m_initDis(-1.0)
{
	//创建根节点
	if (!m_root)
	{
		m_root = new Quadnode;
	}

	InitGenBaseNameFromOther(fileName);

	if (!m_boundBox)
	{
		InitBoxAndDepthFromFile();
	}

	//根节点对应的层级号为1.
	m_root->SetLevel(1);
	m_root->SetBoundBox(m_boundBox);
	m_root->SetFullName(fileName);

	//创建子节点
	QuadCreateBranch(m_root);
}

Quadtree::Quadtree(QString fileName)
	: m_root(nullptr)
	, m_boundBox(nullptr)
	, m_nodeDebug(new NodeDebug)
	, m_isDisInited(false)
	, m_initDis(-1.0)
{
	//创建根节点
	if (!m_root)
	{
		m_root = new Quadnode;
	}

	InitGenBaseNameFromOther(fileName);

	if (!m_boundBox)
	{
		InitBoxAndDepthFromFile();
	}

	//根节点对应的层级号为1.
	m_root->SetLevel(1);
	m_root->SetBoundBox(m_boundBox);
	m_root->SetFullName(fileName);

	//创建子节点
	QuadCreateBranch(m_root);
}

Quadtree::~Quadtree()
{

}

void Quadtree::QuadCreateBranch(Quadnode* parent)
{
	//只要没有达到最大深度，就对父对象执行分割
	if (parent->GetLevel() < m_depth)
	{
		//将box划分成四份，创建顺序为左下，左上，右下，右上，并添加到父节点中


		Quadnode* leftDown = new Quadnode;
		BoundBox* leftDownBox = new BoundBox(parent->GetBoundBox()->m_minCorner, parent->GetBoundBox()->GetCenter());
		leftDown->SetBoundBox(leftDownBox);
		//根据box值，确定当前box在当前level下，对应的文件名值
		unsigned columnIndex = 0, rowIndex = 0;
		m_root->GetBoundBox()->GetPointIndexInLevel(leftDownBox->GetCenter(), parent->GetLevel() + 1, columnIndex, rowIndex);
		QString fName = m_dir + "\\" + m_baseFileName + QString("_L%1_X%2_Y%3.").arg(parent->GetLevel() + 1).arg(columnIndex).arg(rowIndex) + m_suffix;
		
		leftDown->SetFullName(fName);
		leftDown->SetLevel(parent->GetLevel() + 1);
		parent->AddChild(leftDown);
		m_nodeDebug->AddDebugRectangle(leftDownBox->m_minCorner, leftDownBox->m_maxCorner);
		QuadCreateBranch(leftDown);

		Quadnode* leftup = new Quadnode;
		BoundBox* leftupBox = new BoundBox(parent->GetBoundBox()->m_minCorner + Point3D(0, parent->GetBoundBox()->m_width/2.0, 0), parent->GetBoundBox()->GetCenter() + Point3D(0, parent->GetBoundBox()->m_width/2.0, 0));
		leftup->SetBoundBox(leftupBox);
		m_root->GetBoundBox()->GetPointIndexInLevel(leftupBox->GetCenter(), parent->GetLevel() + 1, columnIndex, rowIndex);
		fName = m_dir + "\\" + m_baseFileName + QString("_L%1_X%2_Y%3.").arg(parent->GetLevel() + 1).arg(columnIndex).arg(rowIndex) + m_suffix;
		leftup->SetFullName(fName);
		leftup->SetLevel(parent->GetLevel() + 1);
		parent->AddChild(leftup);
		m_nodeDebug->AddDebugRectangle(leftupBox->m_minCorner, leftupBox->m_maxCorner);
		QuadCreateBranch(leftup);

		Quadnode* rightDown = new Quadnode;
		BoundBox* rightDownBox = new BoundBox(parent->GetBoundBox()->m_minCorner + Point3D(parent->GetBoundBox()->m_length / 2.0, 0.0, 0), parent->GetBoundBox()->GetCenter() + Point3D(parent->GetBoundBox()->m_length / 2.0, 0.0, 0));
		rightDown->SetBoundBox(rightDownBox);
		m_root->GetBoundBox()->GetPointIndexInLevel(rightDownBox->GetCenter(), parent->GetLevel() + 1, columnIndex, rowIndex);
		fName = m_dir + "\\" + m_baseFileName + QString("_L%1_X%2_Y%3.").arg(parent->GetLevel() + 1).arg(columnIndex).arg(rowIndex) + m_suffix;
		rightDown->SetFullName(fName);
		rightDown->SetLevel(parent->GetLevel() + 1);
		parent->AddChild(rightDown);
		m_nodeDebug->AddDebugRectangle(rightDownBox->m_minCorner, rightDownBox->m_maxCorner);
		QuadCreateBranch(rightDown);

		Quadnode* rightUp = new Quadnode;
		BoundBox* rightUpBox = new BoundBox(parent->GetBoundBox()->m_minCorner + Point3D(parent->GetBoundBox()->m_length / 2.0, parent->GetBoundBox()->m_width/2.0, 0), parent->GetBoundBox()->GetCenter() + Point3D(parent->GetBoundBox()->m_length / 2.0, parent->GetBoundBox()->m_width/2.0, 0));
		rightUp->SetBoundBox(rightUpBox);
		m_root->GetBoundBox()->GetPointIndexInLevel(rightUpBox->GetCenter(), parent->GetLevel() + 1, columnIndex, rowIndex);
		fName = m_dir + "\\" + m_baseFileName + QString("_L%1_X%2_Y%3.").arg(parent->GetLevel() + 1).arg(columnIndex).arg(rowIndex) + m_suffix;
		rightUp->SetFullName(fName);
		rightUp->SetLevel(parent->GetLevel() + 1);
		parent->AddChild(rightUp);
		m_nodeDebug->AddDebugRectangle(rightUpBox->m_minCorner, rightUpBox->m_maxCorner);
		QuadCreateBranch(rightUp);
	}
}

Quadnode* Quadtree::GetRoot() const
{
	return m_root;
}

QString Quadtree::GetBaseFileName() const
{
	return m_baseFileName;
}

uint Quadtree::CaluLoadedLevel(float lenth, uint depth, const float& height)
{
	
	int pixelSize = ceil(m_boundBox->m_length / lenth) + 3;

	//层数 默认为0
	//计算最小距离值
	//double minDist = m_initDis / pow(float(2.0),float(depth -1));
	////计算层数
	//int layer = depth - (depth -1) /(m_initDis - minDist) * (dist -minDist);
	//if (layer <= 0)
	//{
	//	layer = 1;
	//}
	//return layer;
	if (pixelSize > depth)
	{
		return depth;
	}
	return pixelSize;
}



void Quadtree::Draw(const float& actualPsize, std::array<std::array<float, 4> ,6> frustum, const float& inter)
{
	//根据距离值决定取哪层节点，根据box过滤层内的某一部分节点
	//遍历m_root,绘制符合条件的,传入参数是level和视景体
	if (!m_isDisInited)
	{
		m_initDis = actualPsize;
		m_isDisInited = true;
	}
	uint level = CaluLoadedLevel(actualPsize, m_depth, inter);
	DCCore::Logger::Message("level:" + QString::number(level));
	loadnum = 0;
	m_root->Draw(level, frustum, m_depth);
	DCCore::Logger::Message("DrawNum:" + QString::number(loadnum));
	//m_nodeDebug->RenderDebugLines();
}

BoundBox* Quadtree::GetBoundBox() const
{
	return m_boundBox;
}

void Quadtree::InitGenBaseNameFromOther(QString name)
{
	QFileInfo info(name);
	m_baseFileName = info.baseName().split("_").at(0);
	m_dir = info.absolutePath();
	m_suffix = info.suffix();
}

void Quadtree::InitBoxAndDepthFromFile()
{
	//获取box对应的文件名
	QString boxFile = m_dir + "/" + m_baseFileName + QString("_%1.%2").arg(BOXEXT).arg("box");

	//解析boxfile并初始化m_boundBox;
	QFile inFile(boxFile);
	if (!inFile.open(QIODevice::ReadOnly))
	{
		return;
	}

	QTextStream stream(&inFile);

	Point3D leftCornerPoint;
	Point3D rightCornerPoint;

	//读取depth信息，第一行
	QString line = inFile.readLine();
	m_depth = line.toUInt();

	//读取两个点坐标
	line = inFile.readLine();
	QStringList list = line.split(",");
	if (list.size() != 3)
	{
		return;
	}
	leftCornerPoint = Point3D(list[0].toDouble(), list[1].toDouble(), list[2].toDouble());
	
	line = inFile.readLine();
	list = line.split(",");
	if (list.size() != 3)
	{
		return;
	}
	rightCornerPoint = Point3D(list[0].toDouble(), list[1].toDouble(), list[2].toDouble());

	m_boundBox = new BoundBox(leftCornerPoint, rightCornerPoint);

	inFile.close();
}