#ifndef DC_VIEW_IMPL_VIEWER_H
#define DC_VIEW_IMPL_VIEWER_H

#include "DCView/View.h"
#include <array>
#include <set>
//Boost
#include "boost/shared_ptr.hpp"

//Qt
#include "QObject"
#include "QTimer"

#include "DCCore/DCConfig.h"

class QTabWidget;

namespace DcGp
{
	class DcGpEntity;
	class DcGpPolyline;
}

namespace DCCore
{
	class Transform;
	class GUIEventHandle;
	
}

namespace DCView
	{
		//FBO
		class FrameBuffer;
		typedef boost::shared_ptr<FrameBuffer> FrameBufferPtr;

		class Camera;
		class GUIEventHandle;
		//视窗实现类
		class View::ViewerImpl : public QObject
		{
			Q_OBJECT

		public:
			//获取当前视窗
			static View* CurrentViewer();
			static void NullCurrentViewer();
			//设为当前
			void SetCurrent();

			
			enum KeyboardAction 
			{ 
				DRAW_AXIS, 
				DRAW_GRID, 
				DRAW_OPT_HIDING, 
				HELP,
				PROJECTMODE,
				EYESHADER,
				SNAPSHOT_TO_CLIPBOARD
			};

			enum MouseHandler 
			{ 
				CAMERA, 
				FRAME 
			};

			enum ClickAction 
			{ NO_CLICK_ACTION, ZOOM_ON_PIXEL, ZOOM_TO_FIT, SELECT, RAP_FROM_PIXEL, RAP_IS_CENTER,
				CENTER_FRAME, CENTER_SCENE, SHOW_ENTIRE_SCENE, ALIGN_FRAME, ALIGN_CAMERA 
			};
			enum MouseAction { 
				NO_MOUSE_ACTION,
				ROTATE, ZOOM, 
				TRANSLATE,
				MOVE_FORWARD,
				LOOK_AROUND, 
				MOVE_BACKWARD,
				SCREEN_ROTATE,
				ROLL, DRIVE,
				SCREEN_TRANSLATE, 
				ZOOM_ON_REGION 
			};			

		private:
			//当前视窗
			static View* s_pCurrentViewer;

		public:
			explicit ViewerImpl(View* pInterface);
			~ViewerImpl();

			//获取投影模式
			ProjectionMode GetProjection() const;
			//设置投影模式
			void SetProjection(ProjectionMode mode);

			//屏幕坐标到世界坐标
			bool ScreenToWorld(const Point_2& point2D,
				Point_3& point3D);
			//世界坐标到屏幕坐标
			bool WorldToScreen(const Point_3& point,
				Point_2& point2D);

			//初始化OpenGL环境
			void InitializeGL();
			//大小变化
			void ResizeGL(int iWidth, int iHeight);
			//绘制OpenGL
			void Render();

			//! 更新遍历
			void UpdateTraversal();

			//鼠标点击
			void PressedMouse(QMouseEvent* event);
			//鼠标松开
			void ReleasedMouse(QMouseEvent* event);
			//鼠标移动
			void MovingMouse(QMouseEvent* event);
			//转动滚轮
			void Wheeling(QWheelEvent* event);

			void KeyPress(QKeyEvent* event);
			void KeyRelease(QKeyEvent* event);
			
			//! 更新gl
			void ReDraw();

			//刷新
			void Refresh();

			void DirtyRefresh();

			//启用正投影
			void EnableOrthoProjection();

			//更新旋转中心
			void UpdatePivot(const Point_3& ptPivot);

			//更新中心并移动相机
			void UpdateCenterAndZoom(const View::BoundBox& box);

			//设置视图
			void SetView(View::ViewMode mode);
			//重置视图
			void ResetView();

			//设置GLFilter
			void SetGLFilter(GLFilterPtr pFilter);
			//获取GLFilter
			GLFilterPtr GetGLFilter() const;

			//返回当前视图的方向
			Point_3 GetCurrentViewDirection() const;

			//计算屏幕实际像素大小
			float ComputeActualPixelSize();

			//获取视景体六面体参数
			std::array<std::array<float, 4> ,6> GetFrustum();

			const ViewerParams& GetViewportParameters() const;

			//设置是否启用旋转隐藏
			void SetApplyLODState(bool state);

			//! 设置三维漫游模式
			void SetInterActionMode(View::InterActionMode iaMode);

			//-----------------------------------InnerRoot[主要用来管理辅助实体对象，如选取工具中的多边形]-----------------------------------------------

			//! 获取内部根节点
			virtual DcGp::DcGpEntity* GetInnerRoot();

			//! 添加实体对象到innerRoot中
			virtual void AddtoInnerRoot(DcGp::DcGpEntity* obj2D, bool noDependency = true);

			//! 移除指定的对象
			virtual void RemoveFormInnerRoot(DcGp::DcGpEntity* obj2D);

		//----------------------------------------------------------------------------------
		
		//-----------------------------屏幕光标位置射线相关----------------------------
		public:
			//! 获取点击位置的远近截面点射线
			void Get3DRayUnderMouse(const Point_2& point2D, Point_3* v1, Point_3* v2);


			//! 光照相关
			//设置阳光效果光照
			void SetSunLightState(bool state);

			bool GetSunLightState();

			void DrawLightPos();
		
		//-----------------------------动态帧控制----------------------------
		public:
			void SetRefreshRate( int msec );
			int GetRefreshRate();

			void SetContinuousUpdate(bool continuous);


		//帮助信息接口
		public:
			void Help();
			QString HelpString() const;
			QString MouseString() const;
			QString KeyboardString() const;

			void SetKeyDescription(int key, QString description);
		private:
			void SetDefaultShortcuts();
			void SetDefaultMouseBindings();
			void HandleKeyboardAction(KeyboardAction id);
		public slots:
			void SetShortcut(KeyboardAction action, int key);

		private:
			// M o u s e   a c t i o n s
			struct MouseActionPrivate {
				MouseHandler handler;
				MouseAction action;
				bool withConstraint;
			};

			// C l i c k   a c t i o n s
			struct ClickActionPrivate {
				Qt::KeyboardModifiers modifiers;
				Qt::MouseButtons button;
				bool doubleClick;
				Qt::MouseButtons buttonsBefore; // only defined when doubleClick is true

				// This sort order in used in mouseString() to displays sorted mouse bindings
				bool operator<(const ClickActionPrivate& cap) const
				{
					if (buttonsBefore != cap.buttonsBefore)
						return buttonsBefore < cap.buttonsBefore;
					else
						if (modifiers != cap.modifiers)
							return modifiers < cap.modifiers;
						else
							if (button != cap.button)
								return button < cap.button;
							else
								return !doubleClick && cap.doubleClick;
				}
			};

			QMap<int, MouseActionPrivate> m_mouseBinding;
			QMap<Qt::KeyboardModifiers, MouseActionPrivate> m_wheelBinding;
			QMap<ClickActionPrivate, ClickAction> m_clickBinding;
			QMap<ClickActionPrivate, QString> m_mouseDescription;

		private:
			static QString MouseActionString(MouseAction ma);
			static QString ClickActionString(ClickAction ca);
			static QString FormatClickActionPrivate(ClickActionPrivate cap);
		public slots:
			void SetMouseBinding(int state, MouseHandler handler, MouseAction action, bool withConstraint=true);
			void SetMouseBinding(int state, ClickAction action, bool doubleClick=false, Qt::MouseButtons buttonsBefore=Qt::NoButton);
			void SetWheelBinding(Qt::KeyboardModifiers modifiers, MouseHandler handler, MouseAction action, bool withConstraint=true);
			void SetMouseBindingDescription(int state, QString description, bool doubleClick=false, Qt::MouseButtons buttonsBefore=Qt::NoButton);

		public:
			void DisplayMessage(const QString& message, int delay=2000);
			void DrawText(int x, int y, const QString& text, const QFont& fnt=QFont());
		private slots:
			void HideMessage();
		private:
			//屏幕提示信息
			QString m_message;
			bool m_isDisplayMessage;
			QTimer m_messageTimer;

			//辅助显示
		public:
			void DrawGrid(float size=1.0f, int nbSubdivisions=10);
			bool IsGridDrawn() const { return m_isGridDrawn; };
			void SetGridDrawnState(bool state);
		private:
			bool m_isGridDrawn;

			//屏幕快照
		public:
			void SnapshotToClipboard();
		//快照
		private:
			QImage FrameBufferSnapshot();

			//区域缩放
		public slots:
			void FitScreenRegion(const QRect& rectangle);
		private:
			void DrawRectangleZoom();
		private:
			QPoint m_prevPos; 
			QPoint m_pressPos;

			MouseAction m_action;

		//pick
		public Q_SLOTS:
			virtual void Pick(const QMouseEvent* event);
			virtual void Pick(const QPoint& point);

		//hud接口
		public:
			void SetHudtext(QString text);

		private:
			QString m_hudText;
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
			
		public:
			//使投影矩阵无效
			void DisableProjection();
			//使模型视图无效
			void DisableModelView();

			//计算鼠标方向
			Vector_3 MouseOrientation(unsigned iX, unsigned iY);

			//移动相机
			void MoveCamera(const Vector_3& v);

			//! 添加一个漫游器
			void AddManipulatorHandle(DCCore::GUIEventHandle* manipulator);

			//旋转视图
			void RotateView(const Matrix_4_4& matrix);

			//启用旋转中心
			void EnablePivot(bool bState);

			//转动滚轴
			void RotatingWheel(float fDegree);

			//! 获取视窗关联的camera
			Camera* GetCamera() const;

			MatrixPiple GetMatrixPiple() const;

			void SetMatrixPiple(MatrixPiple mpType);

			//更新相机位置
			void UpdateCameraPos(const Point_3& pos);

			void UpdateRotateView(const Matrix_4_4& rm);

			//! 当前按下按键容器
			const std::set<Qt::Key>& Keyboard() const { return m_Keyboard; }

			//! 指定按键是否按下
			bool IsKeyPressed(Qt::Key key) const { return m_Keyboard.find(key) != m_Keyboard.end(); }

			void UpdateFps(float deltaTime);

			//缩放
			void Scale(float fScale);


			
		private:
			//加载OpenGL扩展
			void LoadGLExtensions();

			//更新GLFilter
			bool UpdateGLFilter(unsigned uWidth, unsigned uHeight);

			//更新FBO
			bool UpdateFBO(unsigned uWidth, unsigned uHeight);
			//渲染FBO
			void RenderFBO();

			//更新Render
			void UpdateRender();
			//渲染背景
			void RenderBackground();

			//重新计算投影矩阵
			void RecalculateProjectionMatrix();
			//重新计算模型视图矩阵
			void RecalculateModelViewMatrix();
			//加载投影矩阵
			void LoadProjectionMatrix();
			//加载模型视图矩阵
			void LoadModelViewMatrix();

			//重新计算投影矩阵
			void RecalculateProjectionMatrixByCamera();
			//重新计算模型视图矩阵
			void RecalculateModelViewMatrixByCamera();

			
			//设置缩放因子
			void SetScale(float fScale);
			
			//计算透视投影缩放比例
			float ComputePerspectiveZoom() const;

			//标记重绘
			void MarkRefreshing();

			

			//绘制坐标轴
			void RenderAxises();
			//绘制轴线
			void DrawAxis();
			//绘制旋转中心
			void RenderPivot();

			//启用太阳光
			void EnableSunLight();
			//关闭太阳光
			void DisableSunLight();
			//启用自定义光
			void EnableCustomLight();
			//关闭自定义光
			void DisableCustomLight();

			//计算生成视景体参数
			void CalculateFrustum();

			void NormalizePlane(std::array<std::array<float, 4> ,6> frustum, int side);
		private:
			//接口
			View* m_pInterface;

			//OpenGL环境大小
			GLuint m_glWidth;
			GLuint m_glHeight;

			//View参数
			ViewerParams m_params;

			//GLFilter
			GLFilterPtr m_pGLFilter;

			//是否启用太阳光
			bool m_bEnableSunLight;
			//太阳光位置
			float m_sunLightPos[4];
			//是否启用自定义光
			bool m_bEnableCustomLight;
			//自定义光位置
			float m_customLightPos[4];

			//是否使用FBO
			bool m_bUsingFBO;
			//FBO
			FrameBufferPtr m_pFBO;
			//更新FBO
			bool m_bUpdatedFBO;

			//是否显示旋转符号
			bool m_bPivotShown;

			//! 交互状态
			View::InterActionMode m_interActionMode;

			//是否采用lod，三维浏览
			bool m_isApplyLodForTransfrom;

			//投影矩阵是否有效
			bool m_bValidProjection;
			//模型视图矩阵是否有效
			bool m_bValidModelView;

			//鼠标是否在移动中
			bool m_bMouseMoving;
			//鼠标上一次的位置
			QPoint m_lastPosOfMouse;
			//鼠标上一次的方向
			Vector_3 m_lastOrientationOfMouse;
			//鼠标当前党项
			Vector_3 m_currentOrientationOfMouse;

			//是否标记重绘
			bool m_bRefreshMarked;

			//坐标轴的显示列表
			GLuint m_axisListID;
			//旋转符号显示列表
			GLuint m_pivotListID;

			//初始状态下边界盒与camera距离
			float m_initdist_box_camer;

			//视景体六个面参数
			std::array<std::array<float, 4> ,6> m_Frustum;

			//help widget
			QTabWidget* m_helpWidget;

			// S h o r t c u t   k e y s
			QMap<KeyboardAction, QString> m_keyboardActionDescription;
			QMap<KeyboardAction, int> m_keyboardBinding;
			QMap<int, QString> m_keyDescription;

			//内部创建并管理的节点
			DcGp::DcGpEntity* m_innerRoot;

			std::vector<Point_3 > m_rayPoints;

			//! 漫游器
			std::vector<DCCore::GUIEventHandle* > m_manipulatorHandles;

			//! camera--测试camera类，后续用来替代gl代码
			MatrixPiple m_matrixPiple;

			DCCore::Transform* m_transForm;
			Camera* m_camera;

			std::set<Qt::Key> m_Keyboard;

			//! 窗口动态帧绘制控制
			int    m_refreshRate;
			QTimer m_updateTimer;

			bool m_continuousUpdate;

			//!帧速率
			float m_cfps;
		};
	}

#endif