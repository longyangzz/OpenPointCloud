#include "EntityVisitor.h"

//dcgp
#include "DCGp/Custom/GpPointCloud.h"


using namespace MPUtil;

EntityVisitor::EntityVisitor(TraversalMode tm)
	: DCUtil::AbstractEntityVisitor(tm)
{
	
}

EntityVisitor::~EntityVisitor()
{

}

void EntityVisitor::Apply(DcGp::DcGpEntity& node)
{
	Traverse(node);
}

void EntityVisitor::Apply(DcGp::DcGpPointCloud& node)
{
	Apply(static_cast<DcGp::DcGpEntity& >(node));
}

//void EntityVisitor::Apply(MPGp::DistanceLine& node)
//{
//	Apply(static_cast<DcGp::DcGpEntity& >(node));
//}
//
//void EntityVisitor::Apply(MPGp::IndividualPlane& node)
//{
//	Apply(static_cast<DcGp::DcGpEntity& >(node));
//}
//
//void EntityVisitor::Apply(MPGp::Outlines& node)
//{
//	Apply(static_cast<DcGp::DcGpEntity&>(node));
//}
//
//void EntityVisitor::Apply(MPGp::IntersectionLine& node)
//{
//	Apply(static_cast<DcGp::DcGpEntity&>(node));
//}
//
//void EntityVisitor::Apply(MPGp::BoundaryLine& node)
//{
//	Apply(static_cast<DcGp::DcGpEntity&>(node));
//}
//
//void EntityVisitor::Apply(MPGp::Round& node)
//{
//	Apply(static_cast<DcGp::DcGpEntity&>(node));
//}
//
//void EntityVisitor::Apply(MPGp::OperationBox &node)
//{
//	Apply(static_cast<DcGp::DcGpEntity&>(node));
//}