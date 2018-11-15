#ifndef DCGRAPHICOBJECT_H
#define DCGRAPHICOBJECT_H

#include "DCGp/DcGpDLL.h"

//Qt
#include "QObject"
#include "QtGui/QColor"
#include "QString"
#include "QFile"

#include "Eigen/Dense"

class QPushButton;

//#include "DCCore/DCGL.h"
#include "DCGp/GpBasicTypes.h"
#include "GpObject.h"
//#include "DCGp/GpBasicDevice.h"
//#include "DCGp/GpDrawContext.h"

namespace DcGp
{
	class DcGpBoundBox;
	class DcGpDrawContext;
	class glDrawParameters;
	class DcGpBasicDevice;
	class DcGpMatrix;
	class DcGpColorRampShader;

	DECLARE_SMARTPTR(DcGpDrawableObject)
	DECLARE_SMARTPTR(DcGpDrawableObjectImpl)

	typedef Eigen::Matrix4d EigenMatrix4d;

	class DCGP_API DcGpDrawableObject : public QObject, public DcGpObject
	{
		Q_OBJECT

		DECLARE_IMPLEMENT(DcGpDrawableObject)
		GET_CLASSNAME(DcGpDrawableObject)

	public:
		//标量场类型
		enum ScalarFieldType
		{
			eScalarField		=	0	,
			eDistanceField				,
			eHeightField
		};

		DcGpDrawableObject(QString name);
		virtual ~DcGpDrawableObject();

		//绘制图形
		virtual void Draw(DcGpDrawContext& context) = 0;

		 //返回边界盒
		virtual DcGpBoundBox GetBoundBox(bool relative=true, 
										bool withGLfeatures=false, 
										const DcGpBasicDevice* window=0) = 0;

		//获取颜色
		virtual QColor GetColor();
		//显示属性
		virtual void ShowProperties(QTableWidget*) {}

		//
		virtual DcGpBoundBox GetFitBoundBox(DcGpMatrix& trans);

		//!
		virtual void DrawBoundBox(const ColorType col[]);

		//连接窗体
		virtual void LinkWindow(DcGpBasicDevice* window) = 0;

		//重绘
		void Refresh();
		DcGpBasicDevice* GetAssociatedWindow() const;
		void SetAssociatedWindow(DcGpBasicDevice* window);

		//获取绘制参数
		virtual void GetDrawingParameters(glDrawParameters& params) const;

		//临时颜色

		//当前颜色是否被临时颜色重写
		virtual bool IsColorOverriden() const;

		//! 唯一色
		virtual const RGBColor GetTempColor() const;

		//设置当前的临时颜色
		//参数一：颜色值
		//参数二：是否自动激活临时颜色
		virtual void SetTempColor(const RGBColor color, bool autoActivate = true);

		//是否允许使用临时颜色
		virtual void EnableTempColor(bool state);

		//是否使用颜色
		virtual bool HasColors() const;

		//是否显示颜色
		virtual bool ColorsShown() const;

		//设置颜色的显示状态
		virtual void ShowColors(bool state);

		//切换颜色的显示状态
		virtual void ToggleColors();

		//是否使用发现
		virtual bool HasNormals() const;

		//是否显示法线
		virtual bool NormalsShown() const;

		//设置法线的显示状态
		virtual void ShowNormals(bool state);

		//切换法线的状态
		virtual void ToggleNormals();

		//是否有标量场被实例化
		virtual bool HasScalarFields() const;

		//设置激活的标量场的显示状态
		virtual void ShowScalarField(bool state);

		//切换标量场的显示状态
		virtual void ToggleScalarField();

		//激活的标量场是否显示
		virtual bool ScalarFieldShown() const;

		//设置被选择
		void SetSelected(bool status);

		//是否被选择
		bool IsSelected() const;

		//
		void PrepareDisplayForRefresh();

		//是否可见
		bool IsVisible() const;

		//设置可见性
		void SetVisible(bool status);

		//可见性是否锁定
		bool IsVisibilityLocked() const;

		//锁定可见性
		void LockVisibility(bool status);

		//是否显示名字
		bool IsShownName() const;

		//显示名字
		void ShowName(bool status);

		//获取颜色模式
		ColorMode GetColorMode() const;
		//设置颜色模式
		void SetColorMode(ColorMode mode);

		//更新选取颜色
		virtual void RefreshChoosedColor() {}

		//更改标量场的颜色表
		virtual void ChangeScalarFieldColorScale(QString name) {}

		//获取标量场的数量
		virtual unsigned ScalarFieldCount() const;

		//获取标量场的名字
		virtual QStringList ScalarFieldNames() const;

		virtual int GetCurrentScalarFieldIndex() {return -1;}
		//设置当前标量场
		virtual void SetCurrentScalarField(unsigned index, unsigned setps, ScalarFieldType type = eScalarField){}
	
		//转换矩阵
		DcGpMatrix GetTransMat() const;
		void SetTransMat(DcGpMatrix mat);

		virtual void ApplyTransform(EigenMatrix4d& transMatrix);
		virtual void ApplyTransform(DcGpMatrix& transMatrix);

		//
		bool IsTransformed() const;
		void SetTransformStatus(bool status);

		//!brief 设置更新颜色条的显示状态
		virtual void SetSfColorBarVisibility(bool status /*!<[in] 传入的状态值 */);

		//!\brief 返回颜色条是否可见状态值
		//!\retval 返回颜色条状态值
		bool IsSfColorbarVisible() const;

	protected:
		DcGpDrawableObject();

	private slots:
	
	};

}
#endif // DCGRAPHICOBJECT_H
