#include "ReferenceCloud.h"

DcGp::ReferenceCloud::ReferenceCloud(DcGpPointCloud* associatedCloud)
	: m_theIndexes(0)
	, m_theAssociatedCloud(associatedCloud)
{
	m_theIndexes = new ReferencesContainer();
}

DcGp::ReferenceCloud::~ReferenceCloud()
{

}

bool DcGp::ReferenceCloud::AddPointIndex(unsigned globalIndex)
{
	m_theIndexes->push_back(globalIndex);
	return true;
}

unsigned DcGp::ReferenceCloud::CurrentSize()
{
	return m_theIndexes->size();
}

void DcGp::ReferenceCloud::Clear()
{
	m_theIndexes->clear();
}

unsigned DcGp::ReferenceCloud::GetValue(unsigned index)
{
	return m_theIndexes->at(index);
}

void DcGp::ReferenceCloud::Resize(unsigned num)
{
	m_theIndexes->resize(num);
}

unsigned DcGp::ReferenceCloud::GetPointGlobalIndex(unsigned localIndex) const
{
	return m_theIndexes->at(localIndex); 
}

void DcGp::ReferenceCloud::SetPointIndex(unsigned localIndex, unsigned globalIndex)
{
	(*m_theIndexes)[localIndex] = globalIndex;
}

DCVector3D DcGp::ReferenceCloud::GetPoint(unsigned index)
{
	return m_theAssociatedCloud->GetPoint(GetValue(index));
}

void DcGp::ReferenceCloud::SetPointScalarValue(unsigned pointIndex, ScalarType value)
{
	//判断当前点云是否存在标量场
	DcGp::DcGpScalarFieldPtr sf = m_theAssociatedCloud->GetScalarFieldByIndex(0);

	//更新标量场的值
	sf->AddData(GetValue(pointIndex), value);
}

void DcGp::ReferenceCloud::RemovePointGlobalIndex(unsigned localIndex)
{
	//assert(localIndex < CurrentSize());

	//unsigned lastIndex = CurrentSize()-1;
	////swap the value to be removed with the last one
	//m_theIndexes[localIndex] = m_theIndexes[lastIndex];
	//m_theIndexes->resize(lastIndex);
}