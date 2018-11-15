#ifndef DC_CONSOLE_IMPL_COMMANDMANAGER_H
#define DC_CONSOLE_IMPL_COMMANDMANAGER_H

//Qt
#include "QtCore/QObject"
#include "QtCore/QMap"
#include "QtCore/QList"
class QString;

#include "DCCore/Command.h"
#include "DCConsole/CommandManager.h"

namespace DCConsole
{
	//命令管理器实现类
	class CommandManager::CommandManagerImpl : public QObject
	{
		Q_OBJECT

	public:
		explicit CommandManagerImpl(CommandManager* pInterface);
		~CommandManagerImpl();

		//注册命令
		void RegisterCommand(const QString& strCmdName,
			const DCCore::CommandGenerator& generator);
		void RegisterCommands(QMap<QString, DCCore::CommandGenerator> generators);

		//查找命令是否以注册
		bool IsRegistered(const QString& strCmdName) const;

		//生成命令
		DCCore::CommandPtr GenerateCommand(const QString& strCmd);

		//获取最近使用的命令
		QStringList GetRecentCommands() const;

		//向上查找最近命令
		QString SearchUpRecentCommand();
		//向下查找最近命令
		QString SearchDownRecentCommand();

	private:
		//记录命令
		void RecordCommand(DCCore::CommandPtr pCmd);

		//加载命令模块
		void LoadCommands();

	private:
		//接口
		CommandManager* m_pInterface;
		//所有命令
		QMap<QString, DCCore::CommandGenerator> m_commandGenerators;
		//命令栈
		QList<DCCore::CommandPtr> m_cmdStack;
		//最近使用的命令（记录五种最近执行的命令）
		QStringList m_recentCommands;

		//命令栈最大容量
		static const unsigned c_cmdStackSize = 5;
		//历史命令数据
		static const unsigned c_recentCmdCount = 10;
		//当前最近命令的序号
		int m_currentRecentCmdIndex;
	};

	//输出注册的命令
	class OutputRegisteredCommand : public DCCore::Command
	{
		Q_OBJECT
	public:
		//命令生成器
		static DCCore::CommandPtr Generate();

		//获取命令全局名称(保证唯一性)
		static QString GlobalName();
		//执行命令
		virtual void Execute();
		//撤销命令
		virtual void Undo(){}
		//重做命令
		virtual void Redo(){}

		//获取命令全局名称(保证唯一性)
		virtual QString GetGlobalName() const;

		//获取设计者信息
		virtual QString GetDesigner() const;

		//获取描述信息
		virtual QString GetDescription() const;

	private:
		//将所有命令保存到指定文件中
		void SaveCommands(const QString& strFileName,
			QMap<QString, DCCore::CommandGenerator> commands);

	public:
		//注册的所有命令
		static QMap<QString, DCCore::CommandGenerator>* s_commands;
	};
	typedef boost::shared_ptr<OutputRegisteredCommand> OutputRegisteredCommandPtr;
}
#endif