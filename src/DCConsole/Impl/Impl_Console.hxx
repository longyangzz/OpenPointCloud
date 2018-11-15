#ifndef DC_CONSOLE_IMPL_CONSOLE_H
#define DC_CONSOLE_IMPL_CONSOLE_H

//Boost
#include "boost/shared_ptr.hpp"

//Qt
#include "QtCore/QObject"
#include "QtCore/QDir"
#include "QtCore/QList"

//DCCore
#include "DCCore/Logger.h"

//Self
#include "DCConsole/Console.h"

namespace DCCore
{
	//交互命令
	class InteractiveCommand;
	typedef boost::shared_ptr<InteractiveCommand> InteractiveCommandPtr;
}

namespace DCConsole
{
	//控制台UI
	class UI_Console;

	

	//命令管理器
	class CommandManager;

	//日志管理器
	class LogManager : public QObject,
		public DCCore::Logger
	{
		Q_OBJECT

	public:
		LogManager(QObject* pParent);
		~LogManager(){}

signals:
		//记录日志
		void Logged(const QString& strLog);

	private:
		//显示信息
		virtual void ShowMessage(const QString& msg, LogLevel level = eMessageLog);
	};

	//控制台实现类
	class Console::ConsoleImpl : public QObject
	{
		Q_OBJECT
	public:
		ConsoleImpl(Console* pInterface);
		~ConsoleImpl();

		//获取正在执行的命令
		DCCore::CommandPtr GetRunningCommand() const;

		public slots:
			//显示消息
			void Message(const QString& strMsg);

			//取消命令
			void CancelCommand();

	private:
		//初始化UI
		void InitUI();

		//创建信号与槽的连接
		void CreateConnections();

		//获取提示信息
		QString Prompt();
		//输出命令信息
		void OutputCommandLog();

		//清除当前命令
		void ClearCurrentCommand();

		public slots:
			//执行命令
			void ExecuteCommand(const QString& strCmd);

		private slots:
			//完成命令
			void FinishCommand();

			//改变提示信息
			void ChangedPrompt(const QString& strPrompt);

			//移出命令行焦点
			void RemoveCommandLineFocus();
			//进入命令行焦点
			void EnterCommandLineFocus();

			//向上查找最近命令
			void SearchUp();
			//向下查找最近命令
			void SearchDown();

			//显示最近命令菜单
			void ShowRecentCommandMenu();
			//执行最近命令
			void ExecuteRecentCommand();

	protected:
		//事件过滤器
		virtual bool eventFilter(QObject* pObject, QEvent* event);

	private:
		//接口
		Console* m_pInterface;
		//UI
		UI_Console* m_pUi;
		//日志管理器
		LogManager* m_pLogManager;

		//当前正在执行的命令
		DCCore::InteractiveCommandPtr m_pCurrentCommand;
		//命令管理器
		CommandManager* m_pCommandManager;
		//默认提示信息
		QString m_defaultPrompt;
		//当前命令提示
		QString m_currentPrompt;
		//当前参数
		QString m_currentParam;
	};


	//参数类实现
	class Parameters::ParametersImpl
	{
	public:
		ParametersImpl(Parameters* pInterface);

		//设置命令文件搜索路径
		void AppendCommandDir(const QDir& dirCmd);
		//移除命令文件搜索路径
		void RemoveCommandDir(const QDir& dirCmd);
		//命令搜索路径
		QList<QDir> CommandDirs();

	private:
		//接口
		Parameters* m_pInterface;
		//命令搜索路径
		QList<QDir> m_dirCommands;
	};
}

#endif