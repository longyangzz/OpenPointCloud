#ifndef DCGP_DRAW_CONTEXT_H
#define DCGP_DRAW_CONTEXT_H

//c++
#include "cstdint"

//#include "DCCore/DCGL.h"
#include "DCGp/GpBasicDevice.h"
#include "DCGp/Custom/GpMaterial.h"

namespace DcGp
{
	class DcGpScalarField;
	class DcGpBasicDevice;
	//三维实体的绘制参数
	class glDrawParameters
	{
	public:
		//! 显示标量场
		bool showScalarField;
		//是否显示颜色
		bool showColors;
		//是否显示法线
		bool showNormals;
	};

	//可显示的内容
	class DcGpDrawContext
	{
	public:
		uint16_t flags;       //绘制选项(see below)
		int glW;                    //Opengl窗口宽度
		int glH;                    //Opengl窗口高度
		DcGpBasicDevice* _win;   //Opengl窗口指针

		//默认材质
		DcGpMaterial defaultMat;
		float defaultMeshFrontDiff[4];
		float defaultMeshBackDiff[4];
		unsigned char pointsDefaultCol[3];
		unsigned char textDefaultCol[3];
		unsigned char labelDefaultCol[3];
		unsigned char bbDefaultCol[3];

		//优化简化选项
		bool decimateCloudOnMove;
		bool decimateMeshOnMove;

		//颜色标尺
		DcGpScalarField* sfColorScaleToDisplay;

		//点捕捉
		float pickedPointsRadius;
		float pickedPointsTextShift;

		//文本
		unsigned dispNumberPrecision;

		//标签
		unsigned labelsTransparency;

		DcGpDrawContext()
			: flags(0)
			, glW(0)
			, glH(0)
			, _win(0)
			, defaultMat()
			, decimateCloudOnMove(true)
			, decimateMeshOnMove(true)
			, sfColorScaleToDisplay(nullptr)
			, pickedPointsRadius(4)
			, pickedPointsTextShift(0.0)
			, dispNumberPrecision(6)
			, labelsTransparency(100)
		{}
	};

	// 绘制标识
#define DC_DRAW_2D                              0x0001
#define DC_DRAW_3D                              0x0002
#define DC_DRAW_FOREGROUND                      0x0004
#define DC_LIGHT_ENABLED                        0x0008
#define DC_SKIP_UNSELECTED                      0x0010
#define DC_SKIP_SELECTED                        0x0020
#define DC_SKIP_ALL                             0x0030	
#define DC_DRAW_ENTITY_NAMES                    0x0040
#define DC_DRAW_POINT_NAMES                     0x0080
#define DC_DRAW_TRI_NAMES						0x0100
#define DC_DRAW_FAST_NAMES_ONLY					0x0200
#define DC_DRAW_ANY_NAMES						0x03C0
#define DC_LOD_ACTIVATED                        0x0400
#define DC_VIRTUAL_TRANS_ENABLED                0x0800

	//绘制标识
#define MACRO_Draw2D(context) (context.flags & DC_DRAW_2D)							//是否是绘制二维物体
#define MACRO_Draw3D(context) (context.flags & DC_DRAW_3D)							//是否是绘制三维物体
#define MACRO_DrawPointNames(context) (context.flags & DC_DRAW_POINT_NAMES)
#define MACRO_DrawTriangleNames(context) (context.flags & DC_DRAW_TRI_NAMES)
#define MACRO_DrawEntityNames(context) (context.flags & DC_DRAW_ENTITY_NAMES)
#define MACRO_DrawNames(context) (context.flags & DC_DRAW_ANY_NAMES)
#define MACRO_DrawFastNamesOnly(context) (context.flags & DC_DRAW_FAST_NAMES_ONLY)
#define MACRO_SkipUnselected(context) (context.flags & DC_SKIP_UNSELECTED)
#define MACRO_SkipSelected(context) (context.flags & DC_SKIP_SELECTED)
#define MACRO_LightIsEnabled(context) (context.flags & DC_LIGHT_ENABLED)			//是否使用灯光
#define MACRO_Foreground(context) (context.flags & DC_DRAW_FOREGROUND)
#define MACRO_LODActivated(context) (context.flags & DC_LOD_ACTIVATED)
#define MACRO_VirtualTransEnabled(context) (context.flags & DC_VIRTUAL_TRANS_ENABLED)
}

#endif