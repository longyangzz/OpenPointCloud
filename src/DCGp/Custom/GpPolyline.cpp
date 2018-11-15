#include "GpPolyline.h"

//
#include <vector>

namespace DcGp
{
	DECLARE_SMARTPTR(DcGpPolylineImpl)

	class DcGpPolylineImpl
	{
		DECLARE_PHONY_CONSTRUCTOR(DcGpPolylineImpl)

	protected:
		DcGpPolylineImpl()
			: m_isClosed(true)
		{
			m_globalColor[0] = 255;
			m_globalColor[1] = 255;
			m_globalColor[2] = 255;
		}

	private:
		friend class DcGpPolyline;

		//顶点
		std::vector<PointCoord> m_vertex;

		//全局颜色
		RGBColor m_globalColor;

		//闭合状态
		bool m_isClosed;
	};
}

DcGp::DcGpPolyline::DcGpPolyline()
{
	m_pDcGpPolylineImpl = DcGpPolylineImpl::CreateObject();
}

void DcGp::DcGpPolyline::AddPoint(PointCoord point) 
{
	m_pDcGpPolylineImpl->m_vertex.push_back(point);
}

PointCoord DcGp::DcGpPolyline::GetPoint(unsigned index) const
{
	return m_pDcGpPolylineImpl->m_vertex[index];
}

void DcGp::DcGpPolyline::GetPoint(unsigned index, DCVector3D& point) const
{
	point = DCVector3D(m_pDcGpPolylineImpl->m_vertex[index][0], 
		m_pDcGpPolylineImpl->m_vertex[index][1], 
		m_pDcGpPolylineImpl->m_vertex[index][2]);
}

void DcGp::DcGpPolyline::SetGlobalColor(RGBColor color) 
{
	m_pDcGpPolylineImpl->m_globalColor = color;
}

RGBColor DcGp::DcGpPolyline::GetGlobalColor() const 
{
	return m_pDcGpPolylineImpl->m_globalColor;
}

bool DcGp::DcGpPolyline::GetClosingState() const 
{
	return m_pDcGpPolylineImpl->m_isClosed;
}

void DcGp::DcGpPolyline::SetClosingState(bool state) 
{
	m_pDcGpPolylineImpl->m_isClosed = state;
}

void DcGp::DcGpPolyline::Clear() 
{
	m_pDcGpPolylineImpl->m_vertex.clear();
}

unsigned DcGp::DcGpPolyline::GetVertexNumber() const 
{
	return m_pDcGpPolylineImpl->m_vertex.size();
}

void DcGp::DcGpPolyline::SetPoints(std::vector<DCVector3D> pts)
{
	m_pDcGpPolylineImpl->m_vertex = pts;
}

std::vector<DCVector3D> DcGp::DcGpPolyline::GetPoints() const
{
	return m_pDcGpPolylineImpl->m_vertex;
}
void DcGp::DcGpPolyline::ChangePointByIndex(unsigned index, int x, int y)
{
	m_pDcGpPolylineImpl->m_vertex[index][0] = static_cast<PointCoordinateType>(x);
	m_pDcGpPolylineImpl->m_vertex[index][1] = static_cast<PointCoordinateType>(y);
}

void DcGp::DcGpPolyline::ChangePointByIndex(unsigned index, DCVector3D point)
{
	m_pDcGpPolylineImpl->m_vertex[index] = point;
}

void DcGp::DcGpPolyline::DeleteLastPoint()
{
	m_pDcGpPolylineImpl->m_vertex.pop_back();
}