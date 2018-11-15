//C++
#include <cmath>

#include "DCGp/Custom/GpBoundBox.h"
#include "DCCore/DCGL.h"

namespace DcGp
{
	DECLARE_SMARTPTR(DcGpBoundBoxImpl)

	class DcGpBoundBoxImpl
	{
		DECLARE_PHONY_CONSTRUCTOR(DcGpBoundBoxImpl)

	protected:
		DcGpBoundBoxImpl()
			: m_bValid(false)
		{}
	private:
		friend class DcGpBoundBox;

		//最小值角点
		DCVector3D m_minCorner;
		//最大值角点
		DCVector3D m_maxCorner;
		//有效性
		bool m_bValid;
	};
}

DcGp::DcGpBoundBox::DcGpBoundBox()
{
	m_pDcGpBoundBoxImpl = DcGpBoundBoxImpl::CreateObject();

	Clear();
}

DcGp::DcGpBoundBox::DcGpBoundBox(const DCVector3D &bbMinCorner, const DCVector3D &bbMaxCorner)
{
	m_pDcGpBoundBoxImpl = DcGpBoundBoxImpl::CreateObject();

	m_pDcGpBoundBoxImpl->m_minCorner = bbMinCorner;
	m_pDcGpBoundBoxImpl->m_maxCorner = bbMaxCorner;
	m_pDcGpBoundBoxImpl->m_bValid = true;
}

DcGp::DcGpBoundBox::DcGpBoundBox(const DcGpBoundBox& aBox)
{
	m_pDcGpBoundBoxImpl = DcGpBoundBoxImpl::CreateObject();

	m_pDcGpBoundBoxImpl->m_minCorner = aBox.m_pDcGpBoundBoxImpl->m_minCorner;
	m_pDcGpBoundBoxImpl->m_maxCorner = aBox.m_pDcGpBoundBoxImpl->m_maxCorner;
	m_pDcGpBoundBoxImpl->m_bValid = aBox.m_pDcGpBoundBoxImpl->m_bValid;
}

void DcGp::DcGpBoundBox::Clear()
{
	m_pDcGpBoundBoxImpl->m_minCorner.x = 
		m_pDcGpBoundBoxImpl->m_minCorner.y = 
		m_pDcGpBoundBoxImpl->m_minCorner.z = 0.0;

	m_pDcGpBoundBoxImpl->m_maxCorner.x = 
		m_pDcGpBoundBoxImpl->m_maxCorner.y = 
		m_pDcGpBoundBoxImpl->m_maxCorner.z = 0.0;

	m_pDcGpBoundBoxImpl->m_bValid = false;
}

DCVector3D DcGp::DcGpBoundBox::GetCenter() const
{
	return (m_pDcGpBoundBoxImpl->m_maxCorner + 
			m_pDcGpBoundBoxImpl->m_minCorner) * 0.5;
}

DCVector3D DcGp::DcGpBoundBox::GetDiagVector() const
{
	return m_pDcGpBoundBoxImpl->m_maxCorner - 
			m_pDcGpBoundBoxImpl->m_minCorner;
}

PointCoordinateType DcGp::DcGpBoundBox::GetDiagNormal() const
{
	return GetDiagVector().Normal();
}

void DcGp::DcGpBoundBox::SetValidity(bool state)
{
	m_pDcGpBoundBoxImpl->m_bValid = state;
}

bool DcGp::DcGpBoundBox::IsValid() const
{
	return m_pDcGpBoundBoxImpl->m_bValid;
}

const DCVector3D& DcGp::DcGpBoundBox::MinCorner() const
{
	return m_pDcGpBoundBoxImpl->m_minCorner;
}

const DCVector3D& DcGp::DcGpBoundBox::MaxCorner() const
{
	return m_pDcGpBoundBoxImpl->m_maxCorner;
}

DCVector3D& DcGp::DcGpBoundBox::MinCorner()
{
	return m_pDcGpBoundBoxImpl->m_minCorner;
}

DCVector3D& DcGp::DcGpBoundBox::MaxCorner()
{
	return m_pDcGpBoundBoxImpl->m_maxCorner;
}

void DcGp::DcGpBoundBox::Draw(const ColorType col[]) const
{
	if (!m_pDcGpBoundBoxImpl->m_bValid)
		return;

	glColor3ubv(col);

	glBegin(GL_LINE_LOOP);
	glVertex3fv(m_pDcGpBoundBoxImpl->m_minCorner.u);
	glVertex3f(m_pDcGpBoundBoxImpl->m_maxCorner.x,m_pDcGpBoundBoxImpl->m_minCorner.y,m_pDcGpBoundBoxImpl->m_minCorner.z);
	glVertex3f(m_pDcGpBoundBoxImpl->m_maxCorner.x,m_pDcGpBoundBoxImpl->m_maxCorner.y,m_pDcGpBoundBoxImpl->m_minCorner.z);
	glVertex3f(m_pDcGpBoundBoxImpl->m_minCorner.x,m_pDcGpBoundBoxImpl->m_maxCorner.y,m_pDcGpBoundBoxImpl->m_minCorner.z);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3f(m_pDcGpBoundBoxImpl->m_minCorner.x,m_pDcGpBoundBoxImpl->m_minCorner.y,m_pDcGpBoundBoxImpl->m_maxCorner.z);
	glVertex3f(m_pDcGpBoundBoxImpl->m_maxCorner.x,m_pDcGpBoundBoxImpl->m_minCorner.y,m_pDcGpBoundBoxImpl->m_maxCorner.z);
	glVertex3fv(m_pDcGpBoundBoxImpl->m_maxCorner.u);
	glVertex3f(m_pDcGpBoundBoxImpl->m_minCorner.x,m_pDcGpBoundBoxImpl->m_maxCorner.y,m_pDcGpBoundBoxImpl->m_maxCorner.z);
	glEnd();

	glBegin(GL_LINES);
	glVertex3fv(m_pDcGpBoundBoxImpl->m_minCorner.u);
	glVertex3f(m_pDcGpBoundBoxImpl->m_minCorner.x,m_pDcGpBoundBoxImpl->m_minCorner.y,m_pDcGpBoundBoxImpl->m_maxCorner.z);
	glVertex3f(m_pDcGpBoundBoxImpl->m_maxCorner.x,m_pDcGpBoundBoxImpl->m_minCorner.y,m_pDcGpBoundBoxImpl->m_minCorner.z);
	glVertex3f(m_pDcGpBoundBoxImpl->m_maxCorner.x,m_pDcGpBoundBoxImpl->m_minCorner.y,m_pDcGpBoundBoxImpl->m_maxCorner.z);
	glVertex3f(m_pDcGpBoundBoxImpl->m_maxCorner.x,m_pDcGpBoundBoxImpl->m_maxCorner.y,m_pDcGpBoundBoxImpl->m_minCorner.z);
	glVertex3fv(m_pDcGpBoundBoxImpl->m_maxCorner.u);
	glVertex3f(m_pDcGpBoundBoxImpl->m_minCorner.x,m_pDcGpBoundBoxImpl->m_maxCorner.y,m_pDcGpBoundBoxImpl->m_minCorner.z);
	glVertex3f(m_pDcGpBoundBoxImpl->m_minCorner.x,m_pDcGpBoundBoxImpl->m_maxCorner.y,m_pDcGpBoundBoxImpl->m_maxCorner.z);
	glEnd();
}

DcGp::DcGpBoundBox DcGp::DcGpBoundBox::operator + (const DcGpBoundBox& aBBox) const
{
	if (!m_pDcGpBoundBoxImpl->m_bValid)
		return DcGpBoundBox(aBBox);

	DcGpBoundBox tempBox;

	tempBox.m_pDcGpBoundBoxImpl->m_minCorner.x = std::min(m_pDcGpBoundBoxImpl->m_minCorner.x, 
														aBBox.m_pDcGpBoundBoxImpl->m_minCorner.x);

	tempBox.m_pDcGpBoundBoxImpl->m_minCorner.y = std::min(m_pDcGpBoundBoxImpl->m_minCorner.y, 
														aBBox.m_pDcGpBoundBoxImpl->m_minCorner.y);

	tempBox.m_pDcGpBoundBoxImpl->m_minCorner.z = std::min(m_pDcGpBoundBoxImpl->m_minCorner.z, 
														aBBox.m_pDcGpBoundBoxImpl->m_minCorner.z);

	tempBox.m_pDcGpBoundBoxImpl->m_maxCorner.x = std::max(m_pDcGpBoundBoxImpl->m_maxCorner.x, 
														aBBox.m_pDcGpBoundBoxImpl->m_maxCorner.x);

	tempBox.m_pDcGpBoundBoxImpl->m_maxCorner.y = std::max(m_pDcGpBoundBoxImpl->m_maxCorner.y, 
														aBBox.m_pDcGpBoundBoxImpl->m_maxCorner.y);

	tempBox.m_pDcGpBoundBoxImpl->m_maxCorner.z = std::max(m_pDcGpBoundBoxImpl->m_maxCorner.z, 
														aBBox.m_pDcGpBoundBoxImpl->m_maxCorner.z);

	return tempBox;
}

const DcGp::DcGpBoundBox& DcGp::DcGpBoundBox::operator += (const DcGpBoundBox& aBBox)
{
	if (!m_pDcGpBoundBoxImpl->m_bValid)
	{
		*this = aBBox;
	}
	else if (aBBox.IsValid())
	{
		m_pDcGpBoundBoxImpl->m_minCorner.x = std::min(m_pDcGpBoundBoxImpl->m_minCorner.x, 
													aBBox.m_pDcGpBoundBoxImpl->m_minCorner.x);

		m_pDcGpBoundBoxImpl->m_minCorner.y = std::min(m_pDcGpBoundBoxImpl->m_minCorner.y, 
													aBBox.m_pDcGpBoundBoxImpl->m_minCorner.y);

		m_pDcGpBoundBoxImpl->m_minCorner.z = std::min(m_pDcGpBoundBoxImpl->m_minCorner.z, 
													aBBox.m_pDcGpBoundBoxImpl->m_minCorner.z);

		m_pDcGpBoundBoxImpl->m_maxCorner.x = std::max(m_pDcGpBoundBoxImpl->m_maxCorner.x, 
													aBBox.m_pDcGpBoundBoxImpl->m_maxCorner.x);

		m_pDcGpBoundBoxImpl->m_maxCorner.y = std::max(m_pDcGpBoundBoxImpl->m_maxCorner.y, 
													aBBox.m_pDcGpBoundBoxImpl->m_maxCorner.y);

		m_pDcGpBoundBoxImpl->m_maxCorner.z = std::max(m_pDcGpBoundBoxImpl->m_maxCorner.z, 
													aBBox.m_pDcGpBoundBoxImpl->m_maxCorner.z);
	}

	return *this;
}

const DcGp::DcGpBoundBox& DcGp::DcGpBoundBox::operator += (const DCVector3D& aVector)
{
	if (m_pDcGpBoundBoxImpl->m_bValid)
	{
		m_pDcGpBoundBoxImpl->m_minCorner += aVector;
		m_pDcGpBoundBoxImpl->m_maxCorner += aVector;
	}

	return *this;
}

const DcGp::DcGpBoundBox& DcGp::DcGpBoundBox::operator -= (const DCVector3D& aVector)
{
	if (m_pDcGpBoundBoxImpl->m_bValid)
	{
		m_pDcGpBoundBoxImpl->m_minCorner -= aVector;
		m_pDcGpBoundBoxImpl->m_maxCorner -= aVector;
	}

	return *this;
}

const DcGp::DcGpBoundBox& DcGp::DcGpBoundBox::operator *= (const PointCoordinateType& scaleFactor)
{
	if (m_pDcGpBoundBoxImpl->m_bValid)
	{
		m_pDcGpBoundBoxImpl->m_minCorner *= scaleFactor;
		m_pDcGpBoundBoxImpl->m_maxCorner *= scaleFactor;
	}

	return *this;
}

void DcGp::DcGpBoundBox::Add(const DCVector3D& aVector)
{
	if (m_pDcGpBoundBoxImpl->m_bValid)
	{
		if (aVector.x<m_pDcGpBoundBoxImpl->m_minCorner.x)
			m_pDcGpBoundBoxImpl->m_minCorner.x = aVector.x;
		else if (aVector.x>m_pDcGpBoundBoxImpl->m_maxCorner.x)
			m_pDcGpBoundBoxImpl->m_maxCorner.x = aVector.x;

		if (aVector.y<m_pDcGpBoundBoxImpl->m_minCorner.y)
			m_pDcGpBoundBoxImpl->m_minCorner.y = aVector.y;
		else if (aVector.y>m_pDcGpBoundBoxImpl->m_maxCorner.y)
			m_pDcGpBoundBoxImpl->m_maxCorner.y = aVector.y;

		if (aVector.z<m_pDcGpBoundBoxImpl->m_minCorner.z)
			m_pDcGpBoundBoxImpl->m_minCorner.z = aVector.z;
		else if (aVector.z>m_pDcGpBoundBoxImpl->m_maxCorner.z)
			m_pDcGpBoundBoxImpl->m_maxCorner.z = aVector.z;
	}
	else
	{
		m_pDcGpBoundBoxImpl->m_maxCorner = m_pDcGpBoundBoxImpl->m_minCorner = aVector;
		m_pDcGpBoundBoxImpl->m_bValid = true;
	}
}

const DcGp::DcGpBoundBox& DcGp::DcGpBoundBox::operator *= (const DCCore::SquareMatrix& mat)
{
	if (m_pDcGpBoundBoxImpl->m_bValid)
	{
		DCVector3D boxCorners[8];

		boxCorners[0] = m_pDcGpBoundBoxImpl->m_minCorner;
		boxCorners[1] = DCVector3D(m_pDcGpBoundBoxImpl->m_minCorner.x,m_pDcGpBoundBoxImpl->m_minCorner.y,m_pDcGpBoundBoxImpl->m_maxCorner.z);
		boxCorners[2] = DCVector3D(m_pDcGpBoundBoxImpl->m_minCorner.x,m_pDcGpBoundBoxImpl->m_maxCorner.y,m_pDcGpBoundBoxImpl->m_minCorner.z);
		boxCorners[3] = DCVector3D(m_pDcGpBoundBoxImpl->m_maxCorner.x,m_pDcGpBoundBoxImpl->m_minCorner.y,m_pDcGpBoundBoxImpl->m_minCorner.z);
		boxCorners[4] = m_pDcGpBoundBoxImpl->m_maxCorner;
		boxCorners[5] = DCVector3D(m_pDcGpBoundBoxImpl->m_minCorner.x,m_pDcGpBoundBoxImpl->m_maxCorner.y,m_pDcGpBoundBoxImpl->m_maxCorner.z);
		boxCorners[6] = DCVector3D(m_pDcGpBoundBoxImpl->m_maxCorner.x,m_pDcGpBoundBoxImpl->m_maxCorner.y,m_pDcGpBoundBoxImpl->m_minCorner.z);
		boxCorners[7] = DCVector3D(m_pDcGpBoundBoxImpl->m_maxCorner.x,m_pDcGpBoundBoxImpl->m_minCorner.y,m_pDcGpBoundBoxImpl->m_maxCorner.z);

		Clear();

		for (int i=0;i<8;++i)
			Add(mat*boxCorners[i]);
	}

	return *this;
}

const DcGp::DcGpBoundBox& DcGp::DcGpBoundBox::operator *= (const DcGpMatrix& mat)
{
	if (m_pDcGpBoundBoxImpl->m_bValid)
	{
		DCVector3D boxCorners[8];

		boxCorners[0] = m_pDcGpBoundBoxImpl->m_minCorner;
		boxCorners[1] = DCVector3D(m_pDcGpBoundBoxImpl->m_minCorner.x,m_pDcGpBoundBoxImpl->m_minCorner.y,m_pDcGpBoundBoxImpl->m_maxCorner.z);
		boxCorners[2] = DCVector3D(m_pDcGpBoundBoxImpl->m_minCorner.x,m_pDcGpBoundBoxImpl->m_maxCorner.y,m_pDcGpBoundBoxImpl->m_minCorner.z);
		boxCorners[3] = DCVector3D(m_pDcGpBoundBoxImpl->m_maxCorner.x,m_pDcGpBoundBoxImpl->m_minCorner.y,m_pDcGpBoundBoxImpl->m_minCorner.z);
		boxCorners[4] = m_pDcGpBoundBoxImpl->m_maxCorner;
		boxCorners[5] = DCVector3D(m_pDcGpBoundBoxImpl->m_minCorner.x,m_pDcGpBoundBoxImpl->m_maxCorner.y,m_pDcGpBoundBoxImpl->m_maxCorner.z);
		boxCorners[6] = DCVector3D(m_pDcGpBoundBoxImpl->m_maxCorner.x,m_pDcGpBoundBoxImpl->m_maxCorner.y,m_pDcGpBoundBoxImpl->m_minCorner.z);
		boxCorners[7] = DCVector3D(m_pDcGpBoundBoxImpl->m_maxCorner.x,m_pDcGpBoundBoxImpl->m_minCorner.y,m_pDcGpBoundBoxImpl->m_maxCorner.z);

		Clear();

		for (int i=0;i<8;++i)
			Add(mat*boxCorners[i]);
	}

	return *this;
}

PointCoordinateType DcGp::DcGpBoundBox::MinDistTo(const DcGpBoundBox& box) const
{
	if (m_pDcGpBoundBoxImpl->m_bValid && box.IsValid())
	{
		DCVector3D d(0,0,0);

		for (unsigned char dim=0; dim<3; ++dim)
		{
			//if the boxes overlap in one dimension, the distance is zero (in this dimension)
			if (box.m_pDcGpBoundBoxImpl->m_minCorner.u[dim] > m_pDcGpBoundBoxImpl->m_maxCorner.u[dim])
				d.u[dim] = box.m_pDcGpBoundBoxImpl->m_minCorner.u[dim] - m_pDcGpBoundBoxImpl->m_maxCorner.u[dim];
			else if (box.m_pDcGpBoundBoxImpl->m_maxCorner.u[dim] < m_pDcGpBoundBoxImpl->m_minCorner.u[dim])
				d.x = m_pDcGpBoundBoxImpl->m_minCorner.u[dim] - box.m_pDcGpBoundBoxImpl->m_maxCorner.u[dim];
		}

		return d.Normal();
	}
	else
	{
		return (PointCoordinateType)-1.0;
	}
}

bool DcGp::DcGpBoundBox::Contains(const DCVector3D& P) const
{
	return (P.x >= m_pDcGpBoundBoxImpl->m_minCorner.x && P.x <= m_pDcGpBoundBoxImpl->m_maxCorner.x &&
		P.y >= m_pDcGpBoundBoxImpl->m_minCorner.y && P.y <= m_pDcGpBoundBoxImpl->m_maxCorner.y &&
		P.z >= m_pDcGpBoundBoxImpl->m_minCorner.z && P.z <= m_pDcGpBoundBoxImpl->m_maxCorner.z);
}