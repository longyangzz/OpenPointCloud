#ifndef DCCORE_PLUGININTERFACE_H
#define DCCORE_PLUGININTERFACE_H

//Qt
#include "QObject"
#include "QVector"
#include "QList"
#include "QMap"

class QMenu;
class QToolBar;
class QWidget;
class QTreeWidgetItem;
class QDockWidget;

//命令
#include "Command.h"

class PluginInterface : public QObject
{
public:
	//模块菜单
	virtual QList<QMenu*> Menus() const = 0;
	//模块工具条
	virtual QList<QToolBar*> ToolBars() const = 0;
	//停靠窗体
	virtual QMap<QDockWidget*, Qt::DockWidgetArea> DockWidgets() const = 0;

	//选项根节点
	virtual QTreeWidgetItem* OptionRoot() const = 0;
	//选项窗体
	virtual QMap<QTreeWidgetItem*, QWidget*> OptionWidgets() const = 0;

	//命令生成器
	virtual QMap<QString, DCCore::CommandGenerator> CommandGenerators() const = 0;

	/*******************以下信号和槽根据项目需求自行添加*************************/
//	//slots:
//		//改变待处理数据
//		void ChangedHandlingEntities(const QVector<DcGp::DcGpEntity*>& entities);
//
//signals:
//	//发送处理后的数据
//	void EntitiesGenerated(const QVector<DcGp::DcGpEntity*>& entities);
//	//执行命令
//	void ExecuteCommand(const QStringList& commands);
};

Q_DECLARE_INTERFACE(PluginInterface, "com.bjdclw.DC-Platform.PluginInterface/2.0")

#endif