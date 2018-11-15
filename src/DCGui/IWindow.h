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
*【功能概述】IWindow主要设计用来作为基类，加载并配置生成菜单栏；工具栏
管理面板；控制台以及命令行；属性窗口等
*
*作者：dclw         时间：$time$
*文件名：$safeitemname$
*版本：V4.1.0
*
*修改者：          时间：
*修改说明：
*===========================================================================
*/

#ifndef DCGUI_IWINDOW_H
#define DCGUI_IWINDOW_H

//Qt
#include "QMainWindow"

#include "dcguiDLL.h"

namespace DCGui
{
	class UI_IWindow;
	class DCGUI_EXPORT IWindow : public QMainWindow
	{
		Q_OBJECT
	public:
		IWindow(QWidget *parent = 0, Qt::WindowFlags flags = 0);
		virtual ~IWindow();

		/******************************接口****************************************/
		QMenuBar* GetMenuBar() const;

		bool ConfigInit(QMainWindow* pInter);
		void ConfigFinish(QMainWindow* pInter);

		//! 获取menu开头的菜单条目个数
		int GetRootMenuNum() const;

		//! 获取menu开头的菜单中指定ID对应的action，后续插入菜单的时候，作为插入菜单位置使用
		QAction* GetAction(const int ID);

		QString GetTitle();
		QString GetOrg();
	private:
		//启动画面
		void ConfigSplash();
		void ConfigStyleSheet();

		void ConfigUI();
		void ConfigTitleBar();
		void ConfigMenu();

		//setting 设置
		//! 保存相关设置参数
		void SaveSettings();

		//! 加载记录的设置
		void LoadSettings();

		private slots:
			//void on_actionOpen_triggered();

	private:
		/***********************************路径参数(dir结束的都带结束分隔符)*****************************************************/
		QString m_appDir;

		/***********************************UI*************************************************/
		UI_IWindow* m_pUI;

		//软件简称加版本号
		QString m_org;
		QString m_title;
	};
}

#endif // IWINDOW_H
