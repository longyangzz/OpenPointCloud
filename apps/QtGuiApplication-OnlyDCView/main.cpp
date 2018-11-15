#include <QtWidgets/QApplication>

#include "QGridLayout"

#include "DCView/Viewer.h"

//! DCGa
#include "DCGa/TrackballManipulator.h"
#include "DCGa/FlyManipulator.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	//! 添加dcviewer
	//! 读取传入数据
	//QString fileName = "D:\\data\\TestData\\LOUTI-5.txt";
	QString fileName = "D:\\data\\changshu-pdb\\changshu.pdb";
	//QString fileName = "D:\\data\\bieshu-pdb\\bieshu.pdb";


	MPViewer::Viewer* pNewViewer = new MPViewer::Viewer(fileName);
	//MPViewer::Viewer* pNewViewer = new MPViewer::Viewer();

	//! 安装漫游器
	pNewViewer->AddManipulatorHandle(new DCGa::TrackballManipulator("TrackBall"));
	pNewViewer->AddManipulatorHandle(new DCGa::FlyManipulator("Fly"));
	pNewViewer->showMaximized();


	return a.exec();
}
