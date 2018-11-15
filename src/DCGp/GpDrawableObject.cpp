#include "GpDrawableObject.h"

//Qt
#include "QtGui/QPixmap"
#include "QtWidgets/QColorDialog"
#include "QtWidgets/QLabel"
#include "QString"
#include "QtGui/QIcon"

//common
#include "DCCore/Logger.h"

#include "DCGp/Custom/GpBoundBox.h"
#include "DCGp/GpBasicDevice.h"
#include "DCGp/GpDrawContext.h"

namespace DcGp
{
	DECLARE_SMARTPTR(DcGpDrawableObjectImpl)

	class DcGpDrawableObjectImpl
	{
		DECLARE_PHONY_CONSTRUCTOR(DcGpDrawableObjectImpl)

	protected:
		DcGpDrawableObjectImpl()
		{}

	private:
		friend class DcGpDrawableObject;

		//关联的窗体
		DcGpBasicDevice* m_associatedWindow;
		//颜色
		QColor m_color;
		//颜色设置按钮
		QPushButton* m_pbtnColor;

		//颜色模式
		ColorMode m_colorMode;

		//
		bool m_visible;

		//对象是否被选择
		bool m_selected;

		//是否锁定显示（如锁定则不可更改）
		bool m_lockedVisibility;


		//是否显示颜色
		bool m_colorsDisplayed;
		//是否显示法线
		bool m_normalsDisplayed;
		//是否显示标量场
		bool m_scalarFieldDisplayed;

		//临时颜色
		RGBColor m_tempColor;
		//临时颜色的激活状态
		bool m_colorIsOverriden;

		//
		DcGpMatrix m_glTrans;

		//
		bool m_glTransEnabled;

		//是否显示名字
		bool m_showName;

		//是否显示颜色条
		bool m_sfColorBar;
	};
}

DcGp::DcGpDrawableObject::DcGpDrawableObject()
	: DcGpObject()
{
	m_pDcGpDrawableObjectImpl = DcGpDrawableObjectImpl::CreateObject();

	m_pDcGpDrawableObjectImpl->m_color = QColor(255, 255, 255);
	m_pDcGpDrawableObjectImpl->m_pbtnColor = nullptr;
	m_pDcGpDrawableObjectImpl->m_selected = false;
	m_pDcGpDrawableObjectImpl->m_visible = true;
	m_pDcGpDrawableObjectImpl->m_colorsDisplayed = false;
	m_pDcGpDrawableObjectImpl->m_lockedVisibility = false;
	m_pDcGpDrawableObjectImpl->m_normalsDisplayed = false;
	m_pDcGpDrawableObjectImpl->m_scalarFieldDisplayed = false;
	m_pDcGpDrawableObjectImpl->m_associatedWindow = nullptr;
	m_pDcGpDrawableObjectImpl->m_colorIsOverriden = false;
	m_pDcGpDrawableObjectImpl->m_showName = false;
	m_pDcGpDrawableObjectImpl->m_colorMode = eTrueLinearColor;

	//设置默认的临时颜色
	m_pDcGpDrawableObjectImpl->m_tempColor[0] = 255;
	m_pDcGpDrawableObjectImpl->m_tempColor[1] = 255;
	m_pDcGpDrawableObjectImpl->m_tempColor[2] = 255;
}

DcGp::DcGpDrawableObject::DcGpDrawableObject(QString name)
	: DcGpObject(name)
{
	m_pDcGpDrawableObjectImpl = DcGpDrawableObjectImpl::CreateObject();

	m_pDcGpDrawableObjectImpl->m_color = QColor(255, 255, 255);
	m_pDcGpDrawableObjectImpl->m_pbtnColor = nullptr;
	m_pDcGpDrawableObjectImpl->m_selected = false;
	m_pDcGpDrawableObjectImpl->m_visible = true;
	m_pDcGpDrawableObjectImpl->m_colorsDisplayed = false;
	m_pDcGpDrawableObjectImpl->m_lockedVisibility = false;
	m_pDcGpDrawableObjectImpl->m_normalsDisplayed = false;
	m_pDcGpDrawableObjectImpl->m_scalarFieldDisplayed = false;
	m_pDcGpDrawableObjectImpl->m_associatedWindow = nullptr;
	m_pDcGpDrawableObjectImpl->m_colorIsOverriden = false;
	m_pDcGpDrawableObjectImpl->m_showName = false;
	m_pDcGpDrawableObjectImpl->m_colorMode = eTrueLinearColor;

	//设置默认的临时颜色
	m_pDcGpDrawableObjectImpl->m_tempColor[0] = 255;
	m_pDcGpDrawableObjectImpl->m_tempColor[1] = 255;
	m_pDcGpDrawableObjectImpl->m_tempColor[2] = 255;
}

DcGp::DcGpDrawableObject::~DcGpDrawableObject()
{
	//delete m_pDcGpDrawableObjectImpl.get();
}

DcGp::ColorMode DcGp::DcGpDrawableObject::GetColorMode() const
{
	return m_pDcGpDrawableObjectImpl->m_colorMode;
}

void DcGp::DcGpDrawableObject::SetColorMode(ColorMode mode)
{
	m_pDcGpDrawableObjectImpl->m_colorMode = mode;
}

QColor DcGp::DcGpDrawableObject::GetColor()
{
	return m_pDcGpDrawableObjectImpl->m_color;
}

DcGp::DcGpBasicDevice* DcGp::DcGpDrawableObject::GetAssociatedWindow() const
{
	return m_pDcGpDrawableObjectImpl->m_associatedWindow;
}

//
DcGp::DcGpBoundBox DcGp::DcGpDrawableObject::GetBoundBox(bool relative/* =true */, bool withGLfeatures/* =false */, const DcGpBasicDevice* window/* =0 */)
{
	DcGpBoundBox box;
	return box;
}

DcGp::DcGpBoundBox DcGp::DcGpDrawableObject::GetFitBoundBox(DcGpMatrix& trans)
{
	DcGpBoundBox box;
	return box;
}

void DcGp::DcGpDrawableObject::DrawBoundBox(const ColorType col[])
{
	GetBoundBox(true, false, m_pDcGpDrawableObjectImpl->m_associatedWindow).Draw(col);
}

//设置临时颜色
void DcGp::DcGpDrawableObject::SetTempColor(const RGBColor color, bool autoActivate /*= true*/)
{
	m_pDcGpDrawableObjectImpl->m_tempColor = color;

	if (autoActivate)
		EnableTempColor(true);
}

//是否允许临时颜色
void DcGp::DcGpDrawableObject::EnableTempColor(bool state)
{
	m_pDcGpDrawableObjectImpl->m_colorIsOverriden = state;
	if (state)
	{
		m_pDcGpDrawableObjectImpl->m_colorMode = eSingleColor;
	}
}

//是否重新使用临时颜色
bool DcGp::DcGpDrawableObject::IsColorOverriden() const
{
	return m_pDcGpDrawableObjectImpl->m_colorIsOverriden;
}

//获取临时颜色
const RGBColor DcGp::DcGpDrawableObject::GetTempColor() const
{
	return m_pDcGpDrawableObjectImpl->m_tempColor;
}

//显示颜色
void DcGp::DcGpDrawableObject::ShowColors(bool state)
{
	m_pDcGpDrawableObjectImpl->m_colorsDisplayed = state;
	EnableTempColor(!state);
}

//切换颜色显示状态
void DcGp::DcGpDrawableObject::ToggleColors()
{
	ShowColors(!ColorsShown());
}

//颜色的显示状态
bool DcGp::DcGpDrawableObject::ColorsShown() const
{
	return m_pDcGpDrawableObjectImpl->m_colorsDisplayed;
}

//是否显示颜色
bool DcGp::DcGpDrawableObject::HasColors() const
{
	return ColorsShown();
}

//显示法线
void DcGp::DcGpDrawableObject::ShowNormals(bool state)
{
	m_pDcGpDrawableObjectImpl->m_normalsDisplayed = state;
}

//切换法线显示状态
void DcGp::DcGpDrawableObject::ToggleNormals()
{
	ShowNormals(!NormalsShown());
}

//法线的显示状态
bool DcGp::DcGpDrawableObject::NormalsShown() const
{
	return m_pDcGpDrawableObjectImpl->m_normalsDisplayed;
}

//是否显示法线
bool DcGp::DcGpDrawableObject::HasNormals() const
{
	return false;
}

//是否显示标量场
bool DcGp::DcGpDrawableObject::HasScalarFields() const
{
	return false;
}

//显示标量场
void DcGp::DcGpDrawableObject::ShowScalarField(bool state)
{
	m_pDcGpDrawableObjectImpl->m_scalarFieldDisplayed = state;
}

//切换标量场
void DcGp::DcGpDrawableObject::ToggleScalarField()
{
	ShowScalarField(!ScalarFieldShown());
}

void DcGp::DcGpDrawableObject::Refresh()
{
	if (m_pDcGpDrawableObjectImpl->m_associatedWindow)
	{
		m_pDcGpDrawableObjectImpl->m_associatedWindow->Redraw();
	}
}

//标量场的显示状态
bool DcGp::DcGpDrawableObject::ScalarFieldShown() const
{
	return m_pDcGpDrawableObjectImpl->m_scalarFieldDisplayed;
}

//获取显示参数
void DcGp::DcGpDrawableObject::GetDrawingParameters(glDrawParameters& params) const
{
	//颜色重写
	if (IsColorOverriden())
	{
		params.showColors=true;
		params.showNormals=false;
		params.showScalarField=false;
	}
	else
	{
		//a scalar field must have been selected for display!
		params.showScalarField = HasScalarFields() && ScalarFieldShown();
		params.showNormals = HasNormals() &&  NormalsShown();
		//colors are not displayed if scalar field is displayed
		params.showColors = !params.showScalarField && HasColors() && ColorsShown();
	}
}

//设置被选择
void DcGp::DcGpDrawableObject::SetSelected(bool status)
{
	m_pDcGpDrawableObjectImpl->m_selected = status;
}

//是否被选择
bool DcGp::DcGpDrawableObject::IsSelected() const
{
	return m_pDcGpDrawableObjectImpl->m_selected;
}

//
void DcGp::DcGpDrawableObject::PrepareDisplayForRefresh()
{
	if (m_pDcGpDrawableObjectImpl->m_associatedWindow)
	{
		m_pDcGpDrawableObjectImpl->m_associatedWindow->ToBeRefreshed();
	}
}

//是否可见
bool DcGp::DcGpDrawableObject::IsVisible() const
{
	return m_pDcGpDrawableObjectImpl->m_visible;
}

//设置可见性
void DcGp::DcGpDrawableObject::SetVisible(bool status)
{
	m_pDcGpDrawableObjectImpl->m_visible = status;
}

//可见性是否锁定
bool DcGp::DcGpDrawableObject::IsVisibilityLocked() const
{
	return m_pDcGpDrawableObjectImpl->m_lockedVisibility;
}

//锁定可见性
void DcGp::DcGpDrawableObject::LockVisibility(bool status)
{
	m_pDcGpDrawableObjectImpl->m_lockedVisibility = status;
}

//是否显示名字
bool DcGp::DcGpDrawableObject::IsShownName() const
{
	return m_pDcGpDrawableObjectImpl->m_showName;
}

//显示名字
void DcGp::DcGpDrawableObject::ShowName(bool status)
{
	m_pDcGpDrawableObjectImpl->m_showName = status;
}

//
DcGp::DcGpMatrix DcGp::DcGpDrawableObject::GetTransMat() const
{
	return m_pDcGpDrawableObjectImpl->m_glTrans;
}

void DcGp::DcGpDrawableObject::SetTransMat(DcGpMatrix mat)
{
	m_pDcGpDrawableObjectImpl->m_glTrans = mat;
}

bool DcGp::DcGpDrawableObject::IsTransformed() const
{
	return m_pDcGpDrawableObjectImpl->m_glTransEnabled;
}

void DcGp::DcGpDrawableObject::SetTransformStatus(bool status)
{
	m_pDcGpDrawableObjectImpl->m_glTransEnabled = status;
}

void DcGp::DcGpDrawableObject::SetAssociatedWindow(DcGpBasicDevice* window)
{
	m_pDcGpDrawableObjectImpl->m_associatedWindow = window;
}

//显示颜色条
void DcGp::DcGpDrawableObject::SetSfColorBarVisibility(bool state)
{
	m_pDcGpDrawableObjectImpl->m_sfColorBar = state;
}

bool DcGp::DcGpDrawableObject::IsSfColorbarVisible() const
{
	return m_pDcGpDrawableObjectImpl->m_sfColorBar;
}

unsigned DcGp::DcGpDrawableObject::ScalarFieldCount() const
{
	return 0;
}

QStringList DcGp::DcGpDrawableObject::ScalarFieldNames() const
{
	return QStringList();
}

void DcGp::DcGpDrawableObject::ApplyTransform(DcGpMatrix& transMatrix)
{
	DCCore::Logger::Warning(tr("Transform Invalid!"));
}

void DcGp::DcGpDrawableObject::ApplyTransform(EigenMatrix4d& transMatrix)
{
	DCCore::Logger::Warning(tr("Transform Invalid!"));
}