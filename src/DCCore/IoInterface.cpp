
//Qt
#include "QDir"
#include <QtWidgets/QApplication>
#include "QObject"

//DCCore
#include "DCCore/Logger.h"
#include "DCCore/IoInterface.h"

#include <Windows.h>
#include <WinBase.h>

QString DCCore::GetIOPluginPath()
{
	QDir ioDir(QApplication::applicationDirPath());
	if (!ioDir.exists("IO"))
	{
		Logger::Message(QObject::tr("Unable to find the IO plugins directory!"));
	}
	ioDir.cd("IO");

	return ioDir.absolutePath();
}

bool DCCore::GetIOPlugin(QDir dir, QString extension, IoPlugin*& io)
{
	//遍历所有文件
	dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
	QStringList filters;
	filters << "*.dll";
	dir.setNameFilters(filters);
	QStringList list = dir.entryList();

	HINSTANCE hInst;
	for (unsigned i = 0; i < list.size(); ++i)
	{
		//当前dll名字
		QString ioName = list.at(i);

		QString dllFullname = dir.absoluteFilePath(ioName);

		//显示加载该dll，并调用固定接口函数
		hInst = LoadLibrary(dllFullname.toStdString().c_str());

		//定义函数指针
		typedef IoPlugin* (*LoadPlugin)(const char* );
		//获取dll中导出的函数
		LoadPlugin plugin = (LoadPlugin)GetProcAddress(hInst, "LoadPlugin");

		if (!plugin)
		{
			FreeLibrary(hInst);
			continue;
		}

		io = plugin(extension.toStdString().c_str());
		if (io) //只要找到一个匹配的解析器，则跳出循环
		{
			break;
		}
		else
		{
			FreeLibrary(hInst);
		}
	}
	if (!io)
	{
		Logger::Message(QObject::tr("Does not find IO plugin(Maybe missed IO or depend DLL)!"));
		return nullptr;
	}
	return io;
}