#include "UpdateVisitor.h"

#include "QThreadPool"

//dcgp
#include "DCGp/Custom/GpPointCloud.h"
#include "DCGp/Custom/PageLodEntity.h"

#include "DCUtil/PageLodTaskThread.h"

using namespace DCUtil;

UpdateVisitor::UpdateVisitor(TraversalMode tm)
	: DCUtil::AbstractEntityVisitor(tm)
	, m_lodScale(1.0)
{
	m_pEntityQueueManager = new EntityQueueManager();

	m_databaseRequestHandler = new DCUtil::DatabasePage();
}

UpdateVisitor::~UpdateVisitor()
{

}


void UpdateVisitor::Apply(DcGp::DcGpEntity& node)
{
	Traverse(node);
}

void UpdateVisitor::Apply(DcGp::DcGpPointCloud& node)
{
	Apply(static_cast<DcGp::DcGpEntity& >(node));
}

void UpdateVisitor::Apply(DcGp::PageLodEntity& node)
{
	//! 存在分页节点，则根据当前视点与当前根节点边界盒的距离，赛选读取数据
	//! 获取分页节点的四叉树文件名，传入队列管理器，使用线程加载数据
	//! 在线程中读取并将结果添加到根节点中
	//PageLodTaskThread* ioThread = new PageLodTaskThread(m_pEntityQueueManager, &node, 1000);
	//QThreadPool::globalInstance()->start(ioThread);

	//! 获取队列中的实体，删除之前的子节点，添加现在队列到page子节点中
	//while (!node.GetChildren().isEmpty())
	//{
		//node.RemoveChild(0);
	//}

	//std::list<DcGp::DcGpEntity* > en;
	//m_pEntityQueueManager->GetEntityQueue(en);
	//for (std::list<DcGp::DcGpEntity* >::iterator it = en.begin(); it != en.end(); ++it)
	//{
	//	node.AddChild(*it);
	//}
	Traverse(node);

	//Apply(static_cast<DcGp::DcGpEntity& >(node));
}

float UpdateVisitor::GetDistanceToViewPoint(const DCVector3D& pos, bool withLODScale) const
{
	if (withLODScale) return (pos - m_viewPointLocal).Length() * m_lodScale;
	else return (pos - m_viewPointLocal).Length();
}

void UpdateVisitor::SetViewPointLocal(DCVector3D vPoint)
{
	m_viewPointLocal = vPoint;
}