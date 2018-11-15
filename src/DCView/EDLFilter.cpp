#include "DCView/EDLFilter.h"
using namespace DCView;

//Impl
#include "Impl/Impl_EDLFilter.hxx"

EDLFilter::EDLFilter()
	: GLFilter("EyeDome Lighting")
{
	m_pImpl = new EDLFilterImpl(this);
}

EDLFilter::~EDLFilter()
{
	delete m_pImpl;
}

GLFilterPtr EDLFilter::Clone() const
{
	return m_pImpl->Clone();
}

void EDLFilter::Reset()
{
	m_pImpl->Reset();
}

bool EDLFilter::Init(int width, int height, const QString& shadersPath, QString& error)
{
	return m_pImpl->Init(width, height, shadersPath, error);
}

bool EDLFilter::Init(int width, int height, unsigned internalFormat, unsigned minMagFilter, 
	const QString& shadersPath, QString& error)
{
	return m_pImpl->Init(width, height, internalFormat, minMagFilter, shadersPath, error);
}

void EDLFilter::Shade(unsigned texDepth, unsigned texColor, ViewportParameters& parameters)
{
	m_pImpl->Shade(texDepth, texColor, parameters);
}

unsigned EDLFilter::Texture() const
{
	return m_pImpl->Texture();
}

unsigned EDLFilter::Texture(int index) const
{
	return m_pImpl->Texture(index);
}

void EDLFilter::SetLightDirection(float theta_rad, float phi_rad)
{
	m_pImpl->SetLightDirection(theta_rad, phi_rad);
}


void EDLFilter::SetStrength(float value)
{
	m_pImpl->SetStrength(value);
}