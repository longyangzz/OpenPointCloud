//----------------------------------------------------------------------
//author:wly
//time:
//dsc:用来封装鼠标交互事件处理
//
//----------------------------------------------------------------------

#ifndef DCVIEW_GUIEVENTHANDLE_H
#define DCVIEW_GUIEVENTHANDLE_H

#include <QObject>
#include "QtWidgets/QAction"

#include "DCCoreDLL.h"

class QMouseEvent;
class QKeyEvent;
class QWheelEvent;

namespace DCView
{
	class View;
}

namespace DCCore
{
	
	class DCCORE_EXPORT GUIEventHandle : public QObject
	{
		Q_OBJECT

	public:
		GUIEventHandle(QString name);
		~GUIEventHandle();

		//鼠标按下
		virtual void mousePressEvent(QMouseEvent* event, DCView::View* view) = 0;
		//鼠标移动
		virtual void mouseMoveEvent(QMouseEvent* event, DCView::View* view) = 0;
		//键盘按下事件
		virtual void keyPressEvent(QKeyEvent* event, DCView::View* view) = 0;

		//!鼠标松开
		virtual void mouseReleaseEvent(QMouseEvent* event, DCView::View* view);

		//转动滚轮
		virtual void wheelEvent(QWheelEvent* event, DCView::View* view);

		//! 鼠标双击
		virtual void mouseDoubleClickEvent(QMouseEvent* event, DCView::View* view);

		//! 场景更新
		virtual void updateEvent(DCView::View* view);
	
		virtual void SetValid(bool isValid, DCView::View* view = 0);

		void SetName(QString name);
		QString GetName();

		bool GetValid();

		//设定绑定的动作
		void SetAction(QAction* boundAction);

		//获取绑定的动作
		QAction* GetAction();
	private:
		//! 可用状态
		bool m_isValid;

		//! 操作器名字，unique
		QString m_uniqueName;

		//! 绑定的动作名称
		QAction* m_BoundAction;
	};
}

#endif // DCVIEW_GUIEVENTHANDLE_H
