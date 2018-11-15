#ifndef MPVIEWER_VIEWER_H
#define MPVIEWER_VIEWER_H

#include "ViewDLL.h"

#include <mutex>

//DDCore
#include "DCCore/BasicTypes.h"

//View
#include "DCView/View.h"
#include "DCView/ViewBase.h"


//DcGp
#include "DCGp/GpBasicDevice.h"
#include "DCGp/GpDrawContext.h"
#include "DCGp/Custom/GpTools.h"
#include "DCGp/Custom/GpInteractor.h"

//MPutil
#include "DCUtil/Catcher.h"
#include "UnitManager.h"


namespace DcGp
{
	class DcGpEntity;
	class DcGpPolyline;
}

namespace MPGp
{
	class DistanceLine;
}

namespace DCView
{
	class GUIEventHandle;
}

//单视窗浏览器

namespace MPViewer
{

	class DC_VIEW_API Viewer : public DCView::View, public DcGp::DcGpBasicDevice, public ViewBase
	{
		Q_OBJECT
	public:
		explicit Viewer(QWidget* pParent = nullptr, 
			const QGLFormat& format = QGLFormat::defaultFormat(), 
			QGLWidget* pSharedWidget = nullptr);

		//! 根据传入的文件名，实例化一个viewer
		explicit Viewer(QString fileName);

		~Viewer();

		//设置图元
		void SetSceneRoot(DcGp::DcGpEntity* pEntity);
		//获取图元
		DcGp::DcGpEntity* GetSceneRoot() const;

		//是否捕捉到了旋转中心点
		bool IsRCenterPicked(const DCCore::Point_2& point2D,
			DCCore::Point_3& point3D);

		//屏幕坐标到世界坐标
		virtual bool ScreenToWorld(const DCCore::Point_2& point2D,
			DCCore::Point_3& point3D);

		//动态更新旋转中心状态
		void SetDynamicPivotState(bool state);
		bool GetDynamicPivotState();

		//居中显示全部数据
		void CenterAll();
		//居中显示选中数据
		void CenterSelected(DcGp::DcGpEntity* pEntity);

		//更新中心和缩放比例
		void UpdateBoundingBox();

		//! 更新当前激活列表
		void UpdateActiveItemsList(int x, int y, bool extendToSelectedLabels = false);

		void SetPickedWorldPoint(QString pickedWorldPoint);
		void SetCursorText(QString cursorText);

		//void SetDrawPickedPointState(bool state);

		//! 获取view关联的捕捉器
		MPUtil::Catcher* GetCatcher();

		void SetPressPointState(bool state);

		//! 根据当前鼠标事件，计算并将生成的hits记录发射出去
		MPUtil::HitList GenHitsByEvent(QMouseEvent* event);

		//! 添加一个事件处理器
		void AddEventHandle(DCCore::GUIEventHandle* eventHandle);

		void UpdateEventhandleValidState(QStringList handles, bool state);

		//! 根据指定名字获取事件处理器
		DCCore::GUIEventHandle* GetEventHandleByClassName(QString uniqueName);

		//! 获取所有事件处理器
		std::vector<DCCore::GUIEventHandle* > GetEventHandles();

		//! 获取视窗截图
		QImage FrameBufferSnapshot();

		virtual void EventTraversal();

		virtual void UpdateTraversal();

		virtual void RenderingTraversals();

		//OpenGL绘制
		virtual void paintGL();

	private:

		//! 初始化viewer基本配置
		void Init();
		//关闭事件
		void closeEvent(QCloseEvent* event);

		void DrawSnapSymbol();
signals:
		//对象被拾取
		void EntityPicked(DcGp::DcGpEntity* pEntity);
		//点被拾取
		void PointPicked(const DCCore::Point_3& point);
		//生成图元
		void EntityGenerated(DcGp::DcGpEntity* pEntity);

		//移除根图元
		void RemoveRootEntity(DcGp::DcGpEntity* pEntity);

		//拾取到点
		void IdIndexPicked(unsigned selectedID, unsigned subSelectedID);

		//! 实体选择变化，传入的是objID
		void EntitySelectionChanged(int uinqueID);

		//! 返回当前光标位置处的hits
		void RayHits(MPUtil::HitList hits);
		private slots:
			//改变投影矩阵
			void ChangeProjectionMatrix(const double* pMatrix);
			//改变模型视图矩阵
			void ChangeModelViewMatrix(const double* pMatrix);
			//改变视口矩阵
			void ChangeViewportMatrix(const int* pMatrix);

			//改变当前缩放比例
			void ChangedCurrentScale(double scale);

	protected:
		//鼠标按下
		void mousePressEvent(QMouseEvent* event);
		//鼠标移动
		void mouseMoveEvent(QMouseEvent* event);
		//键盘按下事件
		void keyPressEvent(QKeyEvent* event);

		void mouseReleaseEvent(QMouseEvent* event);
		void mouseDoubleClickEvent(QMouseEvent* event);

	private:
		void CreateConnections();

		//绘制上下文
		DcGp::DcGpDrawContext GetContext();

	private:
		//拾取模式
		enum PickingMode
		{ 
			eNoPicking					=	0	,                    //! 没有实体捕捉
			eEntityPicking						,					 //! 单个实体捕捉，一般在鼠标释放中激活
			eEntityRectPicking					,                    //! 矩形选择多个实体
			eFastPicking						,                    //! 快速实体捕捉
			ePointPicking						,
			eTrianglePicking					,
			eAutoPointPicking					,
			eDoubleClickedPicking               ,
			eElementPicking                     ,
			eDefaultPicking						
		};
		//选取
		int Pick(PickingMode mode, const DCCore::Point_2& point, 
			int* subID = nullptr, int width = 5, int height = 5, bool drawSample = false);

		virtual void DrawWithNames(const QPoint& point);
		virtual void PostPicking(const QPoint& point) { Q_UNUSED(point); }

	private:
		virtual void Resize(int iWidth, int iHeight);
		virtual void Paint2D();
		virtual void Paint3D();

		virtual void FastDraw3D();
		virtual BoundBox BoundingBox() const;


	private:
		//-------------------------------视窗上管理的gp对象---------------------------------------------------------
		//场景根节点，共享节点
		DcGp::DcGpEntity* m_pSceneRoot;

		

		//-----------------------------------------------------------------------------------------

		//拾取缓冲区
		static const int s_c_pickBufferSize = 65536;
		GLuint m_pickBuffer[s_c_pickBufferSize];

		//动态更新旋转中心
		bool m_bUpdatingPivot;

		//投影矩阵
		double m_projectionMatrix[16];
		//模型视图矩阵
		double m_modelViewMatrix[16];

		//OpenGL环境大小
		GLuint m_glWidth;
		GLuint m_glHeight;

		/********************************************************************************/

		//-----------------控制是否使用当前选择对象的边界盒【缩放到当前功能使用】-----------------------------------------------------------------
		bool m_useSelectBoundbox;
		BoundBox m_selectBoundbox;

		//--------------------------------------------------------------------------------------------
		//! 当前激活对象列表，可以平移旋转对象
		std::list<DcGp::DcGpInteractor* > m_activeItems;

		//! 当前光标处世界坐标值
		QString m_pickedWorldPoint;

		//! 当前光标处需要显示绘制的文字(例如坐标值，与上一点的距离值等)
		QString m_cursorText;

		QPoint m_cursorPos;

		//**************单位管理********************/
		QString m_unit;

		double m_scale;

		UnitManager* m_unitMan;

		//使用工具捕捉器，通过外部更新捕捉器参数，返回不同的hits
		MPUtil::Catcher* m_catcher;

		//! 是否激活点击左键发射射线捕捉信号
		//bool m_isGetLeftPressedPoint;

		//! 事件处理器
		std::vector<DCCore::GUIEventHandle* > m_eventHandles;

		

	public:
		//单位管理
		virtual QString GetUnit();
		
		virtual QString GetENUnit();

		virtual void SetUnit(QString unit);

		virtual double GetScale();

		virtual void SetScale(double scale);
		//刷新显示
		virtual void Redraw(){DCView::View::Refresh();}

		//
		virtual void ToBeRefreshed();

		//
		virtual void Refresh(){DCView::View::Refresh();}

		//使当前视口设置无效
		/**当下一次重绘时，重新计算视口参数**/
		virtual void InvalidateViewport(){}

		//从图像中获取贴图的ID
		virtual unsigned GetTextureID(const QImage& image){return 0;}

		//从绘制内容中释放Texture
		virtual void ReleaseTexture(unsigned texID){}

		//返回字体
		virtual const QFont& GetDisplayedTextFont(){return QFont();}

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
			const QFont* font = 0);

		//在指定的三维坐标处显示文本
		//这个方法在三维视窗中使用
		//参数一：要显示的文本
		//参数二：文本的位置（向量）
		//参数三：颜色
		//参数四：字体
		virtual void Display3DLabel(const QString& str, 
			const DCVector3D& pos3D,
			const unsigned char* rgbColor = 0, 
			const QFont& font = QFont());

		//返回是否支持给定版本的OpenGL
		//参数为QGLFormat::OpenGLVersionFlag
		virtual bool SupportOpenGLVersion(unsigned openGLVersionFlag){return true;}

		//返回当前的模型视图矩阵(GL_MODELVIEW)
		virtual const double* GetModelViewMatrixd();

		//返回当前的投影矩阵(GL_PROJECTION)
		virtual const double* GetProjectionMatrixd();

		//返回当前的视口设置(GL_VIEWPORT)
		virtual void GetViewportArray(int viewport[/*4*/]);

		virtual QString GetWindowTitle() const{return "";}
		virtual void SetWindowTitle(QString title){}
	};

}

#endif