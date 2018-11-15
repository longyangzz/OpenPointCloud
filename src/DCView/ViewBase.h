#ifndef VIEWBASE_H
#define VIEWBASE_H

#include "ViewDLL.h"

namespace DCUtil
{
	class UpdateVisitor;
}

class DC_VIEW_API ViewBase
{
public:
	ViewBase();
	~ViewBase();

	void Frame();

	//!½Ó¿Ú
	//virtual void Advance() = 0;

	virtual void EventTraversal() = 0;

	virtual void UpdateTraversal() = 0;

	virtual void RenderingTraversals() = 0;
protected:
	bool m_firstFrame;

	DCUtil::UpdateVisitor*               m_updateVisitor;
};

#endif // VIEWBASE_H
