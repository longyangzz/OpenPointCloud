#include "DCGp/GpBasicDevice.h"

//Qt
#include "QString"
#include "QDataStream"

//Common
#include "DCCore/Const.h"

namespace DcGp
{
	DECLARE_SMARTPTR(DcGpViewportParametersImpl)
	
	class DcGpViewportParametersImpl
	{
		DECLARE_PHONY_CONSTRUCTOR(DcGpViewportParametersImpl)

	protected:
		DcGpViewportParametersImpl()
			: m_bPerspective(true)
		{}

	private:
		friend class DcGpViewportParameters;

		float m_pixeSize;

		//当前的缩放比例
		float m_zoom;

		//视图矩阵，仅旋转矩阵R
		DcGpMatrix m_viewRotateMat;

		//点大小
		float m_pointSize;

		//直线的宽度
		float m_lineWidth;

		//透视投影状态
		bool m_bPerspective;
		bool m_bObjectCentered;

		//旋转中心点 (for object-centered view modes)
		DCVector3D m_pivotPos;

		//相机中心（仅对透视模式）
		DCVector3D m_cameraPos;

		//! Camera F.O.V. (field of view - for perspective mode only)
		float m_fov;
		//相机的缩放比例(仅透视模式)
		float m_aspect;

		//! Theoretical perspective 'zNear' relative position
		double m_zNearCoef;
		//! Actual perspective 'zNear' value
		double m_zNear;
		//! Actual perspective 'zFar' value
		double m_zFar;
	};
}

DcGp::DcGpViewportParameters::DcGpViewportParameters()
{
	m_pDcGpViewportParametersImpl = DcGpViewportParametersImpl::CreateObject();

	m_pDcGpViewportParametersImpl->m_pixeSize = 1.0f;
	m_pDcGpViewportParametersImpl->m_zoom = 1.0f;
	m_pDcGpViewportParametersImpl->m_pointSize = 1.0f;
	m_pDcGpViewportParametersImpl->m_lineWidth = 1.0f;
	m_pDcGpViewportParametersImpl->m_bPerspective = true;
	m_pDcGpViewportParametersImpl->m_bObjectCentered = true;
	m_pDcGpViewportParametersImpl->m_pivotPos = 0.0f;
	m_pDcGpViewportParametersImpl->m_cameraPos = 0.0f;
	m_pDcGpViewportParametersImpl->m_fov = 30.0f;
	m_pDcGpViewportParametersImpl->m_aspect = 1.0f;
	m_pDcGpViewportParametersImpl->m_viewRotateMat.ToIdentity();
	m_pDcGpViewportParametersImpl->m_zNearCoef = 0.001;
	m_pDcGpViewportParametersImpl->m_zNear = 0;
	m_pDcGpViewportParametersImpl->m_zFar = 0;
}

DcGp::DcGpViewportParameters::DcGpViewportParameters(const DcGpViewportParameters& params)
{
	m_pDcGpViewportParametersImpl = DcGpViewportParametersImpl::CreateObject();

	m_pDcGpViewportParametersImpl->m_pixeSize = params.m_pDcGpViewportParametersImpl->m_pixeSize;
	m_pDcGpViewportParametersImpl->m_zoom = params.m_pDcGpViewportParametersImpl->m_zoom;
	m_pDcGpViewportParametersImpl->m_pointSize = params.m_pDcGpViewportParametersImpl->m_pointSize;
	m_pDcGpViewportParametersImpl->m_lineWidth = params.m_pDcGpViewportParametersImpl->m_lineWidth;
	m_pDcGpViewportParametersImpl->m_bPerspective = params.m_pDcGpViewportParametersImpl->m_bPerspective;
	m_pDcGpViewportParametersImpl->m_bObjectCentered = params.m_pDcGpViewportParametersImpl->m_bObjectCentered;
	m_pDcGpViewportParametersImpl->m_pivotPos = params.m_pDcGpViewportParametersImpl->m_pivotPos;
	m_pDcGpViewportParametersImpl->m_cameraPos = params.m_pDcGpViewportParametersImpl->m_cameraPos;
	m_pDcGpViewportParametersImpl->m_fov = params.m_pDcGpViewportParametersImpl->m_fov;
	m_pDcGpViewportParametersImpl->m_aspect = params.m_pDcGpViewportParametersImpl->m_aspect;
	m_pDcGpViewportParametersImpl->m_viewRotateMat = params.m_pDcGpViewportParametersImpl->m_viewRotateMat;
}


//像素大小
float& DcGp::DcGpViewportParameters::PixeSize()
{
	return m_pDcGpViewportParametersImpl->m_pixeSize;
}

//缩放比例
float& DcGp::DcGpViewportParameters::Zoom()
{
	return m_pDcGpViewportParametersImpl->m_zoom;
}

//视图矩阵的旋转矩阵
DcGp::DcGpMatrix& DcGp::DcGpViewportParameters::ViewRotateMatrix()
{
	return m_pDcGpViewportParametersImpl->m_viewRotateMat;
}

//点大小
float& DcGp::DcGpViewportParameters::PointSize()
{
	return m_pDcGpViewportParametersImpl->m_pointSize;
}

//线宽
float& DcGp::DcGpViewportParameters::LineWidth()
{
	return m_pDcGpViewportParametersImpl->m_lineWidth;
}

//投影状态
bool& DcGp::DcGpViewportParameters::Perspective()
{
	return m_pDcGpViewportParametersImpl->m_bPerspective;
}

bool& DcGp::DcGpViewportParameters::ObjCenterd()
{
	return m_pDcGpViewportParametersImpl->m_bObjectCentered;
}

//旋转中心
DCVector3D& DcGp::DcGpViewportParameters::PivotPosition()
{
	return m_pDcGpViewportParametersImpl->m_pivotPos;
}

//相机位置
DCVector3D& DcGp::DcGpViewportParameters::CameraPosition()
{
	return m_pDcGpViewportParametersImpl->m_cameraPos;
}

//
float& DcGp::DcGpViewportParameters::Fov()
{
	return m_pDcGpViewportParametersImpl->m_fov;
}

//
float& DcGp::DcGpViewportParameters::Aspect()
{
	return m_pDcGpViewportParametersImpl->m_aspect;
}

void DcGp::DcGpViewportParameters::SaveFar(double far)
{
	m_pDcGpViewportParametersImpl->m_zFar = far;
}

void DcGp::DcGpViewportParameters::SaveNear(double near)
{
	m_pDcGpViewportParametersImpl->m_zNear = near;
}

double DcGp::DcGpViewportParameters::GetNear()
{
	return m_pDcGpViewportParametersImpl->m_zNear;
}

double DcGp::DcGpViewportParameters::GetFar()
{
	return m_pDcGpViewportParametersImpl->m_zFar;
}