#include "Impl_CommandManager.hxx"
using namespace DCConsole;

//Qt
#include "QtCore/QDir"
#include "QtCore/QList"
#include "QtCore/QPluginLoader"

#include "QtWidgets/QApplication"
#include "QtWidgets/QMessageBox"
#include "QtWidgets/QFileDialog"


//Self
#include "DCConsole/Console.h"

CommandManager::CommandManagerImpl
	::CommandManagerImpl(CommandManager* pInterface)
	: m_pInterface(pInterface)
{
	OutputRegisteredCommand::s_commands = &m_commandGenerators;
	//注册命令
	RegisterCommand(OutputRegisteredCommand::GlobalName(), 
		OutputRegisteredCommand::Generate);

	//加载命令
	//LoadCommands();
}

//析构函数
CommandManager::CommandManagerImpl
	::~CommandManagerImpl()
{
	OutputRegisteredCommand::s_commands = nullptr;
}

//查找命令是否以注册
bool CommandManager::CommandManagerImpl
	::IsRegistered(const QString& strCmdName) const
{
	return m_commandGenerators.contains(strCmdName);
}

//注册命令
void CommandManager::CommandManagerImpl
	::RegisterCommand(const QString& strCmdName,
	const DCCore::CommandGenerator& generator)
{
	//查找是否已注册
	//若已注册，则返回false
	//否则注册
	if (IsRegistered(strCmdName))
	{
		QMessageBox::warning(QApplication::activeWindow(),
			tr("Warning"), tr("Command \"%1\" already exists!").arg(strCmdName));
	}
	else
	{
		m_commandGenerators.insert(strCmdName, generator);
	}
}
void CommandManager::CommandManagerImpl
	::RegisterCommands(QMap<QString, DCCore::CommandGenerator> generators)
{
	if (generators.isEmpty())
	{
		return;
	}
	for (auto iter = generators.constBegin();
		iter != generators.constEnd(); ++iter)
	{
		RegisterCommand(iter.key(), iter.value());
	}
}

//查找命令
DCCore::CommandPtr CommandManager::CommandManagerImpl
	::GenerateCommand(const QString& strCmd)
{
	DCCore::CommandPtr pCmd;

	//确保命令不区分大小写，将命令统一转为大写
	QString strUpperCmd = strCmd.toUpper();

	//查找待注册的命令名称是否存在
	if (IsRegistered(strUpperCmd))
	{
		//通过命令生成器生成命令
		pCmd = m_commandGenerators.value(strUpperCmd)();

		//记录命令
		RecordCommand(pCmd);
	}

	return pCmd;
}

//记录命令
void CommandManager::CommandManagerImpl
	::RecordCommand(DCCore::CommandPtr pCmd)
{
	//只记录五条命令,如果记录数超过5条，则从顶层移除
	if (m_cmdStack.count() >= c_cmdStackSize)
	{
		m_cmdStack.pop_front();
	}

	//记录命令
	m_cmdStack.append(pCmd);

	//命令名称
	QString strCmdName = pCmd->GetGlobalName();

	//记录命令名称
	if (m_recentCommands.count() >= c_recentCmdCount)
	{
		m_recentCommands.pop_front();
	}

	if (!m_recentCommands.contains(strCmdName))
	{
		m_recentCommands.append(strCmdName);
		m_currentRecentCmdIndex = m_recentCommands.count();
	}
}

//获取最近使用的命令
QStringList CommandManager::CommandManagerImpl
	::GetRecentCommands() const
{
	return m_recentCommands;
}

//向上查找最近命令
QString CommandManager::CommandManagerImpl
	::SearchUpRecentCommand()
{
	if (--m_currentRecentCmdIndex < 0)
	{
		m_currentRecentCmdIndex = 0;
	}

	return m_recentCommands.value(m_currentRecentCmdIndex);
}

//向下查找最近命令
QString CommandManager::CommandManagerImpl
	::SearchDownRecentCommand()
{
	//逐步增加索引，当索引超过最近使用命令的索引时
	//减少一个索引，保证索引始终不超限
	if (++m_currentRecentCmdIndex >= m_recentCommands.count())
	{
		--m_currentRecentCmdIndex;
		return "";
	}

	return m_recentCommands.value(m_currentRecentCmdIndex);
}

//加载命令
void CommandManager::CommandManagerImpl
	::LoadCommands()
{
	//获取命令搜索路径
// 	QList<QDir> cmdDirs = Parameters::CommandDirs();
// 	QDir dir = QApplication::applicationDirPath() + "/Plugins";
// 	cmdDirs.append(dir);
// 
// 	for (auto iter = cmdDirs.constBegin();
// 		iter != cmdDirs.constEnd(); ++iter)
// 	{
// 		//模块路径
// 		QDir cmdDir = *iter;
// 
// 		//模块列表
// 		QStringList commands = cmdDir.entryList(QStringList() << "*.mdl",
// 			QDir::NoFilter, QDir::Time | QDir::Reversed);
// 
// 		for (auto iter_0 = commands.constBegin();
// 			iter_0 != commands.constEnd(); ++iter_0)
// 		{
// 			//模块加载器
// 			QPluginLoader loader(cmdDir.absoluteFilePath(*iter_0));
// 
// 			//如果模块可识别，则加载
// 			if (CommandRegistrant* pCmdRegistrant = qobject_cast<CommandRegistrant*>(loader.instance()))
// 			{
// 				//获取命令生成器
// 				QMap<QString, CommandGenerator> generators = pCmdRegistrant->Generators();
// 
// 				//注册命令
// 				for (auto iter_1 = generators.constBegin();
// 					iter_1 != generators.constEnd(); ++iter_1)
// 				{
// 					RegisterCommand(iter_1.key(), iter_1.value());
// 				}
// 			}
// 		}
// 	}
}





/****************************************输出注册的命令*****************************************/
QMap<QString, DCCore::CommandGenerator>* OutputRegisteredCommand::s_commands = nullptr;

//命令生成器
DCCore::CommandPtr OutputRegisteredCommand::Generate()
{
	return OutputRegisteredCommandPtr(new OutputRegisteredCommand);
}

//全局名称
QString OutputRegisteredCommand::GlobalName()
{
	return "$$OUTPUTCOMMANDS$$";
}
QString OutputRegisteredCommand::GetGlobalName() const
{
	return GlobalName();
}

//设计者信息
QString OutputRegisteredCommand::GetDesigner() const
{
	return tr("DCLW");
}

//描述信息
QString OutputRegisteredCommand::GetDescription() const
{
	return tr("Output registered commands.");
}

//执行命令
void OutputRegisteredCommand::Execute()
{
	//选择要保存的文件路径
	QString strFileName = QFileDialog::getSaveFileName(QApplication::activeWindow(),
		tr("Save Registered Commands"), "", "Excel File(*.xls)");

	//保存命令
	if (s_commands && !strFileName.isEmpty())
	{
		SaveCommands(strFileName, *s_commands);
	}
}

//将所有命令保存到指定文件中
void OutputRegisteredCommand::SaveCommands(const QString& strFileName,
	QMap<QString, DCCore::CommandGenerator> commands)
{
	
}