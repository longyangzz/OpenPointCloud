#ifndef DCGP_BASIC_TYPES_H
#define DCGP_BASIC_TYPES_H

//C++标准库
#include <array>

using std::array;

//Common
#include "DCCore/BasicTypes.h"

//法线类型
typedef unsigned short NormalType;

//颜色类型(R, G, B)
typedef unsigned char ColorType;

const ColorType MAX_COLOR_COMP = 255;

//! Scan index type
typedef unsigned char ScanIndexType;

//点坐标
typedef DCVector3D PointCoord;			//3D
typedef array<PointCoordinateType, 2> PointCoord2D;			//2D


typedef array<ColorType, 3> PointColor;
//RGB颜色
typedef array<ColorType, 3> RGBColor;
//RGBA颜色
typedef array<ColorType, 4> RGBAColor;
//法向量值
typedef DCVector3D Normal;

namespace DcGp
{
	enum ColorMode
	{
		//伪彩色
		eFalseColor			=	0	,
		eFalseRangeColor			,
		eFalseHeightColor			,
		eFalseScalarColor			,

		//真彩色
		eTrueColor					,
		eTrueLinearColor			,

		//其它
		eOther						,
		eSingleColor				,

		//单元色
		eUnit						,
		eUnitmaterial				,

		//幅度
		eAmplitude					,
		eAmplitudeLinearColor		,
		eTrueLogarithmicColor		,
		eDeviationColor
	};

	namespace Color
	{
		//预定义颜色值
		static const ColorType WHITE[3]			=	{MAX_COLOR_COMP,MAX_COLOR_COMP,MAX_COLOR_COMP};
		static const ColorType LIGHTGREY[3]		=	{200,200,200};
		static const ColorType DARKGREY[3]		=	{MAX_COLOR_COMP/2,MAX_COLOR_COMP/2,MAX_COLOR_COMP/2};
		static const ColorType RED[3]			=	{MAX_COLOR_COMP,0,0};
		static const ColorType GREEN[3]			=	{0,MAX_COLOR_COMP,0};
		static const ColorType BLUE[3]			=	{0,0,MAX_COLOR_COMP};
		static const ColorType DARKBLUE[3]		=	{0,0,MAX_COLOR_COMP/2};
		static const ColorType MAGENTA[3]		=	{MAX_COLOR_COMP,0,MAX_COLOR_COMP};
		static const ColorType CYAN[3]		    =	{0,MAX_COLOR_COMP,MAX_COLOR_COMP};
		static const ColorType ORANGE[3]		=	{MAX_COLOR_COMP,MAX_COLOR_COMP/2,0};
		static const ColorType BLACK[3]			=	{0,0,0};
		static const ColorType YELLOW[3]		=	{MAX_COLOR_COMP,MAX_COLOR_COMP,0};

		//预定义材料色
		static const float BRIGHT[4] =	{1.0, 1.0, 1.0, 1.0};
		static const float LIGHTER[4] = {0.83f,0.83f,0.83f,1.0f};
		static const float LIGHT[4] =	{0.66f,0.66f,0.66f,1.0f};
		static const float MIDDLE[4] =	{0.5f,0.5f,0.5f,1.0f};
		static const float DARK[4] = {0.34f,0.34f,0.34f,1.0f};
		static const float DARKER[4] =	{0.17f,0.17f,0.17f,1.0f};
		static const float DARKEST[4] = {0.08f,0.08f,0.08f,1.0f};
		static const float NIGHT[4] =	{0.0F,0.0F,0.0F,1.0F};
		static const float DEFAULTMESHFRONTDIFFUSE[4] = {0.0f,1.0f,0.32f,1.0f};
		static const float DEFAULTMESHBACKDIFFUSE[4] = {0.32f,1.0f,1.0f,1.0f};
		//static const float DEFAULTMESHBACKDIFFUSE[4] = {0.f,0.0f,0.0f,1.0f};

		//预定义背景色
		static const ColorType DEFAULTCOLOR[3]			=   {MAX_COLOR_COMP,MAX_COLOR_COMP,MAX_COLOR_COMP};
		static const ColorType DEFAULTBACKGROUNDCOLOR[3]		=   {138,138,138};
		static const ColorType DEFAULTHISTBACKGROUNDCOLOR[3]	=   {51,0,51};
		static const ColorType DEFAULTLABELCOLOR[3]		=   {255,255,0};
	};

	typedef std::array<unsigned, 3> Link;
	typedef std::vector<Link> LinkContainer;


	//显示模式
	enum ShowStyle
	{
		eShowGrid				=	0	,
		eShowFill				=	1	,
		eShowDefault			=	2
	};
}
#endif
