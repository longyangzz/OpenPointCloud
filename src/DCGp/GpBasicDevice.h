#ifndef DCGP_BASIC_DEVICE_H
#define DCGP_BASIC_DEVICE_H

#include "DCGp/DcGpDLL.h"

//Qt标准库
#include "QtGui/QFont"
class QString;
class QImage;

//DCCore
#include "DCCore/BasicTypes.h"

#include "DCGp/GpMatrix.h"

namespace DcGp
{

//OpenGL视口模型的标准参数
	DECLARE_SMARTPTR(DcGpViewportParameters)
	DECLARE_SMARTPTR(DcGpViewportParametersImpl)

class DCGP_API DcGpViewportParameters
{
	DECLARE_IMPLEMENT(DcGpViewportParameters)

public:
	//默认构造函数
	DcGpViewportParameters();

	//赋值构造函数
	DcGpViewportParameters(const DcGpViewportParameters& params);

	//像素大小
	float& PixeSize();

	//缩放比例
	float& Zoom();

	//视图矩阵的旋转矩阵
	DcGpMatrix& ViewRotateMatrix();

	//点大小
	float& PointSize();

	//线宽
	float& LineWidth();

	//投影状态
	bool& Perspective();

	bool& ObjCenterd();

	//旋转中心
	DCVector3D& PivotPosition();

	//相机位置
	DCVector3D& CameraPosition();

	//
	float& Fov();

	//
	float& Aspect();

	void SaveNear(double near);
	void SaveFar(double far);
	double GetNear();
	double GetFar();
};

//GL显示界面
class DcGpBasicDevice
{
public:
	virtual ~DcGpBasicDevice()
	{

	}

    //刷新显示
	virtual void Redraw() = 0;

	//
    virtual void ToBeRefreshed() = 0;

    //
    virtual void Refresh() = 0;

    //使当前视口设置无效
    /**当下一次重绘时，重新计算视口参数**/
    virtual void InvalidateViewport() = 0;

    //从图像中获取贴图的ID
    virtual unsigned GetTextureID(const QImage& image) = 0;

    //从绘制内容中释放Texture
    virtual void ReleaseTexture(unsigned texID) = 0;

	//返回字体
	virtual const QFont& GetDisplayedTextFont() = 0;

	//文本的对齐方式
	enum TextAlign
	{ 
		eLeftAlign			=	1	,
		eHMidAlign			=	2	,
		eRightAlign			=	4	,
		eTopAlign			=	8	,
		eVMidAlign			=	16	,
		eBottomAlign		=	32	,
		eDefaultAlign		=	1 | 8
	};

    //在指定的二维坐标处显示字符串
	//方法在二维视窗使用
	//参数一：显示的文本
	//参数二、三：文本的X、Y坐标
	//参数四：文本的对齐方式
	//参数五：文本的透明度
	//参数六：文本的颜色
	//参数七：文本的字体
    virtual void Display2DLabel(QString text, int x, int y, 
								unsigned char align = eDefaultAlign, 
								float bkgAlpha = 0, 
								const unsigned char* rgbColor = 0, 
								const QFont* font = 0) = 0;

	//在指定的三维坐标处显示文本
    //这个方法在三维视窗中使用
    //参数一：要显示的文本
    //参数二：文本的位置（向量）
	//参数三：颜色
	//参数四：字体
	virtual void Display3DLabel(const QString& str, 
								const DCVector3D& pos3D,
								const unsigned char* rgbColor = 0, 
								const QFont& font = QFont()) = 0;

	//返回是否支持给定版本的OpenGL
	//参数为QGLFormat::OpenGLVersionFlag
	virtual bool SupportOpenGLVersion(unsigned openGLVersionFlag) = 0;

	//返回当前的模型视图矩阵(GL_MODELVIEW)
	virtual const double* GetModelViewMatrixd() = 0;

	//返回当前的投影矩阵(GL_PROJECTION)
    virtual const double* GetProjectionMatrixd() = 0;

	//返回当前的视口设置(GL_VIEWPORT)
    virtual void GetViewportArray(int viewport[/*4*/]) = 0;

	virtual QString GetWindowTitle() const = 0;
	virtual void SetWindowTitle(QString title) = 0;
};

}
#endif