#include "DCCore/GUIEventHandle.h"

using namespace DCCore;

GUIEventHandle::GUIEventHandle(QString name)
	: m_isValid(false)
	, m_BoundAction(nullptr)
	, m_uniqueName(name)
{

}

GUIEventHandle::~GUIEventHandle()
{

}

void GUIEventHandle::SetAction(QAction* boundAction)
{
	m_BoundAction = boundAction;
}

QAction* GUIEventHandle::GetAction()
{
	return m_BoundAction;
}

void GUIEventHandle::SetValid(bool isValid, DCView::View* view)
{
	m_isValid = isValid;
}

bool GUIEventHandle::GetValid()
{
	return m_isValid;
}

void GUIEventHandle::SetName(QString name)
{
	m_uniqueName = name;
}

QString GUIEventHandle::GetName()
{
	return m_uniqueName;
}

void GUIEventHandle::mouseReleaseEvent(QMouseEvent* event, DCView::View* view)
{

}

void GUIEventHandle::mouseDoubleClickEvent(QMouseEvent* event, DCView::View* view)
{

}

void GUIEventHandle::wheelEvent(QWheelEvent* event, DCView::View* view)
{

}

void GUIEventHandle::updateEvent(DCView::View* view)
{

}

