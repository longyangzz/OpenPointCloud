#include "DCView/ViewBase.h"

#include "DCUtil/UpdateVisitor.h"

ViewBase::ViewBase()
	: m_firstFrame(true)
	, m_updateVisitor(nullptr)
{
	m_updateVisitor = new DCUtil::UpdateVisitor;
}

ViewBase::~ViewBase()
{
	if (m_updateVisitor)
	{
		delete m_updateVisitor;
		m_updateVisitor = nullptr;
	}
}

void ViewBase::Frame()
{
	if (m_firstFrame)
	{
		m_firstFrame = false;
	}

	//！更新帧信息
	//Advance();

	//1事件获取及更新
	EventTraversal();

	//2更新遍历
	UpdateTraversal();

	//3渲染遍历
	RenderingTraversals();
	
}
