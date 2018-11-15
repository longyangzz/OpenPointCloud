#ifndef DC_CONSOLE_COMMANDMANAGER_H
#define DC_CONSOLE_COMMANDMANAGER_H

#include "DCConsoleDLL.h"

//Boost
#include "boost/shared_ptr.hpp"

//Qt
#include "QMap"
#include "QStringList"
class QString;

namespace DCCore
{
	//命令
	class Command;

	typedef boost::shared_ptr<Command> CommandPtr;

	//命令生成器
	typedef CommandPtr (*CommandGenerator)();
}

namespace DCConsole
{
	//命令管理器
	class DC_CONSOLE_API CommandManager
	{
	public:
		//初始化命令管理器
		static CommandManager* Initialize();
		//卸载命令管理器
		static void Uninitalize();
		//获取命令管理器实体
		static CommandManager* Instance();

		//注册命令
		void RegisterCommand(const QString& strCmdName,
			const DCCore::CommandGenerator& generator);
		void RegisterCommands(QMap<QString, DCCore::CommandGenerator> generators);

		//生成命令
		DCCore::CommandPtr GenerateCommand(const QString& strCmd);

		//获取最近使用的命令
		QStringList GetRecentCommands() const;

		//向上查找最近命令
		QString SearchUpRecentCommand();
		//向下查找最近命令
		QString SearchDownRecentCommand();

	private:
		CommandManager();
		~CommandManager();

	private:
		static CommandManager* s_pInstance;
		class CommandManagerImpl;
		CommandManagerImpl* m_pImpl;
	};
}

#endif