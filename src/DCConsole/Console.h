#ifndef DC_CONSOLE_CONSOLE_H
#define DC_CONSOLE_CONSOLE_H

#include "DCConsoleDLL.h"

//boost
#include "boost/shared_ptr.hpp"

//Qt
#include "QtCore/QList"

#include "QtWidgets/QDockWidget"
class QDir;

namespace DCCore
{
	//命令
	class Command;

	//命令
	typedef boost::shared_ptr<Command> CommandPtr;
}


namespace DCConsole
{
	

	//控制台
	class DC_CONSOLE_API Console : public QDockWidget
	{
		Q_OBJECT

	public:
		//初始化控制台
		static Console* Initialize();
		//卸载控制台
		static void Uninitialize();
		//获取控制台实体
		static Console* Instance();

		//执行命令
		void ExecuteCommand(const QString& strCmd);

		//获取正在执行的命令
		DCCore::CommandPtr GetRunningCommand() const;

		//取消命令
		void CancelRunningCommand();

	private:
		explicit Console(QWidget* pParent = nullptr);
		virtual ~Console();

	private:
		static Console* s_pInstance;

		class ConsoleImpl;
		ConsoleImpl* m_pImpl;
	};

	//参数类
	class DC_CONSOLE_API Parameters
	{
	public:
		//设置命令文件搜索路径
		static void AppendCommandDir(const QDir& dirCmd);
		//移除命令文件搜索路径
		static void RemoveCommandDir(const QDir& dirCmd);
		//命令搜索路径
		static QList<QDir> CommandDirs();

	protected:
		Parameters();
		~Parameters();

	private:
		static Parameters s_instance;

		class ParametersImpl;
		ParametersImpl* m_pImpl;
	};
}

#endif