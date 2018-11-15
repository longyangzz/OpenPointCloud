#ifndef DCCORE_COMMAND_H
#define DCCORE_COMMAND_H

//Boost
#include "boost/shared_ptr.hpp"

//Qt
#include "QObject"
#include "QString"

namespace DCCore
{
	//命令
	class Command : public QObject
	{
	public:
		//执行命令
		virtual void Execute() = 0;
		//撤销命令
		virtual void Undo() = 0;
		//重做命令
		virtual void Redo() = 0;

		//获取命令全局名称(保证唯一性)
		virtual QString GetGlobalName() const = 0;

		//获取设计者信息
		virtual QString GetDesigner() const = 0;

		//获取描述信息
		virtual QString GetDescription() const = 0;

	protected:
		Command(){}
	};
	typedef boost::shared_ptr<Command> CommandPtr;

	//命令生成器
	typedef CommandPtr (*CommandGenerator)();

	//交互命令
	class InteractiveCommand : public Command
	{
	public:
		//命令参数发生变化
		virtual void ChangedCommandParameter(const QString& strState) = 0;

		//取消命令
		virtual void Cancel() = 0;

	protected:
		explicit InteractiveCommand(){}
	};
	typedef boost::shared_ptr<InteractiveCommand> InteractiveCommandPtr;
}

#endif