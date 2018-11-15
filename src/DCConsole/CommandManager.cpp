#include "DCConsole/CommandManager.h"

//C++
#include <algorithm>

//Qt
#include "QString"
#include "QtAlgorithms"

//DCCore
#include "DCCore/Command.h"


#include "Impl/Impl_CommandManager.hxx"

using namespace DCConsole;

CommandManager* CommandManager::s_pInstance = nullptr;

//初始化命令管理器
CommandManager* CommandManager::Initialize()
{
	if (!s_pInstance)
	{
		s_pInstance = new CommandManager;
	}
	return s_pInstance;
}

//卸载命令管理器
void CommandManager::Uninitalize()
{
	if (s_pInstance)
	{
		delete s_pInstance;
		s_pInstance = nullptr;
	}
}

//获取命令管理器实体
CommandManager* CommandManager::Instance()
{
	return s_pInstance;
}

CommandManager::CommandManager()
{
	m_pImpl = new CommandManagerImpl(this);
}

CommandManager::~CommandManager()
{
	delete m_pImpl;;
}

//注册命令
void CommandManager::RegisterCommand(const QString& strCmdName,
	const DCCore::CommandGenerator& generator)
{
	m_pImpl->RegisterCommand(strCmdName, generator);
}

void CommandManager::RegisterCommands(QMap<QString, DCCore::CommandGenerator> generators)
{
	m_pImpl->RegisterCommands(generators);
}

//生成命令
DCCore::CommandPtr CommandManager::GenerateCommand(const QString& strCmd)
{
	return m_pImpl->GenerateCommand(strCmd);
}

//获取最近使用的命令
QStringList CommandManager::GetRecentCommands() const
{
	return m_pImpl->GetRecentCommands();
}

//向上查找最近命令
QString CommandManager::SearchUpRecentCommand()
{
	return m_pImpl->SearchUpRecentCommand();
}

//向下查找最近命令
QString CommandManager::SearchDownRecentCommand()
{
	return m_pImpl->SearchDownRecentCommand();
}