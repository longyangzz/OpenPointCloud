//----------------------------------------------------------------------
//author:wly
//time:
//dsc:跟踪球漫游器
//
//----------------------------------------------------------------------


#ifndef DCGA_TRACKBALLMANIPULATOR_H
#define DCGA_TRACKBALLMANIPULATOR_H

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
	class DCGA_EXPORT TrackballManipulator : public DCCore::GUIEventHandle
	{
		Q_OBJECT
	public:

		TrackballManipulator(QString name);
		~TrackballManipulator();

		//鼠标按下
		virtual void mousePressEvent(QMouseEvent* event, DCView::View* view);
		//鼠标移动
		virtual void mouseMoveEvent(QMouseEvent* event, DCView::View* view);
		//键盘按下事件
		virtual void keyPressEvent(QKeyEvent* event, DCView::View* view);

		void mouseReleaseEvent(QMouseEvent* event, DCView::View* view);

		//转动滚轮
		virtual void wheelEvent(QWheelEvent* event, DCView::View* view);
		virtual void SetValid(bool isValid, DCView::View* view);

	private:
		DCCore::mat4 trackballRotation(int x, int y, DCView::View* view);

		DCVector3D computeVector(int x, int y, DCView::View* view);

	private:
		//鼠标上一次的位置
		QPoint m_lastPosOfMouse;

		//! 鼠标上次朝向
		DCCore::Vector_3 m_lastOrientationOfMouse;
		//鼠标当前朝向
		DCCore::Vector_3 m_currentOrientationOfMouse;

		DCCore::mat4 m_StartMatrix;
		DCVector3D m_Pivot;
		DCVector3D m_StartCameraPos;
		DCVector3D m_StartPivot;
		float m_RotationSpeed;
		float m_TranslationSpeed;
		float m_ZoomSpeed;
	};
}

#endif // DCGA_TRACKBALLMANIPULATOR_H
