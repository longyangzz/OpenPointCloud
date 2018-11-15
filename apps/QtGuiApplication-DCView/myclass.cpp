#include "myclass.h"

//Qt
#include "QStatusBar"
#include "QMdiArea"
#include "QGridLayout"
#include "QApplication"
#include "QDir"

//qglviewer

//DCConsole
#include "DCConsole/Console.h"

//DCCore
#include "DCCore/IoPlugin.h"
#include "DCCore/IoInterface.h"
#include "DCCore/DCConfig.h"

#include "DCView/Viewer.h"
#include "Mdiarea.h"

//! DCGa
#include "DCGa/TrackballManipulator.h"
#include "DCGa/FlyManipulator.h"


MyClass::MyClass(QWidget *parent, Qt::WindowFlags flags)
	: DCGui::AuxMainWindow(parent, flags)
{
	bool status = ConfigInit(this);

	if (status)
	{
		//! 管理模块
		//MPMg::Manager* manager = MPMg::Manager::Instance();
		//manager->CreateUI(this);

		//信号槽链接
		//connect(manager, SIGNAL(SelectionChanged(const QVector<DcGp::DcGpEntity*>&)), this, SLOT(RefreshAll()));
		//connect(manager, SIGNAL(SelectionChanged(const QVector<DcGp::DcGpEntity*>&)), this, SIGNAL( HandlingEntitiesChanged(const QVector<DcGp::DcGpEntity*>&)) );

		//控制台初始化
		DCConsole::Console* pConsole = DCConsole::Console::Initialize();
		//设置控制台位置
		pConsole->setParent(this);
		addDockWidget(Qt::BottomDockWidgetArea, pConsole);

		//初始化多窗口管理
		QGridLayout* gridLayout = new QGridLayout(centralWidget());
		m_pMdiArea = new MdiArea(centralWidget());
		m_pMdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
		m_pMdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
		gridLayout->addWidget(m_pMdiArea, 0, 0, 1, 1);

		//! 激活多窗口的拖放操作
		m_pMdiArea->setAcceptDrops(true);
		m_pMdiArea->setMainWindow(this);

		connect(m_pMdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(ChangedCurrentViewer()));

		//状态栏
		statusBar()->showMessage(("就绪"));

		LoadModules();
	}

	ConfigFinish(this);

	MPViewer::Viewer* pNewViewer = new MPViewer::Viewer(this);
	//! 安装漫游器
	pNewViewer->AddManipulatorHandle(new DCGa::TrackballManipulator("TrackBall"));
	pNewViewer->AddManipulatorHandle(new DCGa::FlyManipulator("Fly"));
	pNewViewer->showMaximized();
	
	//pNewViewer->setAttribute(Qt::WA_DeleteOnClose);


	//QGridLayout* gridLayout = new QGridLayout(centralWidget());
	//gridLayout->addWidget(pNewViewer, 0, 0, 1, 1);
	m_pMdiArea->addSubWindow(pNewViewer);
	pNewViewer->showMaximized();
	loadFile("D:\\data\\TestData\\LOUTI-5.txt", pNewViewer);
}

MyClass::~MyClass()
{

}

int MyClass::loadFile(const QString &fileName, MPViewer::Viewer* pNewViewer)
{
	//MPViewer::Viewer* pNewViewer = new MPViewer::Viewer(this);
	//m_pMdiArea->addSubWindow(pNewViewer);

	////最大化显示
	//pNewViewer->showMaximized();
	//activeMdiChild();
	QString extension = QFileInfo(fileName).suffix();
	//根据扩展名，遍历IO文件夹中的插件，选择合适的io解析器解析文件
	QDir dir(DCCore::GetIOPluginPath());

	DCCore::IoPlugin* currentIo = 0;

	bool hasPlugin = DCCore::GetIOPlugin(dir, extension, currentIo);
	if (!hasPlugin || !currentIo)
	{
		QApplication::restoreOverrideCursor();
		return 0;
	}

	double coordinatesShift[3] = {0,0,0};
	DcGp::DcGpEntity* entity = currentIo->LoadFile(fileName, 0, true, 0, coordinatesShift, nullptr);

	if (pNewViewer)
	{
		//场景对象，添加实体对象
		pNewViewer->SetSceneRoot(entity);
	}

	//计算边界盒，更新相机参数
	//pNewViewer->ShowAllScene();

	return 0;
}

QWidget* MyClass::activeMdiChild()
{
	if (QMdiSubWindow *activeSubWindow = m_pMdiArea->activeSubWindow())
		return qobject_cast<QWidget *>(activeSubWindow->widget());

	return 0;
}

int MyClass::addFile(MPViewer::Viewer*viewer, const QString &fileName)
{
	QApplication::setOverrideCursor(Qt::WaitCursor);

	
	QApplication::restoreOverrideCursor();
	return 0;
}