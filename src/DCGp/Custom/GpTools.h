#ifndef DCGP_TOOLS_H
#define DCGP_TOOLS_H

#include "DCGp/DcGpDLL.h"

//Qt标准库
#include "QString"

#include "DCCore/DCGL.h"

namespace DcGp
{
	class DcGpMatrix;

	//视图方向
enum ViewOrientation
{  
	eResetView			=	0	,
	eTopView					,
	eBottomView					,
	eFrontView					,
	eBackView					,
	eLeftView					,
	eRightView					,
	eISOView1					,
	eISOView2
};

	class DCGP_API DcGpTools
	{
	public:

		static void DisplayTexture2DPosition(GLuint tex, int x, int y, int w, int h, unsigned char alpha=255);
		static void DisplayTexture2D(GLuint tex, int w, int h, unsigned char alpha=255);
		static void DisplayTexture2DCorner(GLuint tex, int w, int h, unsigned char alpha=255);

		static bool SaveTextureToFile(const char* filename, GLuint texID, unsigned w, unsigned h);

		//
		static void SaveTextureToArray(unsigned char* data, GLuint texID, unsigned w, unsigned h);

		//
		static DcGpMatrix GenerateGLRotationMatrixFromVectors(const float* sourceVec, const float* destVec);

		//
		static DcGpMatrix GenerateGLRotationMatrixFromAxisAndAngle(const float* axis, float angle_deg);

		//
		static void MultGLMatrices(const float* A, const float* B, float* dest);

		//
		static void TransposeGLMatrix(const float* A, float* dest);

		//
		static DcGpMatrix GenerateViewMat(ViewOrientation orientation);

		//
		static void MakeLightsNeutral();

		//捕获最近一次的OpenGL错误
		static bool CatchGLError(QString error);
	};
}


#endif //CC_GL_UTILS_HEADER
