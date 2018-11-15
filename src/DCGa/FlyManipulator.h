//----------------------------------------------------------------------
//author:wly
//time:
//dsc:驾驶模式漫游器
//
//----------------------------------------------------------------------


#ifndef DCGA_FLYMANIPULATOR_H
#define DCGA_FLYMANIPULATOR_H

//Qt
#include "QPoint"
#include "QTime"

#include "dcga_global.h"

#include "DCCore/GUIEventHandle.h"

#include "DCCore/BasicTypes.h"

#include "DCView/Common.h"

namespace DCView
{
	class View;
	class Camera;
}

namespace DcGp
{
	class DcGpEntity;
}

namespace DCGa
{
	class DCGA_EXPORT FlyManipulator : public DCCore::GUIEventHandle
	{
		Q_OBJECT
	public:

		FlyManipulator(QString name);
		~FlyManipulator();

		//鼠标按下
		virtual void mousePressEvent(QMouseEvent* event, DCView::View* view);
		//鼠标移动
		virtual void mouseMoveEvent(QMouseEvent* event, DCView::View* view);
		//键盘按下事件
		virtual void keyPressEvent(QKeyEvent* event, DCView::View* view);

		void mouseReleaseEvent(QMouseEvent* event, DCView::View* view);

		//转动滚轮
		virtual void wheelEvent(QWheelEvent* event, DCView::View* view);

		virtual void updateEvent(DCView::View* view);

		virtual void SetValid(bool isValid, DCView::View* view);

	protected:
		void setPosition(DCVector3D position) { mPosition = position; }

		const DCVector3D& position() const { return mPosition; }

		void setXDegrees(float degree) { mXDegrees = degree; }

		float xDegrees() { return mXDegrees; }

		void setYDegrees(float degree) { mYDegrees = degree; }

		float yDegrees() { return mYDegrees; }

	private:

		/** Key bindings to move forward (default = Key_W). */
		void setKeysForward(Qt::Key key, Qt::Key modifier = Qt::Key(0))  { mKeysForward[0] = key; mKeysForward[1] = modifier; };

		/** Key bindings to move backward (default = Key_S). */
		void setKeysBackward(Qt::Key key, Qt::Key modifier = Qt::Key(0)) { mKeysBackward[0] = key; mKeysBackward[1] = modifier; };

		/** Key bindings to move left (default = Key_A). */
		void setKeysLeft(Qt::Key key, Qt::Key modifier = Qt::Key(0))     { mKeysLeft[0] = key; mKeysLeft[1] = modifier; };

		/** Key bindings to move right (default = Key_D). */
		void setKeysRight(Qt::Key key, Qt::Key modifier = Qt::Key(0))    { mKeysRight[0] = key; mKeysRight[1] = modifier; };

		/** Key bindings to move up (default = Key_W + Key_Shift). */
		void setKeysUp(Qt::Key key, Qt::Key modifier = Qt::Key(0))       { mKeysUp[0] = key; mKeysUp[1] = modifier; };

		/** Key bindings to move down (default = Key_S + Key_Shift). */
		void setKeysDown(Qt::Key key, Qt::Key modifier = Qt::Key(0))     { mKeysDown[0] = key; mKeysDown[1] = modifier; };

		/** The camera rotation speed (default = 0.5). */
		void setRotationSpeed(float speed) { mRotationSpeed = speed; }

		/** The camera rotation speed (default = 0.5). */
		float rotationSpeed() const { return mRotationSpeed; }

		/** The camera translation speed (default = 50). */
		void setMovementSpeed(float speed) { mMovementSpeed = speed; }

		/** The camera translation speed (default = 50). */
		float movementSpeed() const { return mMovementSpeed; }

		DCVector3D CalcDreation(DCView::View* view);

	private:
		DCView::Camera* mCamera;
		DCVector3D mPosition;
		float mLastTime;
		float mRotationSpeed;
		float mMovementSpeed;
		float mXDegrees;
		float mYDegrees;
		Qt::Key mKeysForward[2];
		Qt::Key mKeysBackward[2];
		Qt::Key mKeysUp[2];
		Qt::Key mKeysDown[2];
		Qt::Key mKeysLeft[2];
		Qt::Key mKeysRight[2];
		QTime mTime;

		//! 鼠标上次朝向
		DCCore::Vector_3 m_lastOrientationOfMouse;
		//鼠标当前朝向
		DCCore::Vector_3 m_currentOrientationOfMouse;

		DCView::Matrix_4_4 m_matrixRotate;
	};
}

#endif // DCGA_FLYMANIPULATOR_H
