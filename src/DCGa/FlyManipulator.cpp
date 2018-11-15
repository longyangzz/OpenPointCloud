#include "FlyManipulator.h"

//qt
#include "QtWidgets/QApplication"
#include "QtGui/QMouseEvent"
#include "QtGui/QKeyEvent"
#include "QTime"

//DCCore
#include "DCCore/Time.h"

//DCgp
#include "DCGp/GpEntity.h"
#include "DCGp/Custom/GpBoundBox.h"

#include "DCCore/Const.h"
#include "DCCore/Logger.h"

#include "DCCore/Vector4D.h"

#include "DCCore/Matrix2.h"
#include "DCCore/Matrix3.h"
#include "DCCore/Matrix4.h"

//DCView
#include "DCView/View.h"
#include "DCView/GLToolkit.h"
#include "DCView/Parameters.h"
#include "DCView/Camera.h"

using namespace DCGa;

FlyManipulator::FlyManipulator(QString name)
	: DCCore::GUIEventHandle(name)
{
	SetValid(false, 0);

	mRotationSpeed = 0.5;
	mMovementSpeed = 100.0f;
	mXDegrees = 0;
	mYDegrees = 0;
	mLastTime = 0;
	mPosition = DCVector3D(0,0,0);

	m_matrixRotate = DCView::Matrix_4_4::Identity(4,4);

	setKeysForward(Qt::Key_W);
	setKeysBackward(Qt::Key_S);
	setKeysLeft(Qt::Key_A);
	setKeysRight(Qt::Key_D);
	setKeysUp(Qt::Key_W, Qt::Key_Shift);
	setKeysDown(Qt::Key_S, Qt::Key_Shift);
}

FlyManipulator::~FlyManipulator()
{

}

void FlyManipulator::SetValid(bool isValid, DCView::View* view)
{
	DCCore::GUIEventHandle::SetValid(isValid);

	if (!view)
	{
		return;
	}

	if (view->GetMatrixPiple() == DCView::eCamera)
	{
		setPosition( view->GetCamera()->modelingMatrix().getT() );
		float x, y;
		view->GetCamera()->modelingMatrix().getYXRotationAngles( y, x );
		setXDegrees(x);
		setYDegrees(y);
	}
	else 
	{
		setPosition(view->GetViewportParameters().cameraPos);
		DCView::Matrix_4_4 mr = view->GetViewportParameters().matrixRotated;
		DCCore::mat4 rt;
		for (int i = 0; i != 16; ++i)
		{
			rt.ptr()[i] = mr.data()[i];
		}
		float x, y;
		rt.getYXRotationAngles(y,x);
		setXDegrees(x);
		setYDegrees(y);

		//获取单位矩阵并初始化保存的旋转矩阵
		DCCore::mat4 mr2 = DCCore::mat4::getRotation( 0, DCVector3D(0,1,0), 0, DCVector3D(1,0,0) );
		for (int i = 0; i != 16; ++i)
		{
			m_matrixRotate.data()[i] = mr2.ptr()[i];
		}
	}

	view->SetContinuousUpdate(true);
}

void FlyManipulator::mousePressEvent(QMouseEvent* event, DCView::View* view)
{
	if (view->GetMatrixPiple() == DCView::eOpenGL)
	{
		{
			//获取组合键
			DCView::GlobalParameters::CompositeKey key = 
				DCView::GlobalParameters::GetCompositeKey(DCView::GlobalParameters::eRotateOperation);

			if ((event->buttons() & key.buttons) 
				&& ((QApplication::keyboardModifiers() & key.keys) 
				|| (QApplication::keyboardModifiers() == key.keys)))
			{
				//记录鼠标的当前方向
				m_lastOrientationOfMouse = view->MouseOrientation(event->x(), event->y());
			}
		}
	}
}

void FlyManipulator::mouseMoveEvent(QMouseEvent* event, DCView::View* view)
{
	//如果鼠标移出窗体，则不进行任何操作
	if (event->x() <= 1 
		|| event->y() <= 1 
		|| event->x() >= view->width() - 1 
		|| event->y() >= view->height() - 1)
	{
		return;
	}
	if (view->GetMatrixPiple() == DCView::eOpenGL)
	{
		//旋转
		{
			//获取组合键
			DCView::GlobalParameters::CompositeKey key = 
				DCView::GlobalParameters::GetCompositeKey(DCView::GlobalParameters::eRotateOperation);

			if ((event->buttons() & key.buttons) 
				&& ((QApplication::keyboardModifiers() & key.keys) 
				|| (QApplication::keyboardModifiers() == key.keys)))
			{
				//记录当前鼠标方向
				m_currentOrientationOfMouse = view->MouseOrientation(event->x(), event->y());

				//记录旋转矩阵
				m_matrixRotate = (m_matrixRotate * DCView::GLToolkit::CalculateRotationMatrix(m_lastOrientationOfMouse,
					m_currentOrientationOfMouse)).matrix();

				//更新上一次的鼠标方向
				m_lastOrientationOfMouse = m_currentOrientationOfMouse;
			}
		}
	}
}

void FlyManipulator::mouseReleaseEvent(QMouseEvent* event, DCView::View* view)
{
	
}

void FlyManipulator::keyPressEvent(QKeyEvent* event, DCView::View* view)
{
	
}

void FlyManipulator::wheelEvent(QWheelEvent* event, DCView::View* view)
{
	//! 更新速度之
	int numDegrees = event->delta() / 8;
	int numSteps = numDegrees / 15;

	mMovementSpeed += numSteps;
}

void FlyManipulator::updateEvent(DCView::View* view)
{
	if (view->GetCamera() == NULL)
		return;

	if (mLastTime == 0)
	{
		mLastTime = DCCore::Time::currentTime();
		return;
	}
	DCCore::real dt = DCCore::Time::currentTime() - mLastTime;
	mLastTime = DCCore::Time::currentTime();


	if (view->GetMatrixPiple() == DCView::eCamera)
	{
		DCCore::mat4 m = DCCore::mat4::getTranslation(mPosition);
		m *= DCCore::mat4::getRotation( mYDegrees, DCVector3D(0,1,0), mXDegrees, DCVector3D(1,0,0) );
		view->GetCamera()->setModelingMatrix(m);

		view->DisableModelView();

		DCVector3D direction = CalcDreation(view);

		DCVector3D dir;
		dir += view->GetCamera()->modelingMatrix().getX() * direction.x;
		dir += view->GetCamera()->modelingMatrix().getY() * direction.y;
		dir -= view->GetCamera()->modelingMatrix().getZ() * direction.z;
		dir.normalize();
		mPosition += dir * (float)(dt * view->ComputeActualPixelSize() * mMovementSpeed);
	}
	else
	{
		DCCore::mat4 mr = DCCore::mat4::getRotation( mYDegrees, DCVector3D(0,1,0), mXDegrees, DCVector3D(1,0,0) );
		DCView::Matrix_4_4 matrixRotate;
		for (int i = 0; i != 16; ++i)
		{
			matrixRotate.data()[i] = mr.ptr()[i];
		}

		matrixRotate = (m_matrixRotate * matrixRotate).matrix();

		//！ 更新位置		
		view->UpdateCameraPos(mPosition);
		//!rotate更新旋转矩阵
		view->UpdateRotateView(matrixRotate);
		view->DisableModelView();

		DCVector3D direction = CalcDreation(view);

		mPosition += direction * (float)(dt * view->ComputeActualPixelSize() * mMovementSpeed);
	}

}

DCVector3D FlyManipulator::CalcDreation(DCView::View* view)
{
	DCVector3D direction;
	bool okmodifier;
	bool modifier = view->IsKeyPressed( Qt::Key_Alt ) || view->IsKeyPressed( Qt::Key_Control ) || view->IsKeyPressed( Qt::Key_Shift );

	okmodifier = (mKeysLeft[1] == Qt::Key(0)) ? !modifier : view->IsKeyPressed( mKeysLeft[1] );
	if ( view->IsKeyPressed(mKeysLeft[0]) && okmodifier )
		direction.x = -1;

	okmodifier = (mKeysRight[1] == Qt::Key(0)) ? !modifier : view->IsKeyPressed(mKeysRight[1]);
	if ( view->IsKeyPressed(mKeysRight[0]) && okmodifier )
		direction.x = +1;

	okmodifier = (mKeysBackward[1] == Qt::Key(0)) ? !modifier : view->IsKeyPressed(mKeysBackward[1]);
	if ( view->IsKeyPressed(mKeysBackward[0]) && okmodifier )
		direction.z = -1;

	okmodifier = (mKeysForward[1] == Qt::Key(0)) ? !modifier : view->IsKeyPressed(mKeysForward[1]);
	if ( view->IsKeyPressed(mKeysForward[0]) && okmodifier )
		direction.z = +1;

	okmodifier = (mKeysUp[1] == Qt::Key(0)) ? !modifier : view->IsKeyPressed(mKeysUp[1]);
	if ( view->IsKeyPressed(mKeysUp[0]) && okmodifier )
		direction.y = +1;

	okmodifier = (mKeysDown[1] == Qt::Key(0)) ? !modifier : view->IsKeyPressed(mKeysDown[1]);
	if ( view->IsKeyPressed(mKeysDown[0]) && okmodifier )
		direction.y = -1;

	return direction;
}