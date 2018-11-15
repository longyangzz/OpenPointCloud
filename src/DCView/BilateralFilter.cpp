#include "DCView/BilateralFilter.h"
using namespace DCView;

//Impl
#include "Impl/Impl_BilateralFilter.hxx"

BilateralFilter::BilateralFilter()
	: GLFilter("Bilateral Smooth")
{
	m_pImpl = new BilateralFilterImpl(this);
}

BilateralFilter::~BilateralFilter()
{
	delete m_pImpl;
}

void BilateralFilter::UseExistingViewport(bool state)
{
	m_pImpl->UseExistingViewport(state);
}

void BilateralFilter::Reset()
{
	m_pImpl->Reset();
}

bool BilateralFilter::Init(int width, int height, const QString& shadersPath, QString& error)
{
	return m_pImpl->Init(width, height, shadersPath, error);
}

GLFilterPtr BilateralFilter::Clone() const
{
	return m_pImpl->Clone();
}

void BilateralFilter::SetParams(unsigned halfSpatialSize, float spatialSigma, float depthSigma)
{
	m_pImpl->SetParams(halfSpatialSize, spatialSigma, depthSigma);
}

void BilateralFilter::Shade(unsigned texDepth, unsigned texColor, ViewportParameters& parameters)
{
	m_pImpl->Shade(texDepth, texColor, parameters);
}

unsigned BilateralFilter::Texture() const
{
	return m_pImpl->Texture();
}

void BilateralFilter::UpdateDampingTable()
{
	m_pImpl->UpdateDampingTable();
}

