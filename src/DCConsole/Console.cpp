#include "DCConsole/Console.h"
#include "DCCore/DCConfig.h"

using namespace DCConsole;

//self
#include "Impl/Impl_Console.hxx"

Console* Console::s_pInstance = nullptr;

//初始化控制台
Console* Console::Initialize()
{
	if (!s_pInstance)
	{
		s_pInstance = new Console;
	}

	return s_pInstance;
}

//卸载控制台
void Console::Uninitialize()
{
	delete s_pInstance;
	s_pInstance = nullptr;
}

//获取控制台实体
Console* Console::Instance()
{
	return s_pInstance;
}


Console::Console(QWidget* pParent /* = nullptr */)
	: QDockWidget(pParent)
{
	m_pImpl = new ConsoleImpl(this);
	setWindowTitle(("控制台"));
	setObjectName("Console");
}

Console::~Console()
{
	delete m_pImpl;
}

//执行命令
void Console::ExecuteCommand(const QString& strCmd)
{
	m_pImpl->ExecuteCommand(strCmd);
}

//获取正在执行的命令
DCCore::CommandPtr Console::GetRunningCommand() const
{
	return m_pImpl->GetRunningCommand();
}

//取消命令
void Console::CancelRunningCommand()
{
	m_pImpl->CancelCommand();
}






/********************************Parameters***********************************/
Parameters Parameters::s_instance;

Parameters::Parameters()
{
	m_pImpl = new ParametersImpl(this);
}

Parameters::~Parameters()
{
	delete m_pImpl;
}

//设置命令文件搜索路径
void Parameters::AppendCommandDir(const QDir& dirCmd)
{
	s_instance.m_pImpl->AppendCommandDir(dirCmd);
}

//移除命令文件搜索路径
void Parameters::RemoveCommandDir(const QDir& dirCmd)
{
	s_instance.m_pImpl->RemoveCommandDir(dirCmd);
}

//命令搜索路径
QList<QDir> Parameters::CommandDirs()
{
	return s_instance.m_pImpl->CommandDirs();
}