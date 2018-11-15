#include "LineWidth.h"

using namespace DCGp;

LineWidth::LineWidth()
	: m_width(1)
{
}

LineWidth::LineWidth(float width)
	: m_width(width)
{
	
}


LineWidth::~LineWidth()
{
}

void LineWidth::SetWidth(float width)
{
	m_width = width;
}

void LineWidth::Apply(State&) const
{
	glLineWidth(m_width);
}
