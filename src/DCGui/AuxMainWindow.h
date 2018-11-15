//##########################################################################
//#                                                                        #
//#                            DCLW                                        #
//#                                                                        #
//#  This library is free software; you can redistribute it and/or modify  #
//#  it under the terms of the GNU Lesser General Public License(LGPL) as  #
//#  published by the Free Software Foundation; version 2 of the License.  #
//#                                                                        #
//#  This library is distributed in the hope that it will be useful,       #
//#  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
//#  GNU Lesser General Public License for more details.                   #
//#                                                                        #
//#          COPYRIGHT: DCLW             																	 #
//#                                                                        #
//##########################################################################

/*==========================================================================
*【功能概述】
*窗口基类，用来加载插件，为窗口做辅助性工作
*作者：dclw         时间：%time%
*文件名：%safeitemname%
*版本：V2.1.0
*$safeitemrootname$
*修改者：          时间：
*修改说明：
*===========================================================================
*/

#ifndef AUXMAINWINDOW_H
#define AUXMAINWINDOW_H


#include "dcguiDLL.h"

#include <QObject>
#include "QVector"
#include "QStringList"

#include "IWindow.h"

#include "DCCore/PluginInterface.h"

namespace DCGui
{
	class DCGUI_EXPORT AuxMainWindow : public IWindow
	{
		Q_OBJECT
	public:
		AuxMainWindow(QWidget *parent = 0, Qt::WindowFlags flags = 0);
		virtual ~AuxMainWindow();

	public:
		//加载模块
		void LoadModules();

		//卸载模块
		void UnloadModules();
		
	private:
		//连接加载模块的信号
		void ConnectModule(PluginInterface* pModule);

		//加载菜单栏和工具栏
		void LoadMenusAndToolBars(QList<QMenu*> menus,
			QList<QToolBar*> toolBars);

		//加载停靠窗体
		void LoadDockWidgets(QMap<QDockWidget*, Qt::DockWidgetArea> docks);

		//注册命令
		//void RegisterCommands(QMap<QString, DCCore::CommandGenerator> generators);
	
	//! 预定义的一些事件槽/默认的帮助和关于槽
	private slots:
		virtual void About();

		virtual void Help();
	private:
		//插件容器
		QVector<PluginInterface* > m_Module;

		////////////////////////////////文件相关//////////////////////////////////////////
		///最近打开的文件
		QStringList m_recentFiles;

		//! 当前文件
		QString m_currentFile;

		//! 最后打开文件路径
		QString m_lastDirectory;
	};
}

#endif // AUXMAINWINDOW_H
