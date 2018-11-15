#include "FlatManipulator.h"

//qt
#include "QtWidgets/QApplication"
#include "QtGui/QMouseEvent"
#include "QtGui/QKeyEvent"

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

FlatManipulator::FlatManipulator(QString name)
	: DCCore::GUIEventHandle(name)
{
	SetValid(true);
}

FlatManipulator::~FlatManipulator()
{

}


void FlatManipulator::mousePressEvent(QMouseEvent* event, DCView::View* view)
{
	
}

void FlatManipulator::mouseMoveEvent(QMouseEvent* event, DCView::View* view)
{
	
	

	view->Refresh();
}

void FlatManipulator::mouseReleaseEvent(QMouseEvent* event, DCView::View* view)
{
	
}

void FlatManipulator::keyPressEvent(QKeyEvent* event, DCView::View* view)
{

}

void FlatManipulator::wheelEvent(QWheelEvent* event, DCView::View* view)
{
	
}