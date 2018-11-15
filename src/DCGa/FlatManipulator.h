//----------------------------------------------------------------------
//author:wly
//time:
//dsc:驾驶模式漫游器
//
//----------------------------------------------------------------------


#ifndef DCGA_FLATMANIPULATOR_H
#define DCGA_FLATMANIPULATOR_H

//Qt
#include "QPoint"

#include "dcga_global.h"

#include "DCCore/GUIEventHandle.h"

#include "DCCore/BasicTypes.h"

#include "DCView/Common.h"

namespace DCView
{
	class View;
}

namespace DcGp
{
	class DcGpEntity;
}

namespace DCGa
{
	class DCGA_EXPORT FlatManipulator : public DCCore::GUIEventHandle
	{
		Q_OBJECT
	public:

		FlatManipulator(QString name);
		~FlatManipulator();

		//鼠标按下
		virtual void mousePressEvent(QMouseEvent* event, DCView::View* view);
		//鼠标移动
		virtual void mouseMoveEvent(QMouseEvent* event, DCView::View* view);
		//键盘按下事件
		virtual void keyPressEvent(QKeyEvent* event, DCView::View* view);

		void mouseReleaseEvent(QMouseEvent* event, DCView::View* view);

		//转动滚轮
		virtual void wheelEvent(QWheelEvent* event, DCView::View* view);

	private:

	private:

	};
}

#endif // DCGA_TRACKBALLMANIPULATOR_H
