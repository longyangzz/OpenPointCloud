#ifndef DC_VIEW_VIEWER_H
#define DC_VIEW_VIEWER_H

#include "ViewDLL.h"

#include <array>
//Boost
#include "boost/shared_ptr.hpp"

//Qt
#include "DCCore/DCGL.h"
#include <QPoint>

class QColor;
class QLinearGradient;

#include "Common.h"
#include "ViewerParams.h"

#define DCLWVIEWER_VERSION 0x020101

namespace DcGp
{
	class DcGpEntity;
	class DcGpPolyline;
}

namespace DCCore
{
	class GUIEventHandle;
}

namespace DCView
{
	class GLFilter;
	typedef boost::shared_ptr<GLFilter> GLFilterPtr;
	class GUIEventHandle;
	class Camera;

	enum MatrixPiple
	{
		eOpenGL						=	0,
		eCamera						=	1
	};

	/*
		*
		* 可视化窗口
		* 继承自己 QGLWidget
		* 
	*/
	class DC_VIEW_API View : public QGLWidget
	{
		Q_OBJECT
	public:
		
		// 视图模式
		enum ViewMode
		{
			eTopViewMode				=	0x00000010	,	//顶视图模式
			eBottomViewMode				=	0x00000020	,	//底视图模式
			eFrontViewMode				=	0x00000040	,	//前视图模式
			eBackViewMode				=	0x00000080	,	//后视图模式
			eLeftViewMode				=	0x00000100	,	//左视图模式
			eRightViewMode				=	0x00000200	,	//右视图模式
		};

		// 三维浏览模式
		enum InterActionMode
		{
			eNoInter			=	0, // 无交互
			ePan				=	1, // 平移
			eRotate				=	2, // 旋转
			eZoom				=	3, // 缩放
			e2DOnly				=	4, // 仅二维
			eOrther				=	5  // 其他 
		};

		//范围盒
		struct BoundBox
		{
			Point_3 center;
			Point_3 minCorner;
			Point_3 maxCorner;
		};

	public:
		// 获取当前窗体
		static View* CurrentViewer();

		// 当窗口关闭时候，清空当前窗口对象
		static void NullCurrentViewer();

		// 设为当前
		void SetCurrent();

	public:
		explicit View(QWidget* pParent = nullptr, 
			const QGLFormat& format = QGLFormat::defaultFormat(), 
			QGLWidget* pSharedWidget = nullptr);

		virtual ~View();

		//居中显示全部数据
		virtual void CenterAll() = 0;

		// 获取投影模式
		ProjectionMode GetProjection() const;
		
		// 设置投影模式
		void SetProjection(ProjectionMode mode);

		// 设置视图
		void SetView(ViewMode mode);
		
		// 重置视图
		void ResetView();

		// 获取点击位置的远近截面点射线
		void Get3DRayUnderMouse(const Point_2& point2D, Point_3* v1, Point_3* v2);

		// 屏幕(二维)坐标到世界(三维)坐标
		virtual bool ScreenToWorld(const Point_2& point2D, Point_3& point3D);
		
		// 世界(三维)坐标到屏幕(二维)坐标
		virtual bool WorldToScreen(const Point_3& point, Point_2& point2D);

		// 设置GLFilter
		void SetGLFilter(GLFilterPtr pFilter);
		
		// 获取GLFilter
		GLFilterPtr GetGLFilter() const;

		// 重绘
		void Refresh();

		// 设置是否启用LOD
		void SetApplyLODState(bool state);

		// 设置三维漫游模式
		void SetInterActionMode(InterActionMode iaMode);

		// 显示消息
		void DisplayMessage(const QString& message, int delay = 2000);

		// 绘制文字
		void DrawText(int x, int y, const QString& text, const QFont& fnt = QFont());

		// 设置阳光效果光照状态
		void SetSunLightState(bool state);
		
		// 获取阳光效果光照状态
		bool GetSunLightState();

		//计算鼠标方向
		Vector_3 MouseOrientation(unsigned iX, unsigned iY);

		// 移动相机
		void MoveCamera(const Vector_3& v);

		//  添加一个漫游器
		void AddManipulatorHandle(DCCore::GUIEventHandle* manipulator);

		// 旋转视图
		void RotateView(const Matrix_4_4& matrix);

		// 启用旋转中心
		void EnablePivot(bool bState);

		// 转动滚轴
		void RotatingWheel(float fDegree);

		//缩放
		void Scale(float fScale);

		// 获取视窗关联的camera
		Camera* GetCamera() const;
		
		// camera--测试camera类，后续用来替代gl代码
		MatrixPiple GetMatrixPiple() const;

		void SetMatrixPiple(MatrixPiple mpType);

		//使投影矩阵无效
		void DisableProjection();
		//使模型视图无效
		void DisableModelView();

		//更新相机位置
		void UpdateCameraPos(const DCVector3D& pos);
		void UpdateRotateView(const Matrix_4_4& rm);

		bool IsKeyPressed(Qt::Key key);

	//-----------------------------动态帧控制----------------------------
	public:
		void SetRefreshRate( int msec );
		int GetRefreshRate();

		void SetContinuousUpdate(bool continuous);
	signals:
		//鼠标按下
		void MousePressed(Qt::MouseButton button, int x, int y);
		//鼠标松开
		void MouseReleased(Qt::MouseButton button, int x, int y);
		//移动鼠标
		void MouseMoving(Qt::MouseButtons buttons, int x, int y);

		//投影矩阵发生变化
		void ProjectionMatrixChanged(const double* pMatrix);
		//模型视图矩阵发生变化
		void ModelViewMatrixChanged(const double* pMatrix);
		//视口矩阵发生变化
		void ViewportMatrixChanged(const int* pMatrix);

		//当前缩放比例发生变化,只有正射投影可用
		void CurrentScaleChanged(double scale);

	protected:
// 			enum Module
// 			{
// 				eGlewModule					=	0x00000010	,	//glew模块
// 				eShaderModule				=	0x00000020	,	//Shader模块
// 				eFBOModule					=	0x00000040		//FBO模块
// 			};
// 			typedef Module Modules;
// 			//启用模块
// 			void Enable(Modules modules);
	protected:
		//鼠标点击
		virtual void mousePressEvent(QMouseEvent* event);
		//鼠标松开
		virtual void mouseReleaseEvent(QMouseEvent* event);
		//鼠标移动
		virtual void mouseMoveEvent(QMouseEvent* event);
		//转动滚轮
		virtual void wheelEvent(QWheelEvent* event);

		//键盘按键点击
		virtual void keyPressEvent(QKeyEvent* event);

		virtual void keyReleaseEvent(QKeyEvent * event);
	public:
		//更新大小
		virtual void Resize(int iWidth, int iHeight){}
		//绘制
		virtual void Paint3D() = 0;
		virtual void Paint2D() = 0;
		virtual void FastDraw3D() {}

		//pick及post
		virtual void DrawWithNames(const QPoint& point) {}
		virtual void PostPicking(const QPoint& point) { Q_UNUSED(point); }

		
		virtual BoundBox BoundingBox() const = 0;

		//更新中心并移动相机
		void UpdateCenterAndZoom(const BoundBox& box);

		//启用正投影
		void EnableOrthoProjection();

		//更新旋转中心
		void UpdatePivot(const Point_3& ptPivot);

		//计算屏幕实际像素大小
		float ComputeActualPixelSize();

		//获取视景体六面体参数
		std::array<std::array<float, 4> ,6> GetFrustum();

		virtual const ViewerParams& GetViewportParameters() const;

		//OpenGL绘制
		void Render();
	private:
		//初始化OpenGL环境
		virtual void initializeGL();
		//大小变化
		virtual void resizeGL(int iWidth, int iHeight);
		
	
	 //帮助信息接口
	public:
		virtual QString HelpString() const;

		virtual QString MouseString() const;
		virtual QString KeyboardString() const;

	//hud文字设置
	public:
		virtual void SetHudtext(QString text);

		//-----------------------------------InnerRoot[主要用来管理辅助实体对象，如选取工具中的多边形]-----------------------------------------------

		//! 获取内部根节点
		virtual DcGp::DcGpEntity* GetInnerRoot();

		//! 添加实体对象到innerRoot中
		virtual void AddtoInnerRoot(DcGp::DcGpEntity* obj2D, bool noDependency = true);

		//! 移除指定的对象
		virtual void RemoveFormInnerRoot(DcGp::DcGpEntity* obj2D);

		//----------------------------------------------------------------------------------
		
	public slots:
		virtual void Help();
		void SetKeyDescription(int key, QString description);
	private:
		class ViewerImpl;
		friend class ViewerImpl;
		ViewerImpl* m_pImpl;
	};
}

#endif // VIEWER_H
