#include "PointSize.h"

using namespace DCGp;

PointSize::PointSize()
	: m_size(1)
{
}

PointSize::PointSize(float size)
	: m_size(size)
{
	
}


PointSize::~PointSize()
{
}

void PointSize::SetSize(float size)
{
	m_size = size;
}

void PointSize::Apply(State&) const
{
	glPointSize(m_size);
}
