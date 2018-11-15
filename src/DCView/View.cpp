#include "DCView/View.h"

//DCore
#include "DCCore/Logger.h"

//Impl
#include "Impl/Impl_Viewer.hxx"

using namespace DCView;

//当前视窗
View* View::CurrentViewer()
{
	return ViewerImpl::CurrentViewer();
}

void View::NullCurrentViewer()
{
	ViewerImpl::NullCurrentViewer();
}

//构造函数
View::View(QWidget* pParent /* = nullptr */, 
	const QGLFormat& format /* =QGLFormat::defaultFormat() */, 
	QGLWidget* pSharedWidget /* = nullptr */)
	: QGLWidget(format, pParent, pSharedWidget)
{
	m_pImpl = new ViewerImpl(this);
}

//析构函数
View::~View()
{
	delete m_pImpl;
	m_pImpl = nullptr;
}

//设为当前
void View::SetCurrent()
{
	m_pImpl->SetCurrent();
}

//获取投影模式
ProjectionMode View::GetProjection() const
{
	return m_pImpl->GetProjection();
}
//设置投影模式
void View::SetProjection(ProjectionMode mode)
{
	m_pImpl->SetProjection(mode);
}

//初始化OpenGL环境
void View::initializeGL()
{
	m_pImpl->InitializeGL();
}

//大小变化
void View::resizeGL(int iWidth, int iHeight)
{
	m_pImpl->ResizeGL(iWidth, iHeight);
}

//OpenGL绘制
void View::Render()
{
	//DCCore::Logger::MessageDebug("View::paintGL()");
	m_pImpl->Render();
}

void View::SetSunLightState(bool state)
{
	m_pImpl->SetSunLightState(state);
}

bool View::GetSunLightState()
{
	return m_pImpl->GetSunLightState();
}

//鼠标点击
void View::mousePressEvent(QMouseEvent* event)
{
	m_pImpl->PressedMouse(event);
}

//鼠标松开
void View::mouseReleaseEvent(QMouseEvent* event)
{
	m_pImpl->ReleasedMouse(event);
}

//鼠标移动
void View::mouseMoveEvent(QMouseEvent* event)
{
	m_pImpl->MovingMouse(event);
}

//转动滚轮
void View::wheelEvent(QWheelEvent* event)
{
	m_pImpl->Wheeling(event);
}

void View::keyPressEvent(QKeyEvent* event)
{
	m_pImpl->KeyPress(event);
}

void View::keyReleaseEvent(QKeyEvent * event)
{
	m_pImpl->KeyRelease(event);
}

bool View::IsKeyPressed(Qt::Key key)
{
	return m_pImpl->IsKeyPressed(key);
}

//更新旋转中心
void View::UpdatePivot(const Point_3& ptPivot)
{
	m_pImpl->UpdatePivot(ptPivot);
}

//重绘
void View::Refresh()
{
	m_pImpl->Refresh();
}

void View::Get3DRayUnderMouse(const Point_2& point2D, Point_3* v1, Point_3* v2)
{
	m_pImpl->Get3DRayUnderMouse(point2D, v1, v2);
}

//屏幕坐标到世界坐标
bool View::ScreenToWorld(const Point_2& point2D,
	Point_3& point3D)
{
	return m_pImpl->ScreenToWorld(point2D, point3D);
}
//世界坐标到屏幕坐标
bool View::WorldToScreen(const Point_3& point3D,
	Point_2& point2D)
{
	return m_pImpl->WorldToScreen(point3D, point2D);
}

//设置视图
void View::SetView(ViewMode mode)
{
	m_pImpl->SetView(mode);
}

//重置视图
void View::ResetView()
{
	m_pImpl->ResetView();
}

//设置GLFilter
void View::SetGLFilter(GLFilterPtr pFilter)
{
	m_pImpl->SetGLFilter(pFilter);
}

//获取GLFilter
GLFilterPtr View::GetGLFilter() const
{
	return m_pImpl->GetGLFilter();
}

//启用正投影
void View::EnableOrthoProjection()
{
	m_pImpl->EnableOrthoProjection();
}

//更新中心并移动相机
void View::UpdateCenterAndZoom(const BoundBox& box)
{
	m_pImpl->UpdateCenterAndZoom(box);
}

//计算屏幕实际像素大小
float View::ComputeActualPixelSize()
{
	return m_pImpl->ComputeActualPixelSize();
}

std::array<std::array<float, 4> ,6> View::GetFrustum()
{
	return m_pImpl->GetFrustum();
}

const ViewerParams& View::GetViewportParameters() const
{
	return m_pImpl->GetViewportParameters();
}

void View::SetApplyLODState(bool state)
{
	m_pImpl->SetApplyLODState(state);
}


void View::SetKeyDescription(int key, QString description)
{
	m_pImpl->SetKeyDescription(key, description);
}

void View::Help()
{
	m_pImpl->Help();
}

QString View::HelpString() const
{
	return m_pImpl->HelpString();
}

QString View::MouseString() const
{
	return m_pImpl->MouseString();
}

QString View::KeyboardString() const
{
	return m_pImpl->KeyboardString();
}

void View::SetHudtext(QString text)
{
	m_pImpl->SetHudtext(text);
}

DcGp::DcGpEntity* View::GetInnerRoot()
{
	return m_pImpl->GetInnerRoot();
}

void View::AddtoInnerRoot(DcGp::DcGpEntity* obj2D, bool noDependency /* = true */)
{
	m_pImpl->AddtoInnerRoot(obj2D, noDependency);
}

void View::RemoveFormInnerRoot(DcGp::DcGpEntity* obj2D)
{
	m_pImpl->RemoveFormInnerRoot(obj2D);
}

void View::DisplayMessage(const QString& message, int delay/*=2000*/)
{
	m_pImpl->DisplayMessage(message, delay);
}

void View::DrawText(int x, int y, const QString& text, const QFont& fnt/*=QFont()*/)
{
	m_pImpl->DrawText(x, y, text, fnt);
}

//计算鼠标方向
Vector_3 View::MouseOrientation(unsigned iX, unsigned iY)
{
	return m_pImpl->MouseOrientation(iX, iY);
}

void View::MoveCamera(const Vector_3& v)
{
	m_pImpl->MoveCamera(v);
}

void View::AddManipulatorHandle(DCCore::GUIEventHandle* manipulator)
{
	m_pImpl->AddManipulatorHandle(manipulator);
}

void View::RotateView(const Matrix_4_4& matrix)
{
	m_pImpl->RotateView(matrix);
}

void View::EnablePivot(bool bState)
{
	m_pImpl->EnablePivot(bState);
}

void View::RotatingWheel(float fDegree)
{
	m_pImpl->RotatingWheel(fDegree);
}

void View::Scale(float fScale)
{
	m_pImpl->Scale(fScale);
}

void View::SetInterActionMode(InterActionMode iaMode)
{
	m_pImpl->SetInterActionMode(iaMode);
}

Camera* View::GetCamera() const
{
	return m_pImpl->GetCamera();
}

MatrixPiple View::GetMatrixPiple() const
{
	return m_pImpl->GetMatrixPiple();
}

void View::SetMatrixPiple(MatrixPiple mpType)
{
	m_pImpl->SetMatrixPiple(mpType);
}

void View::DisableModelView()
{
	m_pImpl->DisableModelView();
}

void View::DisableProjection()
{
	m_pImpl->DisableProjection();
}

void View::UpdateCameraPos(const DCVector3D& pos)
{
	m_pImpl->UpdateCameraPos(pos);
}

void View::UpdateRotateView(const Matrix_4_4& rm)
{
	m_pImpl->UpdateRotateView(rm);
}

void View::SetRefreshRate( int msec )
{
	m_pImpl->SetRefreshRate(msec);
}

int View::GetRefreshRate()
{
	return m_pImpl->GetRefreshRate();
}

void View::SetContinuousUpdate(bool continuous)
{
	m_pImpl->SetContinuousUpdate(continuous);
}