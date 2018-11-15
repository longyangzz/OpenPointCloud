#ifndef DC_DCGP_COMMON_H
#define DC_DCGP_COMMON_H

#include "QFile"
#include "QTextStream"
#include "DCCore/BasicTypes.h"


//  A=X方向上的象素分辨率
//	D、B=旋转系统
//	E=Y方向上的象素分辨素
//	C=栅格地图左上角象素中心X坐标
//	F=栅格地图左上角象素中心Y坐标
struct TFWParas 
{
	double a;
	double d;
	double b;
	double e;
	double c;
	double f;

	TFWParas()
	{

	}

	TFWParas(double value)
		: a(value)
		, d(value)
		, b(value)
		, e(value)
		, c(value)
		, f(value)
	{

	}
};

static TFWParas GetParasFromTfw(const QString& tfwFile)
{
	TFWParas paras(0.0);

	QFile file(tfwFile);
	if (!file.open(QIODevice::ReadOnly))
	{
		return paras;
	}

	QTextStream stream(&file);
	paras.a = stream.readLine().toDouble();
	paras.d = stream.readLine().toDouble();
	paras.b = stream.readLine().toDouble();
	paras.e = stream.readLine().toDouble();
	paras.c = stream.readLine().toDouble();
	paras.f = stream.readLine().toDouble();

	file.close();

	return paras;
}

static void GetPixelPositionByTFW(const TFWParas& paras, const Point_3D& point, int& row, int& column)
{
	//读取tfwfile，根据读取值，初始化

	double a = paras.a;

	double d = paras.d;
	double b = 	paras.b;

	double e =	paras.e;

	double	c = paras.c;// C 【栅格地图左上角象素中心X坐标】

	double f =	paras.f;// F 【栅格地图左上角象素中心Y坐标】


	double  xImg, yImg;

	yImg = ((d* point.x - a* point.y) - (c*d - a*f)) / (d*b - a* e);

	xImg = (point.x- (b* yImg + c )) / a;

	row = yImg;

	column = xImg;
}

static void GetPixelPosition(const Point_3D& minpoint, const Point_3D& maxpoint, const int& totalrow, const int& totalcolumn, const Point_3D& point, int& row, int& column)
{

	double a = (maxpoint.x - minpoint.x) / totalcolumn;

	double d = 0.0;
	double b = 	0.0 ;

	double e =	-(maxpoint.y - minpoint.y) / totalrow;;

	double	c = minpoint.x;// C 【栅格地图左上角象素中心X坐标】

	double f =	maxpoint.y;// F 【栅格地图左上角象素中心Y坐标】


	double  xImg, yImg;

	yImg = ((d* point.x - a* point.y) - (c*d - a*f)) / (d*b - a* e);

	xImg = (point.x- (b* yImg + c )) / a;

	row = yImg;

	column = xImg;
}

static void NormalizePixelPos(const int& totalrow, const int& totalcolumn, const int& row, const int& column, DCVector2D& texCoord)
{
	double height = totalrow;    //行总数
	double width = totalcolumn;     //列总数

	texCoord.x = column / width;
	texCoord.y = (height-row) / height;
}

#endif