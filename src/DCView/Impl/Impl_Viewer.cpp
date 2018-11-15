#include "Impl_Viewer.hxx"

#include "DCCore/DCGL.h"

//std
#include <cmath>

//Qt
#include "QtCore/QtGlobal"

#include "QtWidgets/QApplication"
#include "QtGui/QMouseEvent"
#include "QtGui/QWheelEvent"
#include "QtGui/QColor"
#include "QtGui/QLinearGradient"
#include "QtGui/QBrush"
#include "QDebug"
#include "QtWidgets/QTableWidget"
#include "QtWidgets/QTextEdit"
#include "QUrl"
#include "QtGui/QClipboard"
#include "QFileInfo"
#include "QtWidgets/QFileDialog"
#include "QtWidgets/QMessageBox"
#include "QTime"

//DCCore
#include "DCCore/Algorithm.h"
#include "DCCore/Logger.h"
#include "DCCore/Const.h"

//Dcgp
#include "DCGp/GpEntity.h"

//Self
#include "FrameBuffer.hxx"

#include "DCView/GLToolkit.h"
#include "DCView/Parameters.h"
#include "DCView/SymbolLibrary.h"
#include "DCView/GLFilter.h"
#include "DCView/EDLFilter.h"
#include "DCCore/GUIEventHandle.h"
#include "DCView/Camera.h"
#include "DCView/Viewport.h"
#include "DCCore/Transform.h"

using namespace DCView;

static QString DCLWViewerVersionString()
{
	return QString::number((DCLWVIEWER_VERSION & 0xff0000) >> 16) + "." +
		QString::number((DCLWVIEWER_VERSION & 0x00ff00) >> 8) + "." +
		QString::number(DCLWVIEWER_VERSION & 0x0000ff);
}

static QString TableLine(const QString& left, const QString& right)
{
	static bool even = false;
	const QString tdtd("</b></td><td>");
	const QString tdtr("</td></tr>\n");

	QString res("<tr bgcolor=\"");

	if (even)
		res += "#eeeeff\">";
	else
		res += "#ffffff\">";
	res += "<td><b>" + left + tdtd + right + tdtr;
	even = !even;

	return res;
}

static int ConvertToKeyboardModifiers(int state)
{
#if QT_VERSION < 0x040000
	// Qt 2 & 3 have different values for ButtonState and Modifiers.
	// Converts CTRL,SHIFT... to ControlButton, ShiftButton...
	if (state & Qt::MODIFIER_MASK)
	{
		if (state & Qt::CTRL)  { state &= ~Qt::CTRL;	state |= Qt::ControlButton; }
		if (state & Qt::SHIFT) { state &= ~Qt::SHIFT;	state |= Qt::ShiftButton; }
		if (state & Qt::ALT)   { state &= ~Qt::ALT; 	state |= Qt::AltButton; }
# if QT_VERSION >= 0x030100
		if (state & Qt::META)  { state &= ~Qt::META; 	state |= Qt::MetaButton; }
# endif
	}
#endif
	return state;
}

static Qt::KeyboardModifiers ConvertKeyboardModifiers(Qt::KeyboardModifiers modifiers)
{
#if QT_VERSION < 0x040000
	return QtKeyboardModifiers(convertToKeyboardModifiers(modifiers));
#else
	return modifiers;
#endif
}

static int ConvertToShortModifier(int state)
{
	// Converts ControlButton, ShiftButton... to CTRL,SHIFT...
	// convertToKeyboardModifiers does the opposite
#if QT_VERSION < 0x040000
	if (state & Qt::KeyButtonMask)
	{
		if (state & Qt::ControlButton) { state &= ~Qt::ControlButton;	state |= Qt::CTRL; }
		if (state & Qt::ShiftButton)   { state &= ~Qt::ShiftButton;	state |= Qt::SHIFT; }
		if (state & Qt::AltButton)     { state &= ~Qt::AltButton;		state |= Qt::ALT; }
# if QT_VERSION >= 0x030100
		if (state & Qt::MetaButton)    { state &= ~Qt::MetaButton; 	state |= Qt::META; }
# endif
	}
#endif
	return state;
}

enum FrustumSide
{
	RIGHT	= 0,		// The RIGHT side of the frustum
	LEFT	= 1,		// The LEFT	 side of the frustum
	BOTTOM	= 2,		// The BOTTOM side of the frustum
	TOP		= 3,		// The TOP side of the frustum
	BACK	= 4,		// The BACK	side of the frustum
	FRONT	= 5			// The FRONT side of the frustum
}; 

// Like above, instead of saying a number for the ABC and D of the plane, we
// want to be more descriptive.
enum PlaneData
{
	A = 0,				// The X value of the plane's normal
	B = 1,				// The Y value of the plane's normal
	C = 2,				// The Z value of the plane's normal
	D = 3				// The distance the plane is from the origin
};

View* View::ViewerImpl::s_pCurrentViewer = nullptr;

//当前视窗
View* View::ViewerImpl
	::CurrentViewer()
{
	return s_pCurrentViewer;
}

void View::ViewerImpl::NullCurrentViewer()
{
	s_pCurrentViewer = nullptr;
}

//构造函数
View::ViewerImpl
	::ViewerImpl(View* pInterface)
	: QObject(nullptr)
	, m_pInterface(pInterface)
	, m_bUsingFBO(false)
	, m_bUpdatedFBO(true)
	, m_bPivotShown(true)
	, m_bValidProjection(false)
	, m_bValidModelView(false)
	, m_bMouseMoving(false)
	, m_bRefreshMarked(false)
	, m_isApplyLodForTransfrom(true)
	, m_helpWidget(nullptr)
	, m_isDisplayMessage(false)
	, m_isGridDrawn(false)
	, m_action(NO_MOUSE_ACTION)
	, m_hudText(tr("DCLW"))
	, m_interActionMode(View::eNoInter)
	, m_matrixPiple(eCamera)
	, m_refreshRate(10) // 100 fps
	, m_continuousUpdate(false)
{
	//初始化freeglut
	QStringList arglist = qApp->arguments();
	int argc = arglist.size();
	std::string str = arglist[0].toStdString();
	char* argv = const_cast<char*>(str.data());

	glutInit(&argc, &argv);
	
	m_innerRoot = new DcGp::DcGpEntity("innerRoot");

	//设置默认为正投影
	m_params.projectionMode = eProjPerspective;

	//! 设置camera,初始化值，下一步在resize中更新Camera
	m_camera = new Camera;
	m_transForm = new DCCore::Transform;

	//设置灯光
	{
		//默认使用阳光效果
		m_bEnableSunLight = true;
		m_sunLightPos[0] = 0.0f;
		m_sunLightPos[1] = 1.0f;
		m_sunLightPos[2] = 1.0f;
		m_sunLightPos[3] = 0.0f;

		m_bEnableCustomLight = true;
		m_customLightPos[0] = 0.0f;
		m_customLightPos[1] = 0.0f;
		m_customLightPos[2] = 0.0f;
		m_customLightPos[3] = 1.0f;
	}

	//启用鼠标跟随
	m_pInterface->setMouseTracking(true);

	//鼠标事件的信号处理
	connect(this, SIGNAL(MousePressed(Qt::MouseButton, int, int)),
		m_pInterface, SIGNAL(MousePressed(Qt::MouseButton, int, int)));
	connect(this, SIGNAL(MouseReleased(Qt::MouseButton, int, int)),
		m_pInterface, SIGNAL(MouseReleased(Qt::MouseButton, int, int)));
	connect(this, SIGNAL(MouseMoving(Qt::MouseButtons, int, int)),
		m_pInterface, SIGNAL(MouseMoving(Qt::MouseButtons, int, int)));

	//矩阵变化的信号处理
	connect(this, SIGNAL(ProjectionMatrixChanged(const double*)),
		m_pInterface, SIGNAL(ProjectionMatrixChanged(const double*)));
	connect(this, SIGNAL(ModelViewMatrixChanged(const double*)),
		m_pInterface, SIGNAL(ModelViewMatrixChanged(const double*)));
	connect(this, SIGNAL(ViewportMatrixChanged(const int*)),
		m_pInterface, SIGNAL(ViewportMatrixChanged(const int*)));

	//当前缩放比例发生变化
	connect(this, SIGNAL(CurrentScaleChanged(double)),
		m_pInterface, SIGNAL(CurrentScaleChanged(double)));

	SetDefaultShortcuts();
	SetDefaultMouseBindings();

	connect(&m_messageTimer, SIGNAL(timeout()), SLOT(HideMessage()));
#if QT_VERSION >= 0x040000
	m_messageTimer.setSingleShot(true);
#endif
}

//析构
View::ViewerImpl
	::~ViewerImpl()
{
	if (m_innerRoot)
	{
		delete m_innerRoot;
		m_innerRoot = nullptr;
	}

	if (m_camera)
	{
		delete m_camera;
		m_camera = nullptr;
	}

	if (m_transForm)
	{
		delete m_transForm;
		m_transForm = nullptr;
	}
	//！清除所有的漫游器
	while(!m_manipulatorHandles.empty())
	{
		DCCore::GUIEventHandle* ehandle = m_manipulatorHandles.back();
		m_manipulatorHandles.pop_back();

		delete ehandle;
		ehandle = nullptr;
	}
	//m_manipulatorHandles.clear();
}

//设为当前
void View::ViewerImpl
	::SetCurrent()
{
	s_pCurrentViewer = m_pInterface;
}

//初始化OpenGL环境
void View::ViewerImpl
	::InitializeGL()
{
	//初始化glew
	GLToolkit::InitGlew();

	//加载OpenGL扩展
	LoadGLExtensions();

	//初始化投影矩阵
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//初始化模型视图矩阵
	glMatrixMode(GL_MODELVIEW);

	glGetDoublev(GL_MODELVIEW_MATRIX, m_params.matrixModelView);
	glLoadIdentity();

	//默认关闭透明效果
	glDisable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//没有全局环境
	float night[4] =	{0.0F,0.0F,0.0F,1.0F};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, night);
}

//大小变化
void View::ViewerImpl
	::ResizeGL(int iWidth, int iHeight)
{
	//更改大小
	m_glWidth = iWidth;
	m_glHeight = iHeight;

	//更新视口矩阵
	glViewport(0, 0, m_glWidth, m_glHeight);
	{
		//发送视口矩阵发生变化的信号
		glGetIntegerv(GL_VIEWPORT, m_params.matrixViewport);
		emit ViewportMatrixChanged(m_params.matrixViewport);
	}

	//! 测试viewport值， 在paintGL中更新投影模型视图矩阵
	GetCamera()->viewport()->set(0,0, m_glWidth, m_glHeight);
	
	

	//使投影矩阵和模型视图矩阵无效化
	DisableProjection();
	DisableModelView();

	//更新FBO
	if (m_pFBO || m_bUsingFBO)
	{
		UpdateFBO(m_glWidth, m_glHeight);
	}
	//更新GLFilter
	if (m_pGLFilter)
	{
		UpdateGLFilter(m_glWidth, m_glHeight);
	}

	//窗体更新大小
	m_pInterface->Resize(iWidth, iHeight);
}

void View::ViewerImpl::SetSunLightState(bool state)
{
	m_bEnableSunLight = state;
}

bool View::ViewerImpl::GetSunLightState()
{
	return m_bEnableSunLight;
}

void View::ViewerImpl::DrawRectangleZoom()
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, m_glWidth, m_glHeight, 0, 0.0, -1.0);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glLineWidth(2.0);
	glBegin(GL_LINE_LOOP);
	glVertex2i(m_pressPos.x(), m_pressPos.y());
	glVertex2i(m_prevPos.x(),  m_pressPos.y());
	glVertex2i(m_prevPos.x(),  m_prevPos.y());
	glVertex2i(m_pressPos.x(), m_prevPos.y());
	glEnd();
	glEnable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void View::ViewerImpl::DrawLightPos()
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glPushAttrib(GL_COLOR_BUFFER_BIT);


	glPopAttrib();
	glPopMatrix();
}

void View::ViewerImpl::UpdateFps(float deltaTime)
{
	const int avgSize =10;
	static float fpsVector[avgSize];
	static int j=0;
	float averageFps=0;
	if (deltaTime>0) {
		fpsVector[j]=deltaTime;
		j=(j+1) % avgSize;
	}
	for (int i=0;i<avgSize;i++) averageFps+=fpsVector[i];
	m_cfps=1000.0f/(averageFps/avgSize);

}

void View::ViewerImpl::UpdateTraversal()
{
	//! 判断根节点中是否存在 分页节点，存在则在线程中读取数据。

}

void View::ViewerImpl::Render()
{
	//! 帧速率统计
	QTime time;
	time.start();

	//! 每一帧的绘制，先更新数据，然后绘制
	UpdateTraversal();

	/*qDebug() << "PaintGLPaintGLPaintGLPaintGLPaintGLPaintGL" << endl;*/
	//FBO是否有效
	bool bValidFBO = !m_pFBO || (m_bUsingFBO && m_bUpdatedFBO) || m_pGLFilter;

	//FBO有效则启动fbo
	if (bValidFBO)
	{
		UpdateRender();
		m_bUpdatedFBO = false;
	}

	//渲染FBO
	RenderFBO();

	//绘制坐标轴
	RenderAxises();

	//! 绘制光源
	if (m_bEnableSunLight)
	{
		DrawLightPos();
	}

	//如果当前处于zoom_on_region状态下，绘制缩放区域矩形
	if (m_action == ZOOM_ON_REGION)
	{
		DrawRectangleZoom();
	}

	if (m_isDisplayMessage) DrawText(10, 20, m_message);

	if (!m_hudText.isEmpty())
	{
		glColor3f(1, 1, 1);
		QFont hudfont("YouYuan", 18, QFont::Bold);

		DrawText(10, 60, m_hudText, hudfont);
	}

	for (int i = 0; i != m_manipulatorHandles.size(); ++i)
	{
		if (m_manipulatorHandles[i]->GetValid())
		{
			m_manipulatorHandles[i]->updateEvent(m_pInterface);
		}
	}

	//!显示并更新帧速率
	if (m_cfps)
	{
		glColor3f(1, 1, 1);
		QFont hudfont("YouYuan", 18, QFont::Bold);

		QString fps = QString("FPS: %1\n").arg(m_cfps, 7, 'f', 1);
		DrawText(10, 80, fps, hudfont);
	}
	UpdateFps(time.elapsed());
}


//加载OpenGL扩展
void View::ViewerImpl
	::LoadGLExtensions()
{
	//shader不可用
	if (!GLToolkit::CheckShadersAvailability())
	{
	}
	//Shader可用
	else
	{
		//FBO不可用
		if (!GLToolkit::CheckFBOAvailability())
		{
		}
		//FBO可用
		else
		{
			//启用FBO
			m_bUsingFBO = true;
		}

		//颜色着色器（暂不确定是否添加）
	}
}

//更新FBO
bool View::ViewerImpl
	::UpdateFBO(unsigned uWidth, unsigned uHeight)
{
	//断开当前的帧缓冲，避免在初始化过程中进行重绘
	FrameBufferPtr pFBO = m_pFBO;
	m_pFBO = FrameBufferPtr();

	if (!pFBO)
		pFBO = FrameBufferPtr(new FrameBuffer());

	bool success = false;
	if (pFBO->Initialize(uWidth, uHeight))
	{
		if (pFBO->InitTexture(0, GL_RGBA, GL_RGBA, GL_FLOAT))
			success = pFBO->InitDepth(GL_CLAMP_TO_BORDER, GL_DEPTH_COMPONENT32, GL_NEAREST, GL_TEXTURE_2D);
	}

	//更新FBO失败
	if (!success)
	{
		m_bUsingFBO = false;

		return false;
	}

	m_pFBO = pFBO;
	m_bUpdatedFBO = true;

	return true;
}

//渲染FBO
void View::ViewerImpl
	::RenderFBO()
{
	//启用正投影
	EnableOrthoProjection();

	//关闭深度测试
	glDisable(GL_DEPTH_TEST);

	//根据FBO创建纹理
	GLuint textureScreen = 0;
	if (m_pFBO)
	{
		if (m_pGLFilter)
		{
			//we process GL filter
			GLuint depthTex = m_pFBO->GetDepthTexture();
			GLuint colorTex = m_pFBO->GetColorTexture(0);
			//minimal set of viewport parameters necessary for GL filters
			GLFilter::ViewportParameters parameters;
			parameters.perspectiveMode = m_params.projectionMode == eProjPerspective;
			parameters.zFar = m_params.zFar;
			parameters.zNear = m_params.zNear;
			parameters.zoom = parameters.perspectiveMode ? ComputePerspectiveZoom() : m_params.scale; //TODO: doesn't work well with EDL in perspective mode!
			//apply shader
			m_pGLFilter->Shade(depthTex, colorTex, parameters); 

			//if capture mode is ON: we only want to capture it, not to display it
			//if (!m_captureMode)
			textureScreen = m_pGLFilter->Texture();
		}
		else
		{
			//暂不考虑shader
			textureScreen = m_pFBO->GetColorTexture(0);
		}
	}

	//获取纹理成功则显示纹理
	if (glIsTexture(textureScreen))
	{
		//渲染纹理
		GLToolkit::RenderTexture2D(textureScreen, m_glWidth, m_glHeight);

		//清除深度缓冲区
		glClear(GL_DEPTH_BUFFER_BIT);
	}

	//绘制二维
	m_pInterface->Paint2D();
}

void View::ViewerImpl::SetInterActionMode(View::InterActionMode iaMode)
{
	m_interActionMode = iaMode;
}
void View::ViewerImpl::SetApplyLODState(bool state)
{
	m_isApplyLodForTransfrom = state;
}

//更新Render
void View::ViewerImpl
	::UpdateRender()
{

	//启用FBO
	if (m_pFBO)
	{
		m_pFBO->Start();
	}

	//EnableOrthoProjection();

	//关闭深度测试
	glDisable(GL_DEPTH_TEST);

	//渲染背景
	RenderBackground();

	//启用深度测试
	glEnable(GL_DEPTH_TEST);

	//启用光照
	if (m_bEnableSunLight)
	{
		EnableSunLight();
	}
	if (m_bEnableCustomLight)
	{
		EnableCustomLight();
	}

	//加载投影矩阵
	LoadProjectionMatrix();
	//加载模型视图矩阵
	LoadModelViewMatrix();

	//if (!m_isRotatinghide)
	//{
	//	//更新视景体
	//	CalculateFrustum();
	//	//视窗绘制
	//	m_pInterface->Paint3D();
	//}

	//! 如果处于三维浏览模式，旋转或者平移、缩放
	if (m_interActionMode != View::eNoInter && m_interActionMode != View::e2DOnly)
	{
		if (m_interActionMode == View::eRotate)
		{
			//绘制旋转符号
			if (m_bPivotShown)
			{
				RenderPivot();
			}
		}
		
		if (m_isApplyLodForTransfrom)
		{
			m_pInterface->FastDraw3D();
		}
		else
		{
			m_pInterface->Paint3D();
		}
	}
	else
	{
		//视窗绘制
		m_pInterface->Paint3D();
	}
	
	//! 绘制射线
	/*if (!m_rayPoints.empty())
	{
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glBegin(GL_LINES);
		glLineWidth(1);
		glVertex3fv((m_rayPoints[0]).u);
		glLineWidth(8);
		glVertex3fv((m_rayPoints[1]).u);
		glLineWidth(8);
		glEnd();
		glPopAttrib();
	}*/

	//绘制指示网格
	if (IsGridDrawn())
	{
		DrawGrid();
	}
	
	//关闭光照
	if (m_bEnableSunLight)
	{
		DisableSunLight();
	}
	if (m_bEnableCustomLight)
	{
		DisableCustomLight();
	}
	
	//停用FBO
	if (m_pFBO)
	{
		m_pFBO->Stop();
	}
}

//渲染背景
void View::ViewerImpl
	::RenderBackground()
{
	//获取背景规则
	GlobalParameters::BackgroundRole role
		= GlobalParameters::GetBackgroundRole();
	//获取背景
	QBrush background = GlobalParameters::GetBackground();

	//纯色背景
	if (role == GlobalParameters::eBckFill)
	{
		QColor color = background.color();

		//设置背景色
		glClearColor(color.redF(), color.greenF(), color.blueF(), color.alphaF());
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	//渐变背景
	else if (role == GlobalParameters::eBckGradient)
	{
		//启用正投影
		EnableOrthoProjection();

		float fWidth = static_cast<float>(m_glWidth) / 2.0f;
		float fHeight = static_cast<float>(m_glHeight) / 2.0f;

		//获取渐变
		QLinearGradient* pGradient = (QLinearGradient*)(background.gradient());
		//获取渐变的值
		QGradientStops grandients = pGradient->stops();
		//计算矩形的高度
		float fPosY = fHeight;

		//绘制渐变
		glBegin(GL_QUAD_STRIP);
		for (auto iter = grandients.constBegin();
			iter != grandients.constEnd(); ++iter)
		{
			//设置颜色
			QColor color = iter->second;
			glColor4f(color.redF(), color.greenF(), color.blueF(), color.alphaF());
			//更新顶点位置
			fPosY = fHeight - iter->first * m_glHeight;
			glVertex2f(-fWidth, fPosY);
			glVertex2f(fWidth, fPosY);

		}
		glEnd();

		//清除背景色
		glClear(GL_DEPTH_BUFFER_BIT);
	}
}

//启用正投影
void View::ViewerImpl
	::EnableOrthoProjection()
{
	//设置投影矩阵
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float halfW = float(m_glWidth) * 0.5;
	float halfH = float(m_glHeight) * 0.5;
	float maxS = (std::max)(halfW, halfH);
	glOrtho(-halfW, halfW, -halfH, halfH, -maxS, maxS);

	//设置模型视图矩阵
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void View::ViewerImpl::RecalculateProjectionMatrixByCamera()
{
	//获取OpenGL焦点
	m_pInterface->makeCurrent();

	//投影矩阵
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//获取边界盒
	View::BoundBox box = m_pInterface->BoundingBox();
	//边界盒中心
	Point_3 centerBox = box.center;
	//边界盒对角线长度
	float fDiagonalLength = (box.maxCorner - box.minCorner).Normal()*0.5;


	//旋转点位置
	Point_3 pivotPos = (m_params.objectCentric ? m_params.pivotPos : centerBox);

	//相机与旋转中心间的距离
	float fDist_CP = (m_params.cameraPos - pivotPos).Normal();
	//边界盒与旋转中心的距离
	float fDist_MP = (centerBox - pivotPos).Normal() + fDiagonalLength;

	if (m_bPivotShown && m_params.objectCentric)
	{
		//旋转符号的半径
		float pivotActualRadius = (m_params.PIVOT_RADIUS_PERCENT) * 
			static_cast<float>((std::min)(m_glWidth, m_glHeight)) * 0.5f;

		//旋转符号的缩放比例
		float pivotSymbolScale = pivotActualRadius * ComputeActualPixelSize();
		fDist_MP = std::max<float>(fDist_MP,pivotSymbolScale);
	}
	fDist_MP *= 1.01f; 

	if (m_params.projectionMode == eProjPerspective)
	{
		float zNear = static_cast<float>(fDist_MP) * 1e-3;
		//zNear = std::max<float>(fDist_CP-fDist_MP,zNear);

		double zFar = std::max<double>(fDist_CP + fDist_MP, 1.0);

		m_params.zNear = zNear;
		m_params.zFar = zFar;

		//视线比例
		double dAspect = static_cast<double>(m_glWidth)/
			static_cast<double>(m_glHeight);

		//gluPerspective(m_params.fov, dAspect, zNear, zFar);
		GetCamera()->setProjectionPerspective(m_params.fov, zNear, zFar);
	}
	else
	{
		//最大距离
		float maxDist = fDist_CP + fDist_MP;

		float maxDist_pix = maxDist / m_params.pixeSize * 
			m_params.scale;
		maxDist_pix = std::max<float>(maxDist_pix,1.0f);

		float halfW = static_cast<float>(m_glWidth)*0.5f;
		float halfH = static_cast<float>(m_glHeight)*0.5f;

		//glOrtho(-halfW,halfW,-halfH,halfH,-maxDist_pix,maxDist_pix);
		GetCamera()->setProjectionOrtho(-halfW,halfW,-halfH,halfH,-maxDist_pix,maxDist_pix);
	}

	//保存投影矩阵
	//glGetDoublev(GL_PROJECTION_MATRIX, m_params.matrixProjection);
	for (int i = 0; i != 16; ++i)
	{
		m_params.matrixProjection[i] = GetCamera()->projectionMatrix().ptr()[i];
	}
	//投影矩阵有效
	m_bValidProjection = true;
}

void View::ViewerImpl::RecalculateModelViewMatrixByCamera()
{
	//获取OpenGL焦点
	m_pInterface->makeCurrent();

	//模型视图矩阵
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	DCCore::mat4 scaleMt;
	//透视投影模式
	if (m_params.projectionMode == eProjPerspective)
	{
		//处理宽高比
		float aspect = (m_glHeight != 0 ? static_cast<float>(m_glWidth) / 
			(static_cast<float>(m_glHeight) * m_params.aspectWH) : 0.0f);

		//判断是否缩放
		if (aspect < 1.0)
		{
			scaleMt.scale(aspect, aspect, 1.0f);
		}
	}
	//正射投影
	else
	{
		//应用缩放比例
		float fScale = m_params.scale / m_params.pixeSize;

		//缩放
		scaleMt.scale(fScale, fScale, fScale);
	}

	//应用当前相机的参数
	DCCore::mat4 modelview;
	if (m_params.objectCentric)
	{
		////放置相机中心
		//glTranslatef(-m_params.cameraPos.x, 
		//	-m_params.cameraPos.y, 
		//	-m_params.cameraPos.z);

		////回到初始位置
		//glTranslatef(m_params.pivotPos.x, 
		//	m_params.pivotPos.y, 
		//	m_params.pivotPos.z);

		////r旋转
		//glMultMatrixd(m_params.matrixRotated.data());

		////设置旋转中心的位置
		//glTranslatef(-m_params.pivotPos.x, 
		//	-m_params.pivotPos.y, 
		//	-m_params.pivotPos.z);
	}

	modelview = scaleMt * m_camera->viewMatrix() * m_transForm->worldMatrix();
	//保存模型视图矩阵
	for (int i = 0; i != 16; ++i)
	{
		m_params.matrixModelView[i] = modelview.ptr()[i];
	}
	//启用模型视图矩阵
	m_bValidModelView = true;
}

//重新计算投影矩阵
void View::ViewerImpl
	::RecalculateProjectionMatrix()
{
	//获取OpenGL焦点
	m_pInterface->makeCurrent();

	//投影矩阵
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//获取边界盒
	View::BoundBox box = m_pInterface->BoundingBox();
	//边界盒中心
	Point_3 centerBox = box.center;
	//边界盒对角线长度
	float fDiagonalLength = (box.maxCorner - box.minCorner).Normal()*0.5;


	//旋转点位置
	Point_3 pivotPos = (m_params.objectCentric ? m_params.pivotPos : centerBox);

	//相机与旋转中心间的距离
	float fDist_CP = (m_params.cameraPos - pivotPos).Normal();
	//边界盒与旋转中心的距离
	float fDist_MP = (centerBox - pivotPos).Normal() + fDiagonalLength;

	if (m_bPivotShown && m_params.objectCentric)
	{
		//旋转符号的半径
		float pivotActualRadius = (m_params.PIVOT_RADIUS_PERCENT) * 
							static_cast<float>((std::min)(m_glWidth, m_glHeight)) * 0.5f;

		//旋转符号的缩放比例
		float pivotSymbolScale = pivotActualRadius * ComputeActualPixelSize();
		fDist_MP = std::max<float>(fDist_MP,pivotSymbolScale);
	}
	fDist_MP *= 1.01f; 

	if (m_params.projectionMode == eProjPerspective)
	{
		float zNear = static_cast<float>(fDist_MP) * 1e-3;
		//zNear = std::max<float>(fDist_CP-fDist_MP,zNear);

		double zFar = std::max<double>(fDist_CP + fDist_MP, 1.0);

		m_params.zNear = zNear;
		m_params.zFar = zFar;

		//视线比例
		double dAspect = static_cast<double>(m_glWidth)/
					static_cast<double>(m_glHeight);

		gluPerspective(m_params.fov, dAspect, zNear, zFar);
	}
	else
	{
		//最大距离
		float maxDist = fDist_CP + fDist_MP;

		float maxDist_pix = maxDist / m_params.pixeSize * 
							m_params.scale;
		maxDist_pix = std::max<float>(maxDist_pix,1.0f);

		float halfW = static_cast<float>(m_glWidth)*0.5f;
		float halfH = static_cast<float>(m_glHeight)*0.5f;

		glOrtho(-halfW,halfW,-halfH,halfH,-maxDist_pix,maxDist_pix);
	}

	//保存投影矩阵
	glGetDoublev(GL_PROJECTION_MATRIX, m_params.matrixProjection);
	//投影矩阵有效
	m_bValidProjection = true;
}

//重新计算模型视图矩阵
void View::ViewerImpl
	::RecalculateModelViewMatrix()
{
	//获取OpenGL焦点
	m_pInterface->makeCurrent();

	//模型视图矩阵
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//透视投影模式
	if (m_params.projectionMode == eProjPerspective)
	{
		//处理宽高比
		float aspect = (m_glHeight != 0 ? static_cast<float>(m_glWidth) / 
			(static_cast<float>(m_glHeight) * m_params.aspectWH) : 0.0f);
		
		//判断是否缩放
		if (aspect < 1.0)
			glScalef(aspect, aspect, 1.0f);
	}
	//正射投影
	else
	{
		//应用缩放比例
		float fScale = m_params.scale / m_params.pixeSize;

		//缩放
		glScalef(fScale, fScale, fScale);
	}

	//应用当前相机的参数
	DCCore::mat4 modelview;
	if (m_params.objectCentric)
	{
		//放置相机中心
		glTranslatef(-m_params.cameraPos.x, 
			-m_params.cameraPos.y, 
			-m_params.cameraPos.z);

		//回到初始位置
		glTranslatef(m_params.pivotPos.x, 
			m_params.pivotPos.y, 
			m_params.pivotPos.z);

		//r旋转
		glMultMatrixd(m_params.matrixRotated.data());

		//设置旋转中心的位置
		glTranslatef(-m_params.pivotPos.x, 
			-m_params.pivotPos.y, 
			-m_params.pivotPos.z);
	}
	else
	{
		//旋转
		glMultMatrixd(m_params.matrixRotated.data());

		//设置相机位置
		glTranslatef(-m_params.cameraPos.x, 
			-m_params.cameraPos.y, 
			-m_params.cameraPos.z);
	}	

	//保存模型视图矩阵
	glGetDoublev(GL_MODELVIEW_MATRIX, m_params.matrixModelView);
	
	//启用模型视图矩阵
	m_bValidModelView = true;
}

//获取投影模式
ProjectionMode View::ViewerImpl
	::GetProjection() const
{
	return m_params.projectionMode;
}
//设置投影模式
void View::ViewerImpl
	::SetProjection(ProjectionMode mode)
{
	m_params.projectionMode = mode;
	DisableProjection();
	DisableModelView();
}

//计算屏幕的像素点大小
float View::ViewerImpl
	::ComputeActualPixelSize()
{
	if (m_params.projectionMode != eProjPerspective)
	{
		return m_params.pixeSize / m_params.scale;
	}

	int minScreenDim = (std::min)(m_glWidth, m_glHeight);
	if (minScreenDim <= 0)
		return 1.0f;

	//相机中心到旋转中心的距离
	float zoomEquivalentDist = (m_params.cameraPos - m_params.pivotPos).Normal();

	return (zoomEquivalentDist * tan(DCCore::AngleToRadian(m_params.fov)) / minScreenDim);
}

//鼠标点击
void View::ViewerImpl
	::PressedMouse(QMouseEvent* event)
{
	//发送鼠标点击事件
	emit MousePressed(event->button(), event->x(), event->y());

	for (int i = 0; i != m_manipulatorHandles.size(); ++i)
	{
		if (m_manipulatorHandles[i]->GetValid())
		{
			m_manipulatorHandles[i]->mousePressEvent(event, m_pInterface);
		}
	}

}

//鼠标松开
void View::ViewerImpl
	::ReleasedMouse(QMouseEvent* event)
{
	//发送松开鼠标的事件
	emit MouseReleased(event->button(), event->x(), event->y());

	if (m_interActionMode == DCView::View::e2DOnly)
	{
			return;
	}

	for (int i = 0; i != m_manipulatorHandles.size(); ++i)
	{
		if (m_manipulatorHandles[i]->GetValid())
		{
			m_manipulatorHandles[i]->mouseReleaseEvent(event, m_pInterface);
		}
	}
}

//鼠标移动
void View::ViewerImpl
	::MovingMouse(QMouseEvent* event)
{
	//发送移动鼠标的事件
	emit MouseMoving(event->buttons(), event->x(), event->y());

	if (m_interActionMode == DCView::View::e2DOnly)
	{
		//if (event->buttons() == Qt::NoButton)
		return;
	}

	for (int i = 0; i != m_manipulatorHandles.size(); ++i)
	{
		if (m_manipulatorHandles[i]->GetValid())
		{
			m_manipulatorHandles[i]->mouseMoveEvent(event, m_pInterface);
		}
	}
}

//转动滚轮
void View::ViewerImpl
	::Wheeling(QWheelEvent* event)
{
	for (int i = 0; i != m_manipulatorHandles.size(); ++i)
	{
		if (m_manipulatorHandles[i]->GetValid())
		{
			m_manipulatorHandles[i]->wheelEvent(event, m_pInterface);
		}
	}

	Refresh();
}

void View::ViewerImpl::HandleKeyboardAction(KeyboardAction id)
{
	switch (id)
	{
	//case DRAW_AXIS :		toggleAxisIsDrawn(); break;
	case DRAW_GRID :		
		SetGridDrawnState(!IsGridDrawn()); 
		break;
	case DRAW_OPT_HIDING :		
		SetApplyLODState(!m_isApplyLodForTransfrom);
		DisplayMessage(m_isApplyLodForTransfrom ? ("采用细节模型") : ("不采用细节模型") );
		break;
	case HELP :			Help(); break;
	case PROJECTMODE : 
		SetProjection(GetProjection() == eProjOrtho ? eProjPerspective : eProjOrtho );
		DisplayMessage(GetProjection() == eProjOrtho ? ("平行") : ("透视") );
		break;
	case EYESHADER:
		{
			DCView::EDLFilterPtr pFilter = nullptr;
			if (!m_pGLFilter)
			{
				pFilter = DCView::EDLFilterPtr(new DCView::EDLFilter);
			}
			SetGLFilter(pFilter);
			DisplayMessage(pFilter ? ("高清渲染") : ("普通渲染") );
			break;
		}
	case SNAPSHOT_TO_CLIPBOARD:
		{
			SnapshotToClipboard(); 
			break;
		}
	}

	Refresh();
}

void View::ViewerImpl::KeyRelease(QKeyEvent* event)
{
	m_Keyboard.erase(Qt::Key(event->key()));

	//! 开关漫游器
	if (event->key() == Qt::Key_T)
	{
		//! 遍历所有漫游器
		for (int i = 0; i != m_manipulatorHandles.size(); ++i)
		{
			QString clName = m_manipulatorHandles[i]->metaObject()->className();
			if (clName == "DCGa::TrackballManipulator")
			{
				m_manipulatorHandles[i]->SetValid(true);
			}
			else
			{
				m_manipulatorHandles[i]->SetValid(false);
			}
		}
	}
	else if (event->key() == Qt::Key_F)
		{
			//! 遍历所有漫游器
			for (int i = 0; i != m_manipulatorHandles.size(); ++i)
			{
				QString clName = m_manipulatorHandles[i]->metaObject()->className();
				if (clName == "DCGa::FlyManipulator")
				{
					m_manipulatorHandles[i]->SetValid(true, m_pInterface);
				}
				else
				{
					m_manipulatorHandles[i]->SetValid(false, m_pInterface);
				}
			}
		}
}

void View::ViewerImpl::KeyPress(QKeyEvent* event)
{
	m_Keyboard.insert(Qt::Key(event->key()));

	if (event->key() == 0)
	{
		event->ignore();
		return;
	}

	const Qt::Key key = Qt::Key(event->key());
#if QT_VERSION >= 0x040000
	const Qt::KeyboardModifiers modifiers = event->modifiers();
#else
	const QtKeyboardModifiers modifiers = (QtKeyboardModifiers)(e->state() & Qt::KeyboardModifierMask);
#endif

	QMap<KeyboardAction, int>::ConstIterator it= m_keyboardBinding.begin(), end=m_keyboardBinding.end();
	while ((it != end) && (it.value() != (key | modifiers)))
		++it;

	if (it != end)
		HandleKeyboardAction(it.key());
}

//转动滚轴
void View::ViewerImpl
	::RotatingWheel(float fDegree)
{
	//透视投影模式
	if (m_params.projectionMode == eProjPerspective)
	{
		//角度到像素转换比例
		static const float c_deg2PixConversion = 4.0f;
		MoveCamera(Vector_3(0.0f,0.0f,-(c_deg2PixConversion * fDegree) * m_params.pixeSize));
	}
	//正射投影模式
	else
	{
		//计算缩放参数
		static const float fFactor = 20.0f;
		float fScale = pow(1.1f, fDegree / fFactor);
		
		//缩放
		Scale(fScale);
	}
}

//缩放
void View::ViewerImpl
	::Scale(float fScale)
{
	//只能在正射投影下进行缩放
	Q_ASSERT_X(m_params.projectionMode == eProjOrtho
		, "View::ViewerImpl::Scale", "Just work in ortho!");

	if (fScale > 0.0f && fScale != 1.0f)
		SetScale(m_params.scale * fScale);
}

//设置缩放因子
void View::ViewerImpl
	::SetScale(float fScale)
{
	//确保缩放比例在允许的范围内
	if (fScale < m_params.MIN_SCALE)
		fScale = m_params.MIN_SCALE;
	else if (fScale > m_params.MAX_SCALE)
		fScale = m_params.MAX_SCALE;

	if (m_params.scale != fScale)
	{
		m_params.scale = fScale;

		//当前缩放比例发生变化
		emit CurrentScaleChanged(m_params.scale);

		//使投影矩阵无效
		DisableProjection();
		//使模型视图矩阵无效
		DisableModelView();
	}
}

//刷新
void View::ViewerImpl
	::Refresh()
{
	//! 只重绘可见窗口
	if (m_pInterface && m_pInterface->isVisible())
	{
		ReDraw();
	}
}

void View::ViewerImpl::ReDraw()
{
	//更新FBO
	m_bUpdatedFBO = true;

	//更新OpenGL
	m_pInterface->updateGL();
}

//使投影矩阵无效
void View::ViewerImpl
	::DisableProjection()
{
	m_bValidProjection = false;
	//更新FBO
	m_bUpdatedFBO = true;
}

//使模型视图无效
void View::ViewerImpl
	::DisableModelView()
{
	m_bValidModelView = false;
	//更新FBO
	m_bUpdatedFBO = true;
}

//加载投影矩阵
void View::ViewerImpl
	::LoadProjectionMatrix()
{
	//如果投影矩阵无效，则重新计算投影矩阵
	if (!m_bValidProjection)
	{
		//重新计算矩阵
		if (m_matrixPiple == eOpenGL)
		{
			RecalculateProjectionMatrix();
		}
		else
		{
			RecalculateProjectionMatrixByCamera();
		}
		
	}

	//设置投影矩阵
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glLoadMatrixd(m_params.matrixProjection);

	//发送矩阵发生变化的信号
	emit ProjectionMatrixChanged(m_params.matrixProjection);
}

//加载模型视图矩阵
void View::ViewerImpl
	::LoadModelViewMatrix()
{
	//如果模型视图矩阵无效，则重新计算
	if (!m_bValidModelView)
	{
		//重新计算矩阵
		if (m_matrixPiple == eOpenGL)
		{
			RecalculateModelViewMatrix();
		}
		else
		{
			RecalculateModelViewMatrixByCamera();
		}
	}

	//设置模型视图矩阵
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glLoadMatrixd(m_params.matrixModelView);

	//发送矩阵发生变化的信号
	emit ModelViewMatrixChanged(m_params.matrixModelView);
}

//移动相机
void View::ViewerImpl
	::MoveCamera(const Vector_3& v)
{
	Vector_3 vCamera(v);

	//矩阵变化
	if (!m_params.objectCentric)
	{
		m_params.matrixRotated.transpose();

		Eigen::Matrix3d mat = m_params.matrixRotated.block(0, 0, 3, 3);
		Eigen::Vector3d vector;
		vector << v.x, v.y, v.z;
		vector = (mat * vector).matrix();

		vCamera = Vector_3(vector[0], vector[1], vector[2]);
	}

	UpdateCameraPos(m_params.cameraPos + vCamera);
}

void View::ViewerImpl::UpdateRotateView(const Matrix_4_4& rm)
{
	m_params.matrixRotated = rm;

	//无效化模型视图矩阵
	DisableModelView();
}

//更新相机位置
void View::ViewerImpl
	::UpdateCameraPos(const Point_3& pos)
{
	m_params.cameraPos = pos;

	if (m_camera)
	{
		DCCore::mat4 m = GetCamera()->modelingMatrix();
		m.setT(pos);
		GetCamera()->setModelingMatrix(m);
	}

	//将投影矩阵和模型视图矩阵设置为无效，重新计算
	DisableProjection();
	DisableModelView();
}

//标记重绘
void View::ViewerImpl
	::MarkRefreshing()
{
	m_bRefreshMarked = true;

	//使投影矩阵无效
	DisableProjection();
}

//计算鼠标方向
Vector_3 View::ViewerImpl
	::MouseOrientation(unsigned iX, unsigned iY)
{
	//窗体的大小
	float fWidth = m_pInterface->width();
	float fHeight = m_pInterface->height();
	float xc = static_cast<float>(fWidth / 2);
	float yc = static_cast<float>(fHeight / 2);

	GLdouble xp,yp;
	if (m_params.objectCentric)
	{
		GLdouble zp;

		//旋转中心的位置
		Point_3 ptPivot = m_params.pivotPos;
		gluProject(ptPivot.x, ptPivot.y, ptPivot.z, 
			m_params.matrixModelView, m_params.matrixProjection, m_params.matrixViewport,
			&xp, &yp, &zp);

		//we set the virtual rotation pivot closer to the actual one (but we always stay in the central part of the screen!)
		xp = std::min<GLdouble>(xp, 3 * fWidth / 4);
		xp = std::max<GLdouble>(xp, fWidth / 4);

		yp = std::min<GLdouble>(yp, 3 * fHeight / 4);
		yp = std::max<GLdouble>(yp, fHeight / 4);
	}
	else
	{
		xp = static_cast<GLdouble>(xc);
		yp = static_cast<GLdouble>(yc);
	}

	//invert y
	iY = fHeight - 1 - iY;

	Vector_3 vector(iX - xp, iY - yp, 0);

	vector.x = std::max<float>(std::min<float>(vector.x / xc, 1.0), -1.0);
	vector.y = std::max<float>(std::min<float>(vector.y / yc, 1.0), -1.0);

	//square 'radius'
	float d2 = vector.x * vector.x + vector.y * vector.y;

	//projection on the unit sphere
	if (d2 > 1.0f)
	{
		float d = sqrt(d2);
		vector.x /= d;
		vector.y /= d;
	}
	else
	{
		vector.z = sqrt(1.0f - d2);
	}

	return vector;
}

//旋转视图
void View::ViewerImpl
	::RotateView(const Matrix_4_4& matrix)
{
	//更新旋转矩阵
	m_params.matrixRotated = (matrix * m_params.matrixRotated).matrix();

	//无效化模型视图矩阵
	DisableModelView();
}

//绘制坐标轴
void View::ViewerImpl
	::RenderAxises()
{
	//坐标轴的位置
	float fPosX = -1.0f * (static_cast<float>(m_glWidth) * 0.5 - m_params.AXIS_LENGTH - 10.0);
	float fPosY = -1.0f * (static_cast<float>(m_glHeight) * 0.5 - m_params.AXIS_LENGTH - 5.0);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(fPosX, fPosY, 0.0f);
	glMultMatrixd(m_params.matrixRotated.data());

	//如果坐标轴的显示列表不存在，则创建显示列表
	if (!glIsList(m_axisListID))
	{
		m_axisListID = glGenLists(1);
		glNewList(m_axisListID, GL_COMPILE_AND_EXECUTE);

		glPushAttrib(GL_LINE_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_DEPTH_TEST);


		//绘制坐标轴文字
		glBegin(GL_LINES);
		// The Z
		glColor3f(0.0, 0.0, 1.0);
		glVertex3f(-5,  5, 30);
		glVertex3f(5, 5,  30);
		glVertex3f(5, 5, 30);
		glVertex3f(-5,  -5,  30);
		glVertex3f(-5,  -5,  30);
		glVertex3f(5,  -5,  30);

		// The X
		glColor3f(1.0, 0.0, 0.0);
		glVertex3f(30,  5, 0);
		glVertex3f(40, -5,  0);
		glVertex3f(30, -5, 0);
		glVertex3f(40,  5,  0);

		// The Y
		glColor3f(0.0, 1.0, 0.0);
		glVertex3f(0,  30, 0);
		glVertex3f(0, 35,  0);
		glVertex3f(0, 35,  0);
		glVertex3f(-5,  40,  0);
		glVertex3f(0, 35,  0);
		glVertex3f(5,  40,  0);
		glEnd();

		//添加Z轴
		//颜色为蓝色
		glColor3f(0.0, 0.0, 1.0);

		//轴体
		DrawAxis();
		//添加X轴
		//颜色为红色
		glColor3f(1.0, 0.0, 0.0);
		glRotatef(90, 0.0, 1.0, 0.0);
		//轴体
		DrawAxis();


		//添加Y轴
		//颜色为绿色
		glColor3f(0.0, 1.0, 0.0);
		glRotatef(270, 1.0, 0.0, 0.0);
		//轴体
		DrawAxis();		

		glPopAttrib();

		glEndList();
	}
	else
	{
		glCallList(m_axisListID);
	}
	

	glPopMatrix();
}

//绘制轴线
void View::ViewerImpl
	::DrawAxis()
{
	GLUquadric *pQuad = gluNewQuadric();
	//绘制圆柱面
	gluCylinder(pQuad, m_params.AXIS_RADIUS, 
		m_params.AXIS_RADIUS, m_params.AXIS_LENGTH, 
		10, 10);
	//封住圆柱的两端，保证坐标轴是实体的
	//封圆柱的底面
	DrawDisc(Point_3(0.0f, 0.0f, 0.0f), m_params.AXIS_RADIUS);
	//封圆柱的顶面
	DrawDisc(Point_3(0.0f, 0.0f, m_params.AXIS_LENGTH),
		m_params.AXIS_RADIUS);
	glTranslatef(0.0, 0.0, m_params.AXIS_LENGTH);
	glutSolidCone(2 * m_params.AXIS_RADIUS, 
		m_params.AXIS_LENGTH / 10, 10, 10);
	//封底
	DrawDisc(Point_3(0.0f, 0.0f, 0.0f), 2 * m_params.AXIS_RADIUS);
	glTranslatef(0.0, 0.0, -1.0f * m_params.AXIS_LENGTH);
}

//更新旋转中心
void View::ViewerImpl
	::UpdatePivot(const Point_3& ptPivot)
{ 

	DisableProjection();
	DisableModelView();
	Vector_3 dP = m_params.pivotPos - ptPivot;
	Vector_3 MdP = dP;

	Eigen::Matrix3d mat = m_params.matrixRotated.block(0, 0, 3, 3);
	Eigen::Vector3d vector;
	vector << MdP.x, MdP.y, MdP.z;
	vector = (mat * vector).matrix();

	MdP = Vector_3(vector[0], vector[1], vector[2]);

	Vector_3 newCameraPos = m_params.cameraPos + MdP - dP;
	if (m_matrixPiple == eOpenGL)
	{
		UpdateCameraPos(newCameraPos);
	}

	m_params.pivotPos = ptPivot;

	DisableProjection();
	DisableModelView();

	
}

//绘制旋转中心
void View::ViewerImpl
	::RenderPivot()
{
	if (!m_params.objectCentric)
		return;

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	//放置旋转中心
	glTranslatef(m_params.pivotPos.x, 
		m_params.pivotPos.y, 
		m_params.pivotPos.z);

	//compute actual symbol radius
	float symbolRadius = m_params.PIVOT_RADIUS_PERCENT * 
		(std::min)(m_glWidth, m_glHeight) * 0.5f;

	if (!glIsList(m_pivotListID))
	{
		m_pivotListID = glGenLists(1);
		glNewList(m_pivotListID, GL_COMPILE);
		GLUquadric *pQuad = gluNewQuadric();

		//draw 3 circles
		glPushAttrib(GL_LINE_BIT);
		glEnable(GL_LINE_SMOOTH);
		glPushAttrib(GL_COLOR_BUFFER_BIT);
		glEnable(GL_BLEND);
		glColor3f(1.0f,0.0f,0.0f);
		glutSolidSphere(10 / symbolRadius, 10, 10);
		const float c_alpha = 0.6f;
		//glLineWidth(2.0f);

		//pivot symbol: 3 circles
		glColor4f(1.0f,0.0f,0.0f,c_alpha);
		glPushMatrix();
		glutSolidTorus(1 / symbolRadius, 1.0, 100, 100);
		glRotatef(90, 0.0, 1.0, 0.0);
		glTranslatef(0.0, 0.0, -1.0);
		//绘制圆柱面
		gluCylinder(pQuad, 1 / symbolRadius, 1 / symbolRadius, 2.0, 10, 10);
		glPopMatrix();

		glColor4f(0.0f,1.0f,0.0f,c_alpha);
		glPushMatrix();
		glRotatef(90, 0.0, 1.0, 0.0);
		//glRotatef(90, 0.0, 0.0, 1.0);
		glutSolidTorus(1 / symbolRadius, 1.0, 100, 100);
		glRotatef(90, 1.0, 0.0, 0.0);
		glTranslatef(0.0, 0.0, -1.0);
		//绘制圆柱面
		gluCylinder(pQuad, 1 / symbolRadius, 1 / symbolRadius, 2.0, 10, 10);
		glPopMatrix();

		glColor4f(0.0f,0.7f,1.0f,c_alpha);
		glPushMatrix();
		glRotatef(90, 1.0, 0.0, 0.0);
		glutSolidTorus(1 / symbolRadius, 1.0, 100, 100);
		glRotatef(90, 1.0, 0.0, 0.0);
		glTranslatef(0.0, 0.0, -1.0);
		//绘制圆柱面
		gluCylinder(pQuad, 1 / symbolRadius, 1 / symbolRadius, 2.0, 10, 10);
		glPopMatrix();

		glPopAttrib();
		glPopAttrib();

		glEndList();
	}

	//constant scale
	float scale = symbolRadius * ComputeActualPixelSize();
	glScalef(scale,scale,scale);

	glCallList(m_pivotListID);

	glPopMatrix();
};

//启用旋转中心
void View::ViewerImpl
	::EnablePivot(bool bState)
{
	m_bPivotShown = bState;

	//旋转标志是否绘制
	if (m_params.objectCentric)
	{
		DisableProjection();
	}
	//m_bUpdatedFBO = true;
}

void View::ViewerImpl::Get3DRayUnderMouse(const Point_2& point2D, Point_3* v1, Point_3* v2)
{
	double dY = m_pInterface->height() - point2D.y;
	GLdouble rx, ry, rz;

	gluUnProject(point2D.x, dY, 0, 
		m_params.matrixModelView, m_params.matrixProjection, m_params.matrixViewport, 
		&rx, &ry, &rz);

	v1->x = rx;
	v1->y = ry;
	v1->z = rz;

	gluUnProject(point2D.x, dY, 1, 
		m_params.matrixModelView, m_params.matrixProjection, m_params.matrixViewport, 
		&rx, &ry, &rz);

	v2->x = rx;
	v2->y = ry;
	v2->z = rz;
	m_rayPoints.clear();
	m_rayPoints.push_back(*v1);
	m_rayPoints.push_back(*v2);
}

//屏幕坐标到世界坐标
bool View::ViewerImpl
	::ScreenToWorld(const Point_2& point2D,
	Point_3& point3D)
{
	double dY = m_pInterface->height() - point2D.y;
	GLdouble rx, ry, rz;

	gluUnProject(point2D.x, dY, 0.5, 
		m_params.matrixModelView, m_params.matrixProjection, m_params.matrixViewport, 
		&rx, &ry, &rz);

	point3D = Point_3(rx, ry, rz);

	return false;
}
//世界坐标到屏幕坐标
bool View::ViewerImpl
	::WorldToScreen(const Point_3& point3D,
	Point_2& point2D)
{
	GLdouble rx, ry, rz;

	gluProject(point3D.x, point3D.y, point3D.z, 
		m_params.matrixModelView, m_params.matrixProjection, m_params.matrixViewport, 
		&rx, &ry, &rz);

	const float fHalfWidth = m_pInterface->width() * 0.5f;
	const float fHalfHeight = m_pInterface->height() * 0.5f;

	point2D = Point_2(rx - fHalfWidth, ry - fHalfHeight);

	return true;
}

//设置视图
void View::ViewerImpl
	::SetView(View::ViewMode mode)
{
	m_pInterface->makeCurrent();

	m_params.matrixRotated = GLToolkit::GenerateViewMatrix(GLToolkit::ViewMode(mode));
	
	DCCore::mat4 rt;
	for (int i = 0; i != 16; ++i)
	{
		rt.ptr()[i] = m_params.matrixRotated.data()[i];
	}
	{
		//! 将旋转矩阵应用到camera中
		GetCamera()->setViewMatrixLookAt(DCVector3D(0,0,0), DCVector3D(0,0,-1), DCVector3D(0,1.0,0));
		DCCore::mat4 m = m_camera->modelingMatrix();
		m.setT(m_params.cameraPos);
		GetCamera()->setModelingMatrix( DCCore::mat4::getTranslation(m_params.pivotPos) * rt.invert() * DCCore::mat4::getTranslation(-m_params.pivotPos) * m );
	}
	

	DisableModelView();

	//刷新
	Refresh();
}

//重置视图
void View::ViewerImpl
	::ResetView()
{
	SetView(View::eTopViewMode);
	SetScale(1.0);
}

//设置GLFilter
void View::ViewerImpl
	::SetGLFilter(GLFilterPtr pFilter)
{
	//若要设置GLFilter，必须支持FBO
	if (!GLToolkit::CheckFBOAvailability())
	{
		DCCore::Logger::Warning("不支持FBO!");
		return;
	}

	//removeGLFilter();
	if (m_pGLFilter)
	{
		m_pGLFilter = nullptr;
	}

	if (pFilter)
	{
		//如果FBO不存在，则更新FBO
		if (!m_pFBO)
		{
			if (!UpdateFBO(m_glWidth, m_glHeight))
			{
				Refresh();
				return;
			}
		}

		m_pGLFilter = pFilter;
		UpdateGLFilter(m_glWidth, m_glHeight);
	}

	if (!m_pGLFilter && m_pFBO && !m_bUsingFBO)
	{
		m_pFBO = FrameBufferPtr();
	}

	Refresh();
}

//获取GLFilter
GLFilterPtr View::ViewerImpl::GetGLFilter() const
{
	return m_pGLFilter;
}

//更新GLFilter
bool View::ViewerImpl
	::UpdateGLFilter(unsigned uWidth, unsigned uHeight)
{
	if (!m_pGLFilter)
		return false;

	//we "disconnect" current glFilter, to avoid wrong display/errors
	//if QT tries to redraw window during initialization
	//断开当前的过滤，避免在初始化过程中进行重绘
	GLFilterPtr pGLFilter = m_pGLFilter;
	m_pGLFilter = GLFilterPtr();

	QString error;
	if (!pGLFilter->Init(uWidth, uHeight, GlobalParameters::ShadersPath(), error))
	{
		DCCore::Logger::Warning(("[GL Filter] 初始化失败!"));
		return false;
	}

	m_pGLFilter = pGLFilter;

	return true;
}

//计算透视投影缩放比例
float View::ViewerImpl
	::ComputePerspectiveZoom() const
{
	//如果是正投影，直接返回
	if (m_params.projectionMode == eProjOrtho)
		return m_params.scale;

	const float ZERO = 1e-8;
	//we compute the zoom equivalent to the corresponding camera position (inverse of above calculus)
	if (m_params.fov < ZERO)
		return 1.0f;

	//Camera center to pivot vector
	float zoomEquivalentDist = (m_params.cameraPos - m_params.pivotPos).Normal();
	if (zoomEquivalentDist < ZERO)
		return 1.0f;

	float screenSize = static_cast<double>((std::min)(m_glWidth,  m_glHeight)) * 
		m_params.pixeSize; //see how pixelSize is computed!
	return screenSize / (zoomEquivalentDist * tan(DCCore::AngleToRadian(m_params.fov)));
}

//启用太阳光
void View::ViewerImpl
	::EnableSunLight()
{
	glLightfv(GL_LIGHT0,GL_DIFFUSE, GlobalParameters::DiffuseLight());
	glLightfv(GL_LIGHT0,GL_AMBIENT, GlobalParameters::AmbientLight());
	glLightfv(GL_LIGHT0,GL_SPECULAR, GlobalParameters::SpecularLight());
	
	glLightfv(GL_LIGHT0, GL_POSITION, m_sunLightPos);

	//设置光照模式
	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE,GL_TRUE);
	glEnable(GL_LIGHT0);
	//glEnable(GL_NORMALIZE);
}
//关闭太阳光
void View::ViewerImpl
	::DisableSunLight()
{
	glDisable(GL_LIGHT0);
}
//启用自定义光
void View::ViewerImpl
	::EnableCustomLight()
{
	glLightfv(GL_LIGHT1,GL_DIFFUSE, GlobalParameters::DiffuseLight());
	glLightfv(GL_LIGHT1,GL_AMBIENT, GlobalParameters::AmbientLight());
	glLightfv(GL_LIGHT1,GL_SPECULAR, GlobalParameters::SpecularLight());

	glLightfv(GL_LIGHT1, GL_POSITION, m_customLightPos);

	//设置光照模式
	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glEnable(GL_LIGHT1);
}
//关闭自定义光
void View::ViewerImpl
	::DisableCustomLight()
{
	glDisable(GL_LIGHT1);
}

//更新中心并移动相机
void View::ViewerImpl
	::UpdateCenterAndZoom(const View::BoundBox& box)
{
	SetScale(1.0);

	//边界盒对角线长度
	float fDiagonalLength = (box.maxCorner - box.minCorner).Normal();

	if (fDiagonalLength < 1e-9)
	{
		DCCore::Logger::Warning(("不存在边界盒，无法缩放...")); 
		return;
	}

	//计算世界坐标系下像素的大小
	{
		int minScreenSize = (std::min)(m_glWidth, m_glHeight);
		m_params.pixeSize = (minScreenSize > 0 ? fDiagonalLength / static_cast<float>(minScreenSize) : 1.0f);
	}

	Point_3 P = box.center;
	//设置旋转中心
	UpdatePivot(P);

	if (m_params.projectionMode == eProjPerspective)
	{
		//we must go backward so as to see the object!
		assert(m_params.fov > 1e-9);
		float d = fDiagonalLength*0.5 / tan(m_params.fov*0.5 * DC_DEG_TO_RAD);
		auto aa = P - GetCurrentViewDirection() * d;
		UpdateCameraPos(P - GetCurrentViewDirection() * d);
	}
	else
	{
		UpdateCameraPos(P); //camera is on the pivot in ortho mode
	}

	//初始化记录box-to-camera距离
	//box中心与camer中心距离
	Point_3 centerBox = box.center;
	m_initdist_box_camer = (centerBox - m_params.cameraPos).Normal();

	//! 清空旋转矩阵
	if (m_matrixPiple == eCamera)
	{
		m_params.matrixRotated = m_params.matrixRotated.Identity();

		//! 更新相机参数
		//! 模型变换---到世界坐标系
		//m_transForm->setLocalMatrix(DCCore::mat4());
		//m_transForm->translate(-m_params.pivotPos);
		//m_transForm->computeWorldMatrix();

		//! 视图变化---到相机坐标系中
		GetCamera()->setViewMatrixLookAt(m_params.cameraPos, box.center, DCVector3D(0,1.0,0));
		//DCCore::mat4 m = m_camera->modelingMatrix();
		//m.setT(m_params.cameraPos);
		//m_camera->setModelingMatrix(m);
	}

	DisableModelView();
	DisableProjection();

	//Refresh();
}

Point_3 View::ViewerImpl::GetCurrentViewDirection() const
{
	if (m_params.objectCentric)
	{
		return Point_3(0.0f,0.0f,-1.0f);
	}
	const double* M = m_params.matrixRotated.data();
	Point_3 axis(-M[2],-M[6],-M[10]);
	axis.Normalize();

	return axis;
}

void View::ViewerImpl::CalculateFrustum()
{
	float   proj[16];								// This will hold our projection matrix
	float   modl[16];								// This will hold our modelview matrix
	float   clip[16];								// This will hold the clipping planes

	// glGetFloatv() is used to extract information about our OpenGL world.
	// Below, we pass in GL_PROJECTION_MATRIX to abstract our projection matrix.
	// It then stores the matrix into an array of [16].
	glGetFloatv( GL_PROJECTION_MATRIX, proj );

	// By passing in GL_MODELVIEW_MATRIX, we can abstract our model view matrix.
	// This also stores it in an array of [16].
	glGetFloatv( GL_MODELVIEW_MATRIX, modl );

	// Now that we have our modelview and projection matrix, if we combine these 2 matrices,
	// it will give us our clipping planes.  To combine 2 matrices, we multiply them.

	clip[ 0] = modl[ 0] * proj[ 0] + modl[ 1] * proj[ 4] + modl[ 2] * proj[ 8] + modl[ 3] * proj[12];
	clip[ 1] = modl[ 0] * proj[ 1] + modl[ 1] * proj[ 5] + modl[ 2] * proj[ 9] + modl[ 3] * proj[13];
	clip[ 2] = modl[ 0] * proj[ 2] + modl[ 1] * proj[ 6] + modl[ 2] * proj[10] + modl[ 3] * proj[14];
	clip[ 3] = modl[ 0] * proj[ 3] + modl[ 1] * proj[ 7] + modl[ 2] * proj[11] + modl[ 3] * proj[15];

	clip[ 4] = modl[ 4] * proj[ 0] + modl[ 5] * proj[ 4] + modl[ 6] * proj[ 8] + modl[ 7] * proj[12];
	clip[ 5] = modl[ 4] * proj[ 1] + modl[ 5] * proj[ 5] + modl[ 6] * proj[ 9] + modl[ 7] * proj[13];
	clip[ 6] = modl[ 4] * proj[ 2] + modl[ 5] * proj[ 6] + modl[ 6] * proj[10] + modl[ 7] * proj[14];
	clip[ 7] = modl[ 4] * proj[ 3] + modl[ 5] * proj[ 7] + modl[ 6] * proj[11] + modl[ 7] * proj[15];

	clip[ 8] = modl[ 8] * proj[ 0] + modl[ 9] * proj[ 4] + modl[10] * proj[ 8] + modl[11] * proj[12];
	clip[ 9] = modl[ 8] * proj[ 1] + modl[ 9] * proj[ 5] + modl[10] * proj[ 9] + modl[11] * proj[13];
	clip[10] = modl[ 8] * proj[ 2] + modl[ 9] * proj[ 6] + modl[10] * proj[10] + modl[11] * proj[14];
	clip[11] = modl[ 8] * proj[ 3] + modl[ 9] * proj[ 7] + modl[10] * proj[11] + modl[11] * proj[15];

	clip[12] = modl[12] * proj[ 0] + modl[13] * proj[ 4] + modl[14] * proj[ 8] + modl[15] * proj[12];
	clip[13] = modl[12] * proj[ 1] + modl[13] * proj[ 5] + modl[14] * proj[ 9] + modl[15] * proj[13];
	clip[14] = modl[12] * proj[ 2] + modl[13] * proj[ 6] + modl[14] * proj[10] + modl[15] * proj[14];
	clip[15] = modl[12] * proj[ 3] + modl[13] * proj[ 7] + modl[14] * proj[11] + modl[15] * proj[15];

	// Now we actually want to get the sides of the frustum.  To do this we take
	// the clipping planes we received above and extract the sides from them.

	// This will extract the RIGHT side of the frustum
	m_Frustum[RIGHT][A] = clip[ 3] - clip[ 0];
	m_Frustum[RIGHT][B] = clip[ 7] - clip[ 4];
	m_Frustum[RIGHT][C] = clip[11] - clip[ 8];
	m_Frustum[RIGHT][D] = clip[15] - clip[12];

	// Now that we have a normal (A,B,C) and a distance (D) to the plane,
	// we want to normalize that normal and distance.

	// Normalize the RIGHT side
	NormalizePlane(m_Frustum, RIGHT);

	// This will extract the LEFT side of the frustum
	m_Frustum[LEFT][A] = clip[ 3] + clip[ 0];
	m_Frustum[LEFT][B] = clip[ 7] + clip[ 4];
	m_Frustum[LEFT][C] = clip[11] + clip[ 8];
	m_Frustum[LEFT][D] = clip[15] + clip[12];

	// Normalize the LEFT side
	NormalizePlane(m_Frustum, LEFT);

	// This will extract the BOTTOM side of the frustum
	m_Frustum[BOTTOM][A] = clip[ 3] + clip[ 1];
	m_Frustum[BOTTOM][B] = clip[ 7] + clip[ 5];
	m_Frustum[BOTTOM][C] = clip[11] + clip[ 9];
	m_Frustum[BOTTOM][D] = clip[15] + clip[13];

	// Normalize the BOTTOM side
	NormalizePlane(m_Frustum, BOTTOM);

	// This will extract the TOP side of the frustum
	m_Frustum[TOP][A] = clip[ 3] - clip[ 1];
	m_Frustum[TOP][B] = clip[ 7] - clip[ 5];
	m_Frustum[TOP][C] = clip[11] - clip[ 9];
	m_Frustum[TOP][D] = clip[15] - clip[13];

	// Normalize the TOP side
	NormalizePlane(m_Frustum, TOP);

	// This will extract the BACK side of the frustum
	m_Frustum[BACK][A] = clip[ 3] - clip[ 2];
	m_Frustum[BACK][B] = clip[ 7] - clip[ 6];
	m_Frustum[BACK][C] = clip[11] - clip[10];
	m_Frustum[BACK][D] = clip[15] - clip[14];

	// Normalize the BACK side
	NormalizePlane(m_Frustum, BACK);

	// This will extract the FRONT side of the frustum
	m_Frustum[FRONT][A] = clip[ 3] + clip[ 2];
	m_Frustum[FRONT][B] = clip[ 7] + clip[ 6];
	m_Frustum[FRONT][C] = clip[11] + clip[10];
	m_Frustum[FRONT][D] = clip[15] + clip[14];

	// Normalize the FRONT side
	NormalizePlane(m_Frustum, FRONT);
}

void View::ViewerImpl::NormalizePlane(std::array<std::array<float, 4> ,6> frustum, int side)
{
	// Here we calculate the magnitude of the normal to the plane (point A B C)
	// Remember that (A, B, C) is that same thing as the normal's (X, Y, Z).
	// To calculate magnitude you use the equation:  magnitude = sqrt( x^2 + y^2 + z^2)
	float magnitude = (float)sqrt( (frustum[side][A]) * frustum[side][A] + 
		frustum[side][B] * frustum[side][B] + 
		frustum[side][C] * frustum[side][C] );

	// Then we divide the plane's values by it's magnitude.
	// This makes it easier to work with.
	frustum[side][A] /= magnitude;
	frustum[side][B] /= magnitude;
	frustum[side][C] /= magnitude;
	frustum[side][D] /= magnitude; 
}

std::array<std::array<float, 4> ,6> View::ViewerImpl::GetFrustum()
{
	return m_Frustum;
}

const ViewerParams& View::ViewerImpl::GetViewportParameters() const
{
	return m_params;
}

QString View::ViewerImpl::HelpString() const
{
	return ("暂无帮助信息");
}

static QString KeyboardModifiersString(Qt::KeyboardModifiers m, bool noButton=false)
{
#if QT_VERSION >= 0x040000
	if (noButton && (m==Qt::NoModifier)) 
#else
	if (noButton && (m==Qt::NoButton))
#endif
		return ("(无按钮)");

	QString keySequence = "";

#if QT_VERSION >= 0x040000
	if (m & Qt::ControlModifier) keySequence += "Ctrl+";
	if (m & Qt::AltModifier) keySequence += "Alt+";
	if (m & Qt::ShiftModifier) keySequence += "Shift+";
	if (m & Qt::MetaModifier) keySequence += "Meta+";
#else
	if (m & Qt::ControlButton) keySequence += "Ctrl+";
	if (m & Qt::AltButton) keySequence += "Alt+";
	if (m & Qt::ShiftButton) keySequence += "Shift+";
# if QT_VERSION >= 0x030000
	if (m & Qt::MetaButton) keySequence += "Meta+";
# endif
#endif

	if (keySequence.length() > 0)
#if QT_VERSION >= 0x040000
		return QKeySequence(keySequence+"X").toString(QKeySequence::NativeText).replace("X", "");
#else
		return QString(QKeySequence(keySequence+"X")).replace("X", "");
#endif
	else
		return QString();
}

static QString MouseButtonsString(Qt::MouseButtons b)
{
	QString result("");
	bool addAmpersand = false;
	if (b & Qt::LeftButton)    { result += ("左", "鼠标左键"); addAmpersand=true; }
	if (b & Qt::MidButton)     { if (addAmpersand) result += " & "; result += ("中", "鼠标中键"); addAmpersand=true; }
	if (b & Qt::RightButton)   { if (addAmpersand) result += " & "; result += ("右", "鼠标右键"); }
	return result;
}

QString View::ViewerImpl::FormatClickActionPrivate(ClickActionPrivate cap)
{
	bool buttonsBefore = cap.buttonsBefore != Qt::NoButton;
	return tr("%1%2%3%4%5%6", "Modifier / button or wheel / double click / with / button / pressed")
		.arg(KeyboardModifiersString(cap.modifiers))
		.arg(MouseButtonsString(cap.button)+(cap.button == Qt::NoButton ? tr("Wheel", "Mouse wheel") : ""))
		.arg(cap.doubleClick ? tr(" double click", "Suffix after mouse button") : "")
		.arg(buttonsBefore ? tr(" with ", "As in : Left button with Ctrl pressed") : "")
		.arg(buttonsBefore ? MouseButtonsString(cap.buttonsBefore) : "")
		.arg(buttonsBefore ? tr(" pressed", "As in : Left button with Ctrl pressed") : "");
}

void View::ViewerImpl::SetMouseBindingDescription(int state, QString description, bool doubleClick, Qt::MouseButtons buttonsBefore)
{
	ClickActionPrivate cap;
	cap.modifiers = Qt::KeyboardModifiers(ConvertToKeyboardModifiers(state) & Qt::KeyboardModifierMask);
	cap.button = Qt::MouseButtons(state & Qt::MouseButtonMask);
	cap.doubleClick = doubleClick;
	cap.buttonsBefore = buttonsBefore;

	if (description.isEmpty())
		m_mouseDescription.remove(cap);
	else
		m_mouseDescription[cap] = description;
}

QString View::ViewerImpl::MouseString() const
{
	QString text("<center><table border=\"1\" cellspacing=\"0\" cellpadding=\"4\">\n");
	const QString trtd("<tr><td>");
	const QString tdtr("</td></tr>\n");
	const QString tdtd("</td><td>");

	text += QString("<tr bgcolor=\"#aaaacc\"><th align=\"center\">%1</th><th align=\"center\">%2</th></tr>\n").
		arg(("鼠标按键")).arg(("描述"));

	QMap<ClickActionPrivate, QString> mouseBinding;

	// User-defined mouse bindings come first.
	for (QMap<ClickActionPrivate, QString>::ConstIterator itm=m_mouseDescription.begin(), endm=m_mouseDescription.end(); itm!=endm; ++itm)
		mouseBinding[itm.key()] = itm.value();

	for (QMap<ClickActionPrivate, QString>::ConstIterator it=mouseBinding.begin(), end=mouseBinding.end(); it != end; ++it)
	{
		// Should not be needed (see setMouseBindingDescription())
		if (it.value().isNull())
			continue;

		text += TableLine(FormatClickActionPrivate(it.key()), it.value());
	}

	// Optionnal separator line
	if (!mouseBinding.isEmpty())
	{
		mouseBinding.clear();
		text += QString("<tr bgcolor=\"#aaaacc\"><td colspan=2>%1</td></tr>\n").arg(tr("Standard mouse bindings", "In help window mouse tab"));
	}

	// Then concatenates the descriptions of m_wheelBinding, m_mouseBinding and clickBinding_.
	// The order is significant and corresponds to the priorities set in mousePressEvent() (reverse priority order, last one overwrites previous)
	// #CONNECTION# mousePressEvent() order
	for (QMap<Qt::KeyboardModifiers, MouseActionPrivate>::ConstIterator itw=m_wheelBinding.begin(), endw=m_wheelBinding.end(); itw != endw; ++itw)
	{
		ClickActionPrivate cap;
		cap.doubleClick = false;
		cap.modifiers = itw.key();
		cap.button = Qt::NoButton;
		cap.buttonsBefore = Qt::NoButton;

		QString text = MouseActionString(itw.value().action);

		if (!text.isNull())
		{
			switch (itw.value().handler)
			{
			case CAMERA: text += " " + tr("相机"); break;
			case FRAME:  text += " " + tr("manipulated frame"); break;
			}
			if (!(itw.value().withConstraint))
				text += "*";
		}

		mouseBinding[cap] = text;
	}

	for (QMap<int, MouseActionPrivate>::ConstIterator itmb=m_mouseBinding.begin(), endmb=m_mouseBinding.end();
		itmb != endmb; ++itmb)
	{
		ClickActionPrivate cap;
		cap.doubleClick = false;
		cap.modifiers = Qt::KeyboardModifiers(itmb.key() & Qt::KeyboardModifierMask);
		cap.button = Qt::MouseButtons(itmb.key() & Qt::MouseButtonMask);
		cap.buttonsBefore = Qt::NoButton;

		QString text = MouseActionString(itmb.value().action);

		if (!text.isNull())
		{
			switch (itmb.value().handler)
			{
			case CAMERA: text += " " + tr("相机", "Suffix after action"); break;
			case FRAME:  text += " " + tr("manipulated frame", "Suffix after action"); break;
			}
			if (!(itmb.value().withConstraint))
				text += "*";
		}
		mouseBinding[cap] = text;
	}

	for (QMap<ClickActionPrivate, ClickAction>::ConstIterator itcb=m_clickBinding.begin(), endcb=m_clickBinding.end(); itcb!=endcb; ++itcb)
		mouseBinding[itcb.key()] = ClickActionString(itcb.value());

	for (QMap<ClickActionPrivate, QString>::ConstIterator it2=mouseBinding.begin(), end2=mouseBinding.end(); it2 != end2; ++it2)
	{
		if (it2.value().isNull())
			continue;

		text += TableLine(FormatClickActionPrivate(it2.key()), it2.value());
	}

	text += "</table></center>";

	return text;
}

static QString KeyString(int key)
{
#if QT_VERSION >= 0x030000
	return QString(QKeySequence(ConvertToShortModifier(key)).toString());
#else
	// #CONNECTION# setKeyDescription. In Qt 2.3, longs modifier overlap with key codes.
	return QString(QKeySequence(key));
#endif
}

QString View::ViewerImpl::KeyboardString() const
{
	QString text("<center><table border=\"1\" cellspacing=\"0\" cellpadding=\"4\">\n");
	text += QString("<tr bgcolor=\"#aaaacc\"><th align=\"center\">%1</th><th align=\"center\">%2</th></tr>\n").
		arg(("快捷键")).arg(("描述"));

	QMap<int, QString> keyDescription;

	//用户自定义的描述信息
	for (QMap<int, QString>::ConstIterator kd = m_keyDescription.begin(), kdend=m_keyDescription.end(); kd!=kdend; ++kd)
	{
		keyDescription[kd.key()] = kd.value();
	}
	//添加到text中
	for (QMap<int, QString>::ConstIterator kb=keyDescription.begin(), endb=keyDescription.end(); kb!=endb; ++kb)
	{
		text += TableLine(KeyString(kb.key()), kb.value());
	}

	// 2 不同类别风格行
	if (!keyDescription.isEmpty())
	{
		keyDescription.clear();
		text += QString("<tr bgcolor=\"#aaaacc\"><td colspan=2>%1</td></tr>\n").arg(tr("Base viewer keys", "In help window keys tab"));
	}


	// 3 - KeyboardAction bindings description
	for (QMap<KeyboardAction, int>::ConstIterator it=m_keyboardBinding.begin(), end=m_keyboardBinding.end(); it != end; ++it)
	{
		if ( (it.value() != 0) )
		{
			keyDescription[it.value()] = m_keyboardActionDescription[it.key()];
		}
	}

	// Add to text in sorted order
	for (QMap<int, QString>::ConstIterator kb2=keyDescription.begin(), endb2=keyDescription.end(); kb2!=endb2; ++kb2)
		text += TableLine(KeyString(kb2.key()), kb2.value());

	text += "</table></center>";

	return text;
}

void View::ViewerImpl::Help()
{
	bool resize = false;
	int width=600;
	int height=400;

	static QString label[] = {"帮助", "快捷按键", "鼠标键", "关于"};

	if (!m_helpWidget)
	{
		// Qt4 requires a NULL parent...Key(s)
		m_helpWidget = new QTabWidget();
		m_helpWidget->setStyleSheet("color: rgb(0, 0, 255)");
#if QT_VERSION >= 0x040000
		m_helpWidget->setWindowTitle("帮助");
		QString path = QApplication::applicationFilePath();
		QString icon = path + "/" + "Resources/logo/DC.ico";
		m_helpWidget->setWindowIcon(QIcon(icon));
#else
		helpWidget()->setCaption(("帮助", "帮助标题"));
#endif

		resize = true;
		for (int i=0; i<4; ++i)
		{
			QTextEdit* tab = new QTextEdit(NULL);
#if QT_VERSION >= 0x030000
			tab->setReadOnly(true);
#endif

#if QT_VERSION >= 0x040000
			m_helpWidget->insertTab(i, tab, label[i]);
			if (i==3) {
#               include "logo.xpm"
				QPixmap pixmap(logo_xpm);
				tab->document()->addResource(QTextDocument::ImageResource,
					QUrl("mydata://logo.xpm"), QVariant(pixmap));
			}
#else
			tab->setTextFormat(Qt::RichText);
			helpWidget()->insertTab(tab, label[i]);
#endif
		}
	}


	for (int i=0; i<4; ++i)
	{
		QString text;
		switch (i)
		{
		case 0 : text = m_pInterface->HelpString();	  break;
		case 1 : text = m_pInterface->KeyboardString(); break;
		case 2 : text = m_pInterface->MouseString();	  break;
		case 3 : text = QString("<center><br><img src=\"mydata://logo.xpm\">") + tr(
					 "<h1>三维浏览器</h1>"
					 "<h3>版本 %1</h3><br>"																	  
					 "基于OpenGL 和 Qt设计的三维浏览器<br>"
					 "版权 2012-%2 鼎创力维<br>"
					 "<code>%3</code>").arg(DCLWViewerVersionString()).arg("2014").arg("http://www.bjdclw.com") +
					 QString("</center>");
			break;
		default : break;
		}

#if QT_VERSION >= 0x040000
		QTextEdit* textEdit = (QTextEdit*)(m_helpWidget->widget(i));
		textEdit->setHtml(text);
		textEdit->setStyleSheet("color: rgb(0, 0, 255)");
#else
# if QT_VERSION < 0x030000
		helpWidget()->setCurrentPage(i);
		QTextEdit* textEdit = (QTextEdit*)(helpWidget()->currentPage());
# else
		QTextEdit* textEdit = (QTextEdit*)(helpWidget()->page(i));
# endif
		textEdit->setText(text);
		textEdit->setStyleSheet("color: rgb(0, 0, 255)");
#endif

#if QT_VERSION < 0x040000
		if (resize && (textEdit->heightForWidth(width) > height))
			height = textEdit->heightForWidth(width);
#else
		if (resize && (textEdit->height() > height))
			height = textEdit->height();
#endif
	}

#if QT_VERSION < 0x030000
	helpWidget()->setCurrentPage(currentPageIndex);
#endif

	if (resize)
		m_helpWidget->resize(width, height+40); // 40 pixels is ~ tabs' height
	m_helpWidget->show();
	m_helpWidget->raise();
}

void View::ViewerImpl::SetKeyDescription(int key, QString description)
{
#if QT_VERSION >= 0x030000
	// #CONNECTION# keyString. In Qt 2.3, longs modifier overlap with key codes.
	key = ConvertToKeyboardModifiers(key);
#endif
	if (description.isEmpty())
		m_keyDescription.remove(key);
	else
		m_keyDescription[key] = description;
}

void View::ViewerImpl::SetShortcut(KeyboardAction action, int key)
{
	m_keyboardBinding[action] = ConvertToKeyboardModifiers(key);
}

void View::ViewerImpl::SetDefaultShortcuts()
{
	SetShortcut(DRAW_AXIS,	Qt::Key_A);
	SetShortcut(DRAW_GRID,	Qt::Key_G);
	SetShortcut(DRAW_OPT_HIDING,	Qt::Key_O);
	SetShortcut(HELP,		Qt::Key_H);
	SetShortcut(PROJECTMODE,		Qt::Key_P);
	SetShortcut(EYESHADER,		Qt::Key_E);
	SetShortcut(SNAPSHOT_TO_CLIPBOARD,Qt::CTRL+Qt::Key_C);

	m_keyboardActionDescription[DRAW_AXIS] = tr("开关世界坐标系坐标轴绘制", "DRAW_AXIS action description");
	m_keyboardActionDescription[DRAW_GRID] = tr("开关平面指示网格绘制", "DRAW_GRID action description");
	m_keyboardActionDescription[DRAW_OPT_HIDING] = tr("开关细节模型", "DRAW_OPT_HIDING action description");
	m_keyboardActionDescription[HELP] = tr("打开帮助文档", "HELP action description");
	m_keyboardActionDescription[PROJECTMODE] = tr("切换投影模式", "PROJECTMODE action description");
	m_keyboardActionDescription[EYESHADER] = tr("高质量渲染", "EYESHADER action description");
	m_keyboardActionDescription[SNAPSHOT_TO_CLIPBOARD] = tr("屏幕截图", "SNAPSHOT_TO_CLIPBOARD action description");
}

void View::ViewerImpl::SetWheelBinding(Qt::KeyboardModifiers modifiers, MouseHandler handler, MouseAction action, bool withConstraint)
{
	//#CONNECTION# ManipulatedFrame::wheelEvent and ManipulatedCameraFrame::wheelEvent switches
	if ((action != ZOOM) && (action != MOVE_FORWARD) && (action != MOVE_BACKWARD) && (action != NO_MOUSE_ACTION))
#if QT_VERSION >= 0x040000
		qWarning("Cannot bind %s to wheel", MouseActionString(action).toLatin1().constData());
#else
		qWarning("Cannot bind %s to wheel",  + mouseActionString(action).latin1());
#endif
	else
		if ((handler == FRAME) && (action != ZOOM) && (action != NO_MOUSE_ACTION))
#if QT_VERSION >= 0x040000
			qWarning("Cannot bind %s to FRAME wheel", MouseActionString(action).toLatin1().constData());
#else
			qWarning("Cannot bind %s to FRAME wheel", mouseActionString(action).latin1());
#endif
		else
		{
			MouseActionPrivate map;
			map.handler = handler;
			map.action  = action;
			map.withConstraint = withConstraint;
			modifiers = ConvertKeyboardModifiers(modifiers);
			m_wheelBinding.remove(modifiers);

			if (action != NO_MOUSE_ACTION)
				m_wheelBinding.insert(modifiers, map);
		}
}

void View::ViewerImpl::SetDefaultMouseBindings()
{
#if QT_VERSION >= 0x040000
	const Qt::KeyboardModifiers cameraKeyboardModifiers = Qt::NoModifier;
	const Qt::KeyboardModifiers frameKeyboardModifiers = Qt::ControlModifier;
#else
	const Qt::KeyboardModifiers cameraKeyboardModifiers = Qt::NoButton;
	const Qt::KeyboardModifiers frameKeyboardModifiers = Qt::ControlButton;
#endif
	//#CONNECTION# toggleCameraMode()
	for (int handler=0; handler<1; ++handler)
	{
		MouseHandler mh = (MouseHandler)(handler);
		Qt::KeyboardModifiers modifiers = (mh == FRAME) ? frameKeyboardModifiers : cameraKeyboardModifiers;

		if (handler == 0)
		{
			//获取漫游组合键
			GlobalParameters::CompositeKey panKey = GlobalParameters::GetCompositeKey(GlobalParameters::eTranslateOperation);
			SetMouseBinding(panKey.keys | panKey.buttons,   mh, TRANSLATE);

			GlobalParameters::CompositeKey rotateKey = GlobalParameters::GetCompositeKey(GlobalParameters::eRotateOperation);
			SetMouseBinding(rotateKey.keys | rotateKey.buttons,   mh, ROTATE);

			GlobalParameters::CompositeKey zoomKey = GlobalParameters::GetCompositeKey(GlobalParameters::eScaleOperation);
			SetWheelBinding(zoomKey.keys, mh, ZOOM);
		}

	}

#if QT_VERSION >= 0x040000
	// Z o o m   o n   r e g i o n
	SetMouseBinding(Qt::ShiftModifier | Qt::MidButton, CAMERA, ZOOM_ON_REGION);
	// S e l e c t
	//SetMouseBinding(Qt::ShiftModifier | Qt::LeftButton, SELECT);
#else
	SetMouseBinding(Qt::ShiftButton | Qt::MidButton, CAMERA, ZOOM_ON_REGION);
	SetMouseBinding(Qt::ShiftButton | Qt::LeftButton, SELECT);
#endif

	// D o u b l e   c l i c k
	//SetMouseBinding(Qt::LeftButton,  ALIGN_CAMERA,      true);
	//SetMouseBinding(Qt::MidButton,   SHOW_ENTIRE_SCENE, true);
	//SetMouseBinding(Qt::RightButton, CENTER_SCENE,      true);

	//SetMouseBinding(frameKeyboardModifiers | Qt::LeftButton,  ALIGN_FRAME,  true);
	//SetMouseBinding(frameKeyboardModifiers | Qt::RightButton, CENTER_FRAME, true);

	//// S p e c i f i c   d o u b l e   c l i c k s
	//SetMouseBinding(Qt::LeftButton,  RAP_FROM_PIXEL, true, Qt::RightButton);
	//SetMouseBinding(Qt::RightButton, RAP_IS_CENTER,  true, Qt::LeftButton);
	//SetMouseBinding(Qt::LeftButton,  ZOOM_ON_PIXEL,  true, Qt::MidButton);
	//SetMouseBinding(Qt::RightButton, ZOOM_TO_FIT,    true, Qt::MidButton);
}

QString View::ViewerImpl::MouseActionString(MouseAction ma)
{
	switch (ma)
	{
		case NO_MOUSE_ACTION : return QString::null;
		case ROTATE : return ("旋转");
		case ZOOM : return ("缩放");
		case TRANSLATE : return ("平移");
		case MOVE_FORWARD : return ("前进");
		case LOOK_AROUND : return ("环视");
		case MOVE_BACKWARD : return tr("后退", "MOVE_BACKWARD mouse action");
		//case ZOOM_ON_REGION : return ("区域缩放");
	}
	return QString::null;
}

QString View::ViewerImpl::ClickActionString(ClickAction ca)
{
	switch (ca)
	{
		case NO_CLICK_ACTION : return QString::null;
		case ZOOM_ON_PIXEL : return tr("Zooms on pixel", "ZOOM_ON_PIXEL click action");
		case ZOOM_TO_FIT : return tr("Zooms to fit scene", "ZOOM_TO_FIT click action");
		case SELECT : return tr("Selects", "SELECT click action");
		case RAP_FROM_PIXEL : return tr("Sets revolve around point", "RAP_FROM_PIXEL click action");
		case RAP_IS_CENTER : return tr("Resets revolve around point", "RAP_IS_CENTER click action");
		case CENTER_FRAME : return tr("Centers frame", "CENTER_FRAME click action");
		case CENTER_SCENE : return tr("Centers scene", "CENTER_SCENE click action");
		case SHOW_ENTIRE_SCENE : return tr("Shows entire scene", "SHOW_ENTIRE_SCENE click action");
		case ALIGN_FRAME : return tr("Aligns frame", "ALIGN_FRAME click action");
		case ALIGN_CAMERA : return tr("Aligns camera", "ALIGN_CAMERA click action");
	}
	return QString::null;
}

void View::ViewerImpl::SetMouseBinding(int state, MouseHandler handler, MouseAction action, bool withConstraint)
{
	if ((handler == FRAME) && ((action == MOVE_FORWARD) || (action == MOVE_BACKWARD) ||
		(action == ROLL) || (action == LOOK_AROUND) ||
		(action == ZOOM_ON_REGION)))
	{
#if QT_VERSION >= 0x040000
		qWarning("Cannot bind %s to FRAME", MouseActionString(action).toLatin1().constData());
#else
		qWarning("Cannot bind %s to FRAME", mouseActionString(action).latin1());
#endif
	}
	else
		if ((state & Qt::MouseButtonMask) == 0)
			qWarning("No mouse button specified in SetMouseBinding");
		else
		{
			MouseActionPrivate map;
			map.handler = handler;
			map.action = action;
			map.withConstraint = withConstraint;
			state = ConvertToKeyboardModifiers(state);

			m_mouseBinding.remove(state);

			if (action != NO_MOUSE_ACTION)
				m_mouseBinding.insert(state, map);

			ClickActionPrivate cap;
			cap.modifiers = Qt::KeyboardModifiers(state & Qt::KeyboardModifierMask);
			cap.button = Qt::MouseButtons(state & Qt::MouseButtonMask);
			cap.doubleClick = false;
			cap.buttonsBefore = Qt::NoButton;
			m_clickBinding.remove(cap);
		}
}

/*! Associates a ClickAction to any mouse buttons and keyboard modifiers combination.

The parameters should read: when the \p state mouse button(s) is (are) pressed (possibly with Alt,
Control or Shift modifiers or any combination of these), and possibly with a \p doubleClick,
perform \p action.

If \p buttonsBefore is specified (valid only when \p doubleClick is \c true), then this mouse
button(s) has to be pressed \e before the double click occurs in order to perform \p action.

The list of all possible ClickAction, some binding examples and default bindings are listed in the
<a href="../mouse.html">mouse page</a>. See also the SetMouseBinding() documentation.

See the <a href="../examples/keyboardAndMouse.html">keyboardAndMouse example</a> for an
illustration.

The binding is ignored if no mouse button is specified in \p state.

\note If you use Qt version 2 or 3, the \p buttonsBefore parameter is actually a Qt::ButtonState. */
void View::ViewerImpl::SetMouseBinding(int state, ClickAction action, bool doubleClick, Qt::MouseButtons buttonsBefore)
{
	if ((buttonsBefore != Qt::NoButton) && !doubleClick)
		qWarning("Buttons before is only meaningful when doubleClick is true in SetMouseBinding().");
	else
		if ((state & Qt::MouseButtonMask) == 0)
			qWarning("No mouse button specified in SetMouseBinding");
		else
		{
			ClickActionPrivate cap;
			state = ConvertToKeyboardModifiers(state);
			cap.modifiers = Qt::KeyboardModifiers(state & Qt::KeyboardModifierMask);
			cap.button = Qt::MouseButtons(state & Qt::MouseButtonMask);
			cap.doubleClick = doubleClick;
			cap.buttonsBefore = buttonsBefore;
			m_clickBinding.remove(cap);

			// #CONNECTION performClickAction comment on NO_CLICK_ACTION
			if (action != NO_CLICK_ACTION)
				m_clickBinding.insert(cap, action);

			if ((!doubleClick) && (buttonsBefore == Qt::NoButton))
				m_mouseBinding.remove(state);
		}
}

void View::ViewerImpl::HideMessage()
{
	m_isDisplayMessage = false;
	m_pInterface->Refresh();
}

void View::ViewerImpl::DrawText(int x, int y, const QString& text, const QFont& fnt/* =QFont */)
{
	m_pInterface->renderText(x, y, text, fnt);
}

void View::ViewerImpl::DisplayMessage(const QString& message, int delay/* =2000 */)
{
	m_message = message;
	m_isDisplayMessage = true;
#if QT_VERSION >= 0x040000
	// Was set to single shot in defaultConstructor.
	m_messageTimer.start(delay);
#else
	if (messageTimer_.isActive())
		messageTimer_.changeInterval(delay);
	else
		messageTimer_.start(delay, true);
#endif
		m_pInterface->Refresh();
}

void View::ViewerImpl::SetGridDrawnState(bool state)
{
	m_isGridDrawn = state;
}

/*! Draws a grid in the XY plane, centered on (0,0,0) (defined in the current coordinate system).

\p size (OpenGL units) and \p nbSubdivisions define its geometry. Set the \c GL_MODELVIEW matrix to
place and orientate the grid in 3D space (see the drawAxis() documentation).

The OpenGL state is not modified by this method. */
void View::ViewerImpl::DrawGrid(float size, int nbSubdivisions)
{
	GLboolean lighting;
	glGetBooleanv(GL_LIGHTING, &lighting);

	glDisable(GL_LIGHTING);
	glColor3ub(QColor(Qt::darkBlue).red(), QColor(Qt::darkBlue).green(), QColor(Qt::darkBlue).blue());
	glBegin(GL_LINES);
	for (int i=0; i<=nbSubdivisions; ++i)
	{
		const float pos = size*(2.0*i/nbSubdivisions-1.0);
		glVertex3f(pos + m_params.cameraPos.x, -size + m_params.cameraPos.y,  0);
		glVertex3f(pos + m_params.cameraPos.x, +size + m_params.cameraPos.y,  0);
		glVertex3f(-size + m_params.cameraPos.x, pos + m_params.cameraPos.y,  0);
		glVertex3f( size + m_params.cameraPos.x, pos + m_params.cameraPos.y,  0);
	}
	glEnd();

	if (lighting)
		glEnable(GL_LIGHTING);
}

void View::ViewerImpl::SnapshotToClipboard()
{
	QClipboard *cb = QApplication::clipboard();
	cb->setImage(FrameBufferSnapshot()); 
}

QImage View::ViewerImpl::FrameBufferSnapshot()
{
	// View must be on top of other windows.
	m_pInterface->makeCurrent();
	m_pInterface->raise();
	// Hack: Qt has problems if the frame buffer is grabbed after QFileDialog is displayed.
	// We grab the frame buffer before, even if it might be not necessary (vectorial rendering).
	// The problem could not be reproduced on a simple example to submit a Qt bug.
	// However, only grabs the backgroundImage in the eponym example. May come from the driver.
	return m_pInterface->grabFrameBuffer(true);
}

void View::ViewerImpl::FitScreenRegion(const QRect& rectangle)
{
	Point_3 vd = GetCurrentViewDirection();
	float aa = m_params.matrixRotated.data()[0];
	Point_3 trs = Point_3(m_params.matrixRotated.data()[12],m_params.matrixRotated.data()[13],m_params.matrixRotated.data()[14]);
	const float distToPlane = (m_params.cameraPos - trs).z;
	const QPoint center = rectangle.center();
}

void View::ViewerImpl::Pick(const QMouseEvent* event)
{
	Pick(event->pos());
}

void View::ViewerImpl::Pick(const QPoint& point)
{
	//开启捕捉
	//beginSelection(point);
	m_pInterface->DrawWithNames(point);

	//结束捕捉
	//endSelection(point);
	m_pInterface->PostPicking(point);
}

void View::ViewerImpl::SetHudtext(QString text)
{
	m_hudText = text;
}

DcGp::DcGpEntity* View::ViewerImpl::GetInnerRoot()
{
	return m_innerRoot;
}

void View::ViewerImpl::AddtoInnerRoot(DcGp::DcGpEntity* obj, bool noDependency /* = true */)
{
	assert(obj);

	if (m_innerRoot)
	{
		m_innerRoot->AddChild(obj, noDependency);
	}
}

void View::ViewerImpl::RemoveFormInnerRoot(DcGp::DcGpEntity* obj2D)
{
	if (m_innerRoot)
		m_innerRoot->RemoveChild(obj2D);
}

void View::ViewerImpl::AddManipulatorHandle(DCCore::GUIEventHandle* manipulator)
{
	m_manipulatorHandles.push_back(manipulator);
}

Camera* View::ViewerImpl::GetCamera() const
{
	return m_camera;
}

MatrixPiple View::ViewerImpl::GetMatrixPiple() const
{
	return m_matrixPiple;
}

void View::ViewerImpl::SetMatrixPiple(MatrixPiple mpType)
{
	m_matrixPiple = mpType;
}

void View::ViewerImpl::SetContinuousUpdate(bool continuous)
{
	m_continuousUpdate = continuous;
	if (continuous)
	{
		disconnect(&m_updateTimer, SIGNAL(timeout()), m_pInterface, SLOT(updateGL()));
		connect(&m_updateTimer, SIGNAL(timeout()), m_pInterface, SLOT(updateGL()));
		m_updateTimer.setSingleShot(false);
		m_updateTimer.setInterval(m_refreshRate);
		m_updateTimer.start(0);
	}
	else
	{
		disconnect(&m_updateTimer, SIGNAL(timeout()), m_pInterface, SLOT(updateGL()));
		m_updateTimer.stop();
	}
}

void View::ViewerImpl::SetRefreshRate( int msec )
{
	m_refreshRate = msec;
	m_updateTimer.setInterval(m_refreshRate);
}

int View::ViewerImpl::GetRefreshRate()
{
	return m_refreshRate;
}