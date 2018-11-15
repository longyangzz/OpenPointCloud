#include "PageLodEntity.h"

#include "DCGp/GpDrawContext.h"

#include "DCGp/Custom/GpBoundBox.h"

#include "DCUtil/UpdateVisitor.h"

#include "DCUtil/UpdateVisitor.h"
#include "DCUtil/DatabasePage.h"

using namespace DcGp;

PerRangeData::PerRangeData() :
	_priorityOffset(0.0f),
	_priorityScale(1.0f),
	_timeStamp(0.0f),
	_frameNumber(0)
{
	m_databaseRequest = nullptr;
}

PageLodEntity::PageLodEntity(QString name)
	: DcGp::DcGpEntity(name)
{
	m_topLevelEntity = new DcGp::DcGpPointCloud(name);

	//! 模拟四叉树结构，放四个文件名
	//QString fl1 = "D:\\data\\changshu-pdb\\changshu_root\\changshu_L1_X0_Y0.pdb";
	//QString fl2 = "D:\\data\\changshu-pdb\\changshu_root\\changshu_L1_X0_Y1.pdb";
	//QString fl3 = "D:\\data\\changshu-pdb\\changshu_root\\changshu_L1_X1_Y0.pdb";
	//QString fl4 = "D:\\data\\changshu-pdb\\changshu_root\\changshu_L1_X1_Y1.pdb";

	//m_pageQuadtree.push_back(fl1);
	//m_pageQuadtree.push_back(fl2);
	//m_pageQuadtree.push_back(fl3);
	//m_pageQuadtree.push_back(fl4);
}

PageLodEntity::~PageLodEntity()
{
}

void PageLodEntity::Accept(DCUtil::AbstractEntityVisitor& nv)
{
	nv.Apply(*this);
}

//std::vector<QString > PageLodEntity::GetPageQuadtree()
//{
//	return m_pageQuadtree;
//}

void PageLodEntity::SetFilename(unsigned int childNo, const QString& filename)
{
	if (childNo >= m_perRangeDataList.size()) m_perRangeDataList.resize(childNo + 1);
	m_perRangeDataList[childNo]._filename = filename;
	 
}

void PageLodEntity::Traverse(DCUtil::AbstractEntityVisitor& nv)
{
	QString fileName = (this->GetDatabasePath() + this->GetPerRangeDataList()[0]._filename);

	//! 边界盒与视点的距离
	double required_range = nv.GetDistanceToViewPoint(m_pageBoxCenter, true);

	bool needToLoadChild = false;
	for (unsigned int i = 0; i < m_perRangeDataList.size(); ++i)
	{
		if (m_range.first <= required_range && required_range < m_range.second)
		{
			if (!GetChildren().empty())
			{

				GetChild(i)->Accept(nv);
			}
			else
			{
				needToLoadChild = true;
			}
		}
	}

	//! 子节点个数
	int num = GetChildrenNumber();


	if (needToLoadChild)
	{
		//! 需要加载数据
		//DCUtil::DatabaseRequest* dbRequest = new DCUtil::DatabaseRequest(fileName, this);
		if (!m_perRangeDataList.empty())
		{
			/*static int i = 0;
			if (i == 0)*/
			{
				nv.GetDatabaseRequestHandler()->RequestNodeFile(fileName, this, m_perRangeDataList[0].m_databaseRequest);
			}
			//++i;
		}
		
	}

	//! 子节点个数
	//int num = GetChildrenNumber();


	//if (!num)
	//{
	//	//! 需要加载数据
	//	//DCUtil::DatabaseRequest* dbRequest = new DCUtil::DatabaseRequest(fileName, this);
	//	if (!m_perRangeDataList.empty())
	//	{
	//		nv.GetDatabaseRequestHandler()->RequestNodeFile(fileName, this, m_perRangeDataList[0].m_databaseRequest);
	//	}

	//}
	//else
	//{
	//	//!数据已经加载过了，往下遍历
	//	DcGp::DcGpEntity* group = GetChild(0);
	//	group->Accept(nv);
	//}

}

void PageLodEntity::FastDrawMyselfOnly(DcGpDrawContext& context)
{
	m_topLevelEntity->FastDrawMyselfOnly(context);
}

void PageLodEntity::DrawMyselfOnly(DcGp::DcGpDrawContext& context)
{
	if (!m_topLevelEntity)
	{
		return;
	}

	//如果数据隐藏，则返回上层数据
	if (!IsVisible())
	{
		return;
	}

	//判断是否是绘制三维物体
	if (MACRO_Draw3D(context))
	{
		m_topLevelEntity->DrawMyselfOnly(context);
	}
	

}

void PageLodEntity::SetTopLevelEntity(DcGp::DcGpPointCloud* topLevelEntity)
{
	m_topLevelEntity = topLevelEntity;
}

void PageLodEntity::AddPoint(DCVector3D vector)
{
	m_topLevelEntity->AddPoint(vector);
}

void PageLodEntity::AddColor(PointColor color)
{
	m_topLevelEntity->AddColor(color);
}

unsigned PageLodEntity::Size()
{
	return m_topLevelEntity->Size();
}

DcGp::DcGpBoundBox PageLodEntity::GetDisplayBoundBox()
{
	return m_topLevelEntity->GetDisplayBoundBox();
}

//
DcGp::DcGpBoundBox PageLodEntity::GetMyOwnBoundBox()
{
	return m_topLevelEntity->GetMyOwnBoundBox();
}