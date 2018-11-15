//Qt
#include "QStringList"
#include "QFile"
#include "QFileInfo"

#include "BoundBox.h"
#include "Quadnode.h"
#include "PointsGeometry.h"

#include <fstream>				// std::ifstream
#include <iostream>				// std::cout

//liblas
#include <liblas/liblas.hpp>
#include "liblas/point.hpp"
#include "liblas/reader.hpp"
#include "liblas/writer.hpp"
#include "liblas/factory.hpp"

using namespace DC::Pyra;

Quadnode::Quadnode()
	: m_box(nullptr)
	, m_level(-1)
	, m_parent(nullptr)
	, m_isLoaded(false)
	, m_geometry(nullptr)
{
	//实例化的是点云几何体
	m_geometry = new PointsGeometry(this);
}

Quadnode::~Quadnode()
{

}

void Quadnode::SetBoundBox(BoundBox* box)
{
	m_box = box;
}

BoundBox* Quadnode::GetBoundBox() const
{
	return m_box;
}

void Quadnode::SetLevel(const LevelType& level)
{
	m_level = level;
}

void Quadnode::SetFullName(const QString& fullName)
{
	m_fileFullname = fullName;
}

int Quadnode::GetLevel() const
{
	return m_level;
}

void Quadnode::AddChild(Quadnode* child, bool dependant /* = true */)
{
	m_children.push_back(child);

	if (dependant)
	{
		child->SetParent(this);
	}
}

void Quadnode::SetParent(Quadnode* parent)
{
	m_parent = parent;
}

Quadnode* Quadnode::GetParent() const
{
	return m_parent;
}

void Quadnode::Draw(const float& drawLevel, std::array<std::array<float, 4> ,6> frustum, unsigned depth)
{
	//确定当前box是否在视景体内
	bool isBoxInFrustum = BoxInFrustum(m_box->m_minCorner, m_box->m_maxCorner, frustum);
	//读取当前文件中点
	QFileInfo info(m_fileFullname);
	QString suffix = info.suffix();

	if (m_level == drawLevel && !m_isLoaded && isBoxInFrustum)
	{
		if (suffix.toUpper() == "TXT")
		{
			QFile file(m_fileFullname);
			if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
			{
				return;
			}

			char currentLine[500];
			DCVector3D currentPoint;

			while(file.readLine(currentLine, 500) > 0)
			{
				QStringList list = QString(currentLine).split(",",QString::SkipEmptyParts);

				if (list.size() < 3)
				{
					return;
				}

				currentPoint = DCVector3D(list[0].toDouble(), list[1].toDouble(), list[2].toDouble());
				m_points.push_back(currentPoint);
				RGBColor color = {255,0,0};
				m_rgbColors.push_back(color);
			}
		}
		else if (suffix.toUpper() == "LAS")
		{
			//打开文件
			std::ifstream ifs;
			ifs.open(m_fileFullname.toStdWString(), std::ios::in | std::ios::binary);

			if (ifs.fail())
			{
				return;
			}

			liblas::Reader* reader = 0;
			unsigned nbOfPoints = 0;
			std::vector<std::string> dimensions;

			try
			{
				reader = new liblas::Reader(liblas::ReaderFactory().CreateWithStream(ifs));
				//处理压缩与非压缩文件
				liblas::Header const& header = reader->GetHeader();

				//获取字段
				dimensions = header.GetSchema().GetDimensionNames();
				//获取点个数
				nbOfPoints = header.GetPointRecordsCount();
			}
			catch (...)
			{
				delete reader;
				ifs.close();
				return;
			}

			if (nbOfPoints == 0)
			{
				delete reader;
				ifs.close();
				return;
			}

			DCVector3D point;
			unsigned int count = 0;
			unsigned int total = reader->GetHeader().GetPointRecordsCount();
			while (true)
			{
				if (!(count++ < total))
				{
					break;
				}
				reader->ReadNextPoint();
				const liblas::Point& p = reader->GetPoint();
				point[0] = p.GetX();
				point[1] = p.GetY();
				point[2] = p.GetZ();
				//如果是读入第一个数据，判断是否需要
				
				m_points.push_back(point);
				RGBColor color = {255,0,0};
				m_rgbColors.push_back(color);
			}

			ifs.close();
		}
		
		//加载实体数据,根据传入的文件名创建实体对象并绘制

		//更新节点加载状态
		SetNodeLoadState(true);
	}
	else if ((m_isLoaded && m_level != drawLevel) || (m_isLoaded && !isBoxInFrustum))  //卸载数据
	{
		//卸载实体数据
		m_points.clear();
		m_rgbColors.clear();
		std::vector<DCVector3D>(m_points).swap(m_points);
		std::vector<PointColor>(m_rgbColors).swap(m_rgbColors);
		SetNodeLoadState(false);
	}

	//几何体渲染
	m_geometry->Draw();

	//遍历子节点
	//if (isBoxInFrustum)
	{
		for (auto it = m_children.begin(); it != m_children.end(); ++it)
		{
			(*it)->Draw(drawLevel, frustum, depth);
		}
	}
	
}

void Quadnode::SetNodeLoadState(bool state)
{
	m_isLoaded = state;
}

void Quadnode::SetGeometry(Geometry* geometry)
{
	m_geometry = geometry;
}

std::vector<PointColor>& Quadnode::GetRGBColors()
{
	return m_rgbColors;
}

std::vector<DCVector3D>& Quadnode::GetPoints()
{
	return m_points;
}

unsigned Quadnode::GetPointsSize()
{
	return m_points.size();
}

bool Quadnode::PointInFrustum(float x, float y, float z, std::array<std::array<float, 4> ,6> frustum)
{
	// Go through all the sides of the frustum
	for(int i = 0; i < 6; i++ )
	{
		// Calculate the plane equation and check if the point is behind a side of the frustum
		if(frustum[i][0] * x + frustum[i][1] * y + frustum[i][2] * z + frustum[i][3] <= 0)
		{
			// The point was behind a side, so it ISN'T in the frustum
			return false;
		}
	}

	// The point was inside of the frustum (In front of ALL the sides of the frustum)
	return true;
}

bool Quadnode::CubeInFrustum(float x, float y, float z, float size, std::array<std::array<float, 4> ,6> frustum)
{
	// Basically, what is going on is, that we are given the center of the cube,
	// and half the length.  Think of it like a radius.  Then we checking each point
	// in the cube and seeing if it is inside the frustum.  If a point is found in front
	// of a side, then we skip to the next side.  If we get to a plane that does NOT have
	// a point in front of it, then it will return false.

	// *Note* - This will sometimes say that a cube is inside the frustum when it isn't.
	// This happens when all the corners of the bounding box are not behind any one plane.
	// This is rare and shouldn't effect the overall rendering speed.

	for(int i = 0; i < 6; i++ )
	{
		if(frustum[i][0] * (x - size) + frustum[i][1] * (y - size) + frustum[i][2] * (z - size) + frustum[i][3] > 0)
			continue;
		if(frustum[i][0] * (x + size) + frustum[i][1] * (y - size) + frustum[i][2] * (z - size) + frustum[i][3] > 0)
			continue;
		if(frustum[i][0] * (x - size) + frustum[i][1] * (y + size) + frustum[i][2] * (z - size) + frustum[i][3] > 0)
			continue;
		if(frustum[i][0] * (x + size) + frustum[i][1] * (y + size) + frustum[i][2] * (z - size) + frustum[i][3] > 0)
			continue;
		if(frustum[i][0] * (x - size) + frustum[i][1] * (y - size) + frustum[i][2] * (z + size) + frustum[i][3] > 0)
			continue;
		if(frustum[i][0] * (x + size) + frustum[i][1] * (y - size) + frustum[i][2] * (z + size) + frustum[i][3] > 0)
			continue;
		if(frustum[i][0] * (x - size) + frustum[i][1] * (y + size) + frustum[i][2] * (z + size) + frustum[i][3] > 0)
			continue;
		if(frustum[i][0] * (x + size) + frustum[i][1] * (y + size) + frustum[i][2] * (z + size) + frustum[i][3] > 0)
			continue;

		// If we get here, it isn't in the frustum
		return false;
	}

	return true;
}

bool Quadnode::BoxInFrustum(const Point3D& minCorner, const Point3D& maxCorner, std::array<std::array<float, 4> ,6> frustum)
{
	for(int i = 0; i < 6; i++ )
	{
		if(frustum[i][0] * minCorner.x + frustum[i][1] * minCorner.y + frustum[i][2] * minCorner.z + frustum[i][3] > 0)
			continue;
		if(frustum[i][0] * maxCorner.x + frustum[i][1] * minCorner.y + frustum[i][2] * minCorner.z + frustum[i][3] > 0)
			continue;
		if(frustum[i][0] * minCorner.x + frustum[i][1] * maxCorner.y + frustum[i][2] * minCorner.z + frustum[i][3] > 0)
			continue;
		if(frustum[i][0] * maxCorner.x + frustum[i][1] * maxCorner.y + frustum[i][2] * minCorner.z + frustum[i][3] > 0)
			continue;
		if(frustum[i][0] * minCorner.x + frustum[i][1] * minCorner.y + frustum[i][2] * maxCorner.z + frustum[i][3] > 0)
			continue;
		if(frustum[i][0] * maxCorner.x + frustum[i][1] * minCorner.y + frustum[i][2] * maxCorner.z + frustum[i][3] > 0)
			continue;
		if(frustum[i][0] * minCorner.x + frustum[i][1] * maxCorner.y + frustum[i][2] * maxCorner.z + frustum[i][3] > 0)
			continue;
		if(frustum[i][0] * maxCorner.x + frustum[i][1] * maxCorner.y + frustum[i][2] * maxCorner.z + frustum[i][3] > 0)
			continue;

		// If we get here, it isn't in the frustum
		return false;
	}

	return true;
}