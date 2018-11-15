#include "DCGui/Iwindow.h"

//Qt
#include "QApplication"
#include "QSplashScreen"
#include "QTime"
#include "QLabel"
#include "QTextCodec"
#include "QSettings"
#include <QtWidgets/QDesktopWidget>
#include "QRegExp"
#include "QDebug"
#include "QFile"

#include "DCGui/UI_IWindow.h"
#include "DCGui/ConfigParser.h"

using namespace DCGui;

IWindow::IWindow(QWidget *parent, Qt::WindowFlags flags)
	: QMainWindow(parent, flags)
	, m_pUI(nullptr)
{
	//QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GB2312"));
	//QTextCodec::setCodecForTr(QTextCodec::codecForName("GB2312"));

	m_org = "DCLW";
	m_appDir = QApplication::applicationDirPath() + "/";
}

IWindow::~IWindow()
{
	SaveSettings();

	delete m_pUI;
	m_pUI = nullptr;
}

int IWindow::GetRootMenuNum() const
{
	QRegExp re("^menu");
	int num = menuBar()->findChildren<QMenu* >(re).size();

	for (int i = 0; i != num; ++i)
	{
		qDebug() << menuBar()->findChildren<QMenu* >(re)[i]->objectName();
	}

	return num;
}

QAction* IWindow::GetAction(const int ID)
{
	if (ID < GetRootMenuNum())
	{
		QRegExp re("^menu");
		QMenu* menu = menuBar()->findChildren<QMenu* >(re)[ID];

		Q_ASSERT(menu);
		QAction* action = menu->menuAction();
		return action;
	}

	return nullptr;
}

bool IWindow::ConfigInit(QMainWindow* pInter)
{
	m_pUI = new UI_IWindow(pInter);

	//连接sqlite配置文件数据库，配置文件的数据库名称采用appname
	QString appname = QApplication::applicationName();
	QString databaseName = m_appDir + "Config/" + appname;
	bool cfgState = ConfigParser::ConnectDatabase(databaseName);

	if (cfgState)
	{
		ConfigStyleSheet();

		//1.添加splash相应
		ConfigSplash();

		//2.根据配置文件创建UI
		ConfigUI();
	}

	return cfgState;

	//QMetaObject::connectSlotsByName(pInter);
}

void IWindow::ConfigStyleSheet()
{
	//! 加载样式表
	//! 从数据库中读取样式表文件名
	QString fileNmae = QString("%1%2").arg(m_appDir).arg(ConfigParser::GetQssFileName());
	
	QFile qssFile(fileNmae);
	qssFile.open(QFile::ReadOnly);
	if (qssFile.isOpen())
	{
		QString qss = QLatin1String(qssFile.readAll());

		qApp->setStyleSheet(qss);

		qssFile.close();
	}
}

void IWindow::ConfigFinish(QMainWindow* pInter)
{
	QMetaObject::connectSlotsByName(pInter);

	LoadSettings();
}

QMenuBar* IWindow::GetMenuBar() const
{
	return m_pUI->menuBar;
}

void IWindow::ConfigSplash()
{
	//获取info
	int splashID = ConfigParser::GetSplashIDStarted();
	SplashInfo info = ConfigParser::GetSplashInfo(splashID);

	if (info.fileName.isEmpty())
	{
		return;
	}
	//创建启动画面()
	QSplashScreen* splash = new QSplashScreen();
	QTime time;
	time.start();
	int start = time.elapsed()/1000;

	//设置启动画文件支持qrc和全路径
	QString startFile;
	if (info.fileName.at(0) != QLatin1Char(':'))
	{
		startFile = QString("%1%2").arg(m_appDir).arg(info.fileName);
	}
	else
	{
		startFile = info.fileName;
	}

	splash->setPixmap(QPixmap(startFile));

	if (splashID == 1)
	{
		//软件中文名称全称
		QString title = ConfigParser::GetGlobalTitle(false);
		QLabel* labelFull = new QLabel(title, splash);
		labelFull->setGeometry(25,98,440,55);
		labelFull->setStyleSheet("color: rgb(255, 255, 255);font: 75 18pt '微软雅黑'");
		labelFull->setAttribute(Qt::WA_TranslucentBackground, true);

		//软件简称 + 版本号
		m_title = ConfigParser::GetSoftName(true);
		QLabel* label = new QLabel(m_title, splash);
		label->setGeometry(280,160,160,20);
		label->setStyleSheet("color: rgb(250, 255, 242);font: 12pt '楷体'");
		label->setAttribute(Qt::WA_TranslucentBackground, true);
	}
	else
	{
		m_title = ConfigParser::GetSoftName(true);
	}

	//显示启动画面()
	splash->show();
	//设置提示信息()ruan
	splash->showMessage(info.showInfo, Qt::AlignBottom | Qt::AlignRight, Qt::yellow);
	
	//启动界面显示一秒钟
	while (time.elapsed()/1000 - start < info.time)
	{
		splash->raise();
	}

	//删除启动画面
	delete splash;
	splash =nullptr;

	QCoreApplication::setOrganizationName(m_org);
	QCoreApplication::setApplicationName(ConfigParser::GetSoftName(false));
}

void IWindow::ConfigUI()
{
	//配置标题栏，title和 logo
	ConfigTitleBar();

	//配置菜单栏
	ConfigMenu();
}

void IWindow::ConfigTitleBar()
{
	QString bit;
#ifdef _WIN64
	bit = " [x64]";
#else
	bit = " [x86]";
#endif
	QString title = ConfigParser::GetGlobalTitle(true, false);
	setWindowTitle(title+bit);

	//配置logo  //:/logo/Resources/logo/DC.ico
	//判断logo中是否包含：（包含则调用qrc路径，不包含则调用组合后的绝对路径）
	QString logo = ConfigParser::GetLogoFile();
	QString logoFile;

	Q_ASSERT(!logo.isEmpty());
	if (logo.at(0) != QLatin1Char(':'))
	{
		logoFile = QString("%1%2").arg(m_appDir).arg(logo);
	}
	else
	{
		logoFile = logo;
	}

	setWindowIcon(QIcon(logoFile));
}

void IWindow::ConfigMenu()
{
	m_pUI->CreateMenuBySqlite();
}

QString IWindow::GetOrg()
{
	return m_org;
}

QString IWindow::GetTitle()
{
	return m_title;
}
//void IWindow::on_actionOpen_triggered()
//{
//	auto aa = 9;
//}

void IWindow::LoadSettings()
{
	QSettings settings(GetOrg(), GetTitle());

	//! 主窗口参数
	settings.beginGroup("AuxMainWindow");

	//! Qmainwindow方法
	bool aa = restoreState(settings.value("MainWindowState").toByteArray(), 0);

	//! qwidget方法
	resize( settings.value( "size", QSize(800, 600) ).toSize() );
	move( settings.value( "pos", QPoint(200, 200) ).toPoint() );

	bool fullScreen = settings.value("fullScreen",false).toBool();

	if (fullScreen)
		showFullScreen();

	settings.endGroup();


}

void IWindow::SaveSettings()
{
	QSettings settings(GetOrg(), GetTitle());

	settings.beginGroup("AuxMainWindow");

	settings.setValue( "MainWindowState",saveState(0) );

	settings.setValue( "size", size() );
	settings.setValue( "pos", pos() );

	settings.setValue( "fullScreen", isFullScreen() );

	// save the screen number for the splashsreeen ...
	if ( QApplication::desktop()->isVirtualDesktop() )
		settings.setValue( "screenNumber", QApplication::desktop()->screenNumber( pos() ) );
	else
		settings.setValue( "screenNumber", QApplication::desktop()->screenNumber(this) );

	settings.setValue( "screenNumber", QApplication::desktop()->screenNumber(this) );

	settings.endGroup();
}