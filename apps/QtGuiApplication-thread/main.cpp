/*******************************************************************
*  Copyright(c) 2012-2017 DCLW
*  All rights reserved.
*
*  文件名称:
*  简要描述: 测试c++11自带的线程管理类
*
*  创建日期: 20170707
*  作者:     王龙阳
*  说明:
*
*  修改日期:
*  作者:
*  说明:
******************************************************************/

#include "QtGuiApplicationthread.h"
#include <QtWidgets/QTreeWidgetItem>


#include <thread>
#include <mutex>

int j = 0;

std::mutex mt;

void Foo()
{
	mt.lock();
	j++;
	mt.unlock();
}

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);


	//! 启动线程
	std::thread td1(Foo);
	td1.join();

	std::thread td2(Foo);
	td2.join();

	QtGuiApplicationthread w;
	w.show();
	return a.exec();
}
