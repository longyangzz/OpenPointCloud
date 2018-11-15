#include "Impl_Console.hxx"
using namespace DCConsole;

//Qt
#include "QtWidgets/QApplication"
#include "QtGui/QTextDocument"
#include "QtWidgets/QMenu"
#include "QtWidgets/QAction"

#include "DCCore/Command.h"
//Self
#include "UI_Console.hxx"
#include "DCConsole/CommandManager.h"


/***************************************日志管理器*************************************************/
LogManager::LogManager(QObject* pParent)
	: QObject(pParent)
{
	DCCore::Logger::Register(this);
}

//显示信息
void LogManager::ShowMessage(const QString& msg, LogLevel level /* = eLogMessage */)
{
	//如果消息内容为空，则不输出任何信息
	if (msg.isEmpty())
	{
		return;
	}

	//日志信息格式
	QString strFormat = tr("<html><head/><body><span style=' "
					"color: rgb(%1,%2,%3); font-size: 20px;'>%4</span></body></html>");
	//日志信息
	QString strLog;
	//根据信息类型，设置文本的格式
	switch (level)
	{
	case eMessageLog:
		strLog = strFormat.arg(255).arg(255).arg(255).arg(msg);
		break;
	case eWarningLog:
		strLog = strFormat.arg(255).arg(165).arg(0).arg(msg);
		break;
	case eErrorLog:
		strLog = strFormat.arg(255).arg(0).arg(0).arg(msg);
		break;
	default:
		break;
	}

	//记录日志
	emit Logged(strLog);
}
/**************************************************控制台实现类*********************************/
Console::ConsoleImpl
	::ConsoleImpl(Console* pInterface)
	: m_pInterface(pInterface)
	, m_pUi(nullptr)
	, m_pCurrentCommand(nullptr)
	, m_pCommandManager(nullptr)
{
	//初始化UI
	InitUI();

	//初始化日志管理器
	m_pLogManager = new LogManager(this);

	//初始化命令管理器
	m_pCommandManager = CommandManager::Initialize();

	//连接信号与槽
	CreateConnections();
}

Console::ConsoleImpl
	::~ConsoleImpl()
{
	//卸载命令管理器
	CommandManager::Uninitalize();

	delete m_pLogManager;
	m_pLogManager = nullptr;
	//删除UI
	delete m_pUi;
}

//显示消息
void Console::ConsoleImpl
	::Message(const QString& strMsg)
{
	m_pUi->msgBrower->append(strMsg);
}

DCCore::CommandPtr Console::ConsoleImpl
	::GetRunningCommand() const
{
	return m_pCurrentCommand;
}

//初始化UI
void Console::ConsoleImpl
	::InitUI()
{
	//创建中心窗体
	QWidget* pCenterWidget = new QWidget;

	//加载UI
	m_pUi = new UI_Console;
	m_pUi->SetupUi(pCenterWidget);

	//设置中心窗体
	m_pInterface->setWidget(pCenterWidget);

	//设置标题位置为垂直方向
	//setFeatures(QDockWidget::DockWidgetVerticalTitleBar);
	//设置默认命令提示
	m_defaultPrompt = QString("<html><head/><body><p>"
		"<span style='color:#AAAAAA;'>%1</span></p></body></html>")
		.arg(tr("Input Command"));
	m_pUi->cmdPrompt->setText(m_defaultPrompt);

	//安装事件过滤器
	{
		//命令提示事件过滤器
		m_pUi->cmdPrompt->installEventFilter(this);
	}
}

//创建信号与槽的连接
void Console::ConsoleImpl
	::CreateConnections()
{
	//记录日志
	connect(m_pLogManager, SIGNAL(Logged(const QString&)),
		this, SLOT(Message(const QString&)));

	//命令行执行命令
	connect(m_pUi->cmdLine, SIGNAL(Execute(const QString&)),
		this, SLOT(ExecuteCommand(const QString&)));
	//取消命令
	connect(m_pUi->cmdLine, SIGNAL(Cancel()),
		this, SLOT(CancelCommand()));

	//向上查找
	connect(m_pUi->cmdLine, SIGNAL(SearchUp()),
		this, SLOT(SearchUp()));
	//向下查找
	connect(m_pUi->cmdLine, SIGNAL(SearchDown()),
		this, SLOT(SearchDown()));
	

	//命令行进入焦点
	connect(m_pUi->cmdLine, SIGNAL(FocusEntered()),
		this, SLOT(EnterCommandLineFocus()));
	//命令行移出焦点
	connect(m_pUi->cmdLine, SIGNAL(FocusRemoved()),
		this, SLOT(RemoveCommandLineFocus()));

	//显示最近命令菜单
	connect(m_pUi->tbtnIcon, SIGNAL(clicked()),
		this, SLOT(ShowRecentCommandMenu()));

	//! 删除内容
	connect(m_pUi->tbtnClear, SIGNAL(clicked()),
		m_pUi->msgBrower, SLOT(ClearMessage()));
}

//执行命令
void Console::ConsoleImpl
	::ExecuteCommand(const QString& strCmd)
{
	if (m_pCurrentCommand)
	{
		m_pCurrentCommand->ChangedCommandParameter(strCmd);
	}
	else
	{
		if (strCmd.isEmpty())
		{
			//获取命令管理器
			CommandManager* pCmdManager = CommandManager::Instance();
			//获取最近命令
			QString strRecentCmd = pCmdManager->SearchUpRecentCommand();
			if (!strRecentCmd.isEmpty())
			{
				ExecuteCommand(strRecentCmd);
				return;
			}
		}
		//输出命令
		Message(tr("Command:").append(strCmd));
		//根据输入的内容查找命令
		DCCore::CommandPtr pCmd = m_pCommandManager->GenerateCommand(strCmd);

		//如果命令不存在，则输出提示信息
		if (!pCmd)
		{
			Message(tr("Unknown Command \"%1\". Press F1 to view helper.").arg(strCmd));
		}
		else
		{
			//检测命令是否为交互命令，若是交互命令，则设置当前命令
			//并连接命令提示信息变更的信号
			DCCore::InteractiveCommandPtr pInterCmd = boost::dynamic_pointer_cast<DCCore::InteractiveCommand>(pCmd);
			if (!m_pCurrentCommand && pInterCmd)
			{
				m_pCurrentCommand = pInterCmd;
				//连接提示信息变化的信号与槽
				connect(m_pCurrentCommand.get(), SIGNAL(PromptChanged(const QString&)),
					this, SLOT(ChangedPrompt(const QString&)));
				//连接参数变化的信号与槽
				connect(m_pUi->cmdPrompt, SIGNAL(linkActivated(const QString&)),
					this, SLOT(ExecuteCommand(const QString&)));
				//连接信息传递的信号与槽
				connect(m_pCurrentCommand.get(), SIGNAL(Message(const QString&)),
					this, SLOT(Message(const QString&)));
				//连接命令完成的信号
				connect(m_pCurrentCommand.get(), SIGNAL(Done()),
					this, SLOT(FinishCommand()));

			}

			//执行命令
			pCmd->Execute();
		}
	}
}

//改变提示信息
void Console::ConsoleImpl
	::ChangedPrompt(const QString& strPrompt)
{
	m_pUi->cmdPrompt->setText(strPrompt);
	m_currentPrompt = strPrompt;
	//设置命令行焦点
	m_pUi->cmdLine->setFocus();
}

//输出命令信息
void Console::ConsoleImpl
	::OutputCommandLog()
{
	if (m_currentPrompt.isEmpty())
	{
		return;
	}

	//获取提示信息
	QString strMsg = m_currentPrompt;

	//检测提示信息的文本格式
	Qt::TextFormat format = m_pUi->cmdPrompt->textFormat();
	
	if (format == Qt::RichText)
	{
		//通过QTextDocument将html转为纯文本
		QTextDocument doc;
		doc.setHtml(strMsg);
		strMsg = doc.toPlainText();
	}

	//移除提示信息中的命令名称
	strMsg.remove(m_pCurrentCommand->GetGlobalName());

	//去除额外的空格
	strMsg = strMsg.simplified();

	//添加输入的参数并发送信息
	Message(strMsg.append(m_currentParam));
}

//移出命令行焦点
void Console::ConsoleImpl
	::RemoveCommandLineFocus()
{
	//如果命令提示为空，则设置命令提示
	if (m_pUi->cmdPrompt->text().isEmpty()
		&& m_pUi->cmdLine->text().isEmpty())
	{
		m_pUi->cmdPrompt->setText(m_defaultPrompt);
	}
}

//进入命令行焦点
void Console::ConsoleImpl
	::EnterCommandLineFocus()
{
	//如果当前执行命令不存在，清空命令提示
	if (!m_pCurrentCommand)
	{
		m_pUi->cmdPrompt->clear();

		//设置命令行焦点
		m_pUi->cmdLine->setFocus();
	}
}

//取消命令
void Console::ConsoleImpl
	::CancelCommand()
{
	//如果存在当前命令，则取消
	if (m_pCurrentCommand)
	{
		//显示命令信息
		Message(Prompt().append(tr("*Cancel*")));

		//取消命令
		m_pCurrentCommand->Cancel();
		//清除当前命令
		ClearCurrentCommand();
	}
}

//获取提示信息
QString Console::ConsoleImpl
	::Prompt()
{
	//获取提示信息
	QString strMsg = m_pUi->cmdPrompt->text();

	//通过QTextDocument将html转为纯文本
	QTextDocument doc;
	doc.setHtml(strMsg);
	strMsg = doc.toPlainText();

	//移除提示信息中的命令名称
	strMsg.remove(m_pCurrentCommand->GetGlobalName());

	//去除额外的空格
	strMsg = strMsg.simplified();

	return strMsg;
}

//清除当前命令
void Console::ConsoleImpl
	::ClearCurrentCommand()
{
	//如果当前命令存在，则取消相关的信号槽
	if (m_pCurrentCommand)
	{
		//取消提示信息变化的信号与槽
		disconnect(m_pCurrentCommand.get(), SIGNAL(PromptChanged(const QString&)),
			this, SLOT(ChangedPrompt(const QString&)));
		//取消参数变化的信号与槽
		disconnect(m_pUi->cmdPrompt, SIGNAL(linkActivated(const QString&)),
			this, SLOT(ExecuteCommand(const QString&)));
		//取消信息传递的信号与槽
		disconnect(m_pCurrentCommand.get(), SIGNAL(Message(const QString&)),
			this, SLOT(Message(const QString&)));

		m_pCurrentCommand = nullptr;
	}
}

//事件过滤器
bool Console::ConsoleImpl
	::eventFilter(QObject* pObject, QEvent* event)
{
	//判断命令提示框
	if (pObject == m_pUi->cmdPrompt)
	{
		if (event->type() == QEvent::MouseButtonPress)
		{
			//进入命令行焦点
			EnterCommandLineFocus();
		}
	}

	return QObject::eventFilter(pObject, event);
}

//向上查找最近命令
void Console::ConsoleImpl
	::SearchUp()
{
	//如果存在正在执行的程序则不执行后续操作
	if (m_pCurrentCommand)
	{
		return;
	}

	//获取命令管理器
	CommandManager* pCmdManager = CommandManager::Instance();

	//设置选择的命令
	m_pUi->cmdLine->SetCommand(pCmdManager->SearchUpRecentCommand());
}

//向下查找最近命令
void Console::ConsoleImpl
	::SearchDown()
{
	//如果存在正在执行的程序则不执行后续操作
	if (m_pCurrentCommand)
	{
		return;
	}

	//获取命令管理器
	CommandManager* pCmdManager = CommandManager::Instance();

	//设置选择的命令
	m_pUi->cmdLine->SetCommand(pCmdManager->SearchDownRecentCommand());
}

//显示最近命令菜单
void Console::ConsoleImpl
	::ShowRecentCommandMenu()
{
	//获取命令管理器
	CommandManager* pCmdManager = CommandManager::Instance();
	//获取最近命令列表
	QStringList listCmd = pCmdManager->GetRecentCommands();

	//创建菜单
	QMenu menuRecentCommands;
	//菜单高度
	int height = 0;
	for (auto iter = listCmd.constBegin();
		iter != listCmd.constEnd(); ++iter)
	{
		QAction* pAction = menuRecentCommands.addAction(*iter);
		height += 20;
		connect(pAction, SIGNAL(triggered()),
			this, SLOT(ExecuteRecentCommand()));
	}

	//菜单位置
	QPoint position = m_pUi->tbtnIcon->pos();
	position = m_pInterface->mapToGlobal(position);
	//获取菜单的高度
	//int height = menuRecentCommands.height();
	position.setY(position.y() - height);
	//执行菜单
	menuRecentCommands.exec(position);
}

//执行最近命令
void Console::ConsoleImpl
	::ExecuteRecentCommand()
{
	//将信号发送者转为QAction
	QAction* pActionSender = dynamic_cast<QAction*>(sender());

	//获取命令
	QString strCmd = pActionSender->text();

	//执行命令
	ExecuteCommand(strCmd);
}
//完成命令
void Console::ConsoleImpl
	::FinishCommand()
{
	ClearCurrentCommand();
}






/******************************************Parameters********************************/
Parameters::ParametersImpl
	::ParametersImpl(Parameters* pInterface)
	: m_pInterface(pInterface)
{
	//初始化命令搜索路径
	{
		
	}
}

//设置命令文件搜索路径
void Parameters::ParametersImpl
	::AppendCommandDir(const QDir& dirCmd)
{
	//如果路径不存在，则追加路径，否则不执行任何操作
	if (!m_dirCommands.contains(dirCmd))
	{
		m_dirCommands.append(dirCmd);
	}
	else
	{
		return;
	}
}

//移除命令文件搜索路径
void Parameters::ParametersImpl
	::RemoveCommandDir(const QDir& dirCmd)
{
	m_dirCommands.removeOne(dirCmd);
}

//命令搜索路径
QList<QDir> Parameters::ParametersImpl
	::CommandDirs()
{
	return m_dirCommands;
}