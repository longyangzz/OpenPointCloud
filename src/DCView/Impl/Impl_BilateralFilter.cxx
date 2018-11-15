#include "Impl_BilateralFilter.hxx"
using namespace DCView;

//Slef
#include "DCView/GLToolkit.h"
#include "DCView/GLShader.h"

#include "DCCore/DCGL.h"
#include "FrameBuffer.hxx"

static const unsigned KERNEL_MAX_HALF_SIZE = 7;

BilateralFilter::BilateralFilterImpl
	::BilateralFilterImpl(BilateralFilter* pInterface)
	: m_pInterface(pInterface)
	, m_width(0)
	, m_height(0)
	, m_useCurrentViewport(false)
{
	unsigned maxCoefListSize = (KERNEL_MAX_HALF_SIZE + 1) * (KERNEL_MAX_HALF_SIZE + 1); //must be inferior to 64 to fit the equivalent array size on the shader's side
	m_dampingPixelDist = new float[maxCoefListSize];
	memset(m_dampingPixelDist, 0, maxCoefListSize); //will be updated right away by 'setParameters'

	SetParams(2, 2.0f, 0.4f);
}

BilateralFilter::BilateralFilterImpl
	::~BilateralFilterImpl()
{
	Reset();

	if (m_dampingPixelDist)
		delete[] m_dampingPixelDist;
}

void BilateralFilter::BilateralFilterImpl
	::Reset()
{
	//清空FBO和Shader
	m_pFBO = FrameBufferPtr();
	m_pShader = GLShaderPtr();

	m_width = m_height = 0;
}

//inherited from ccGlFilter
GLFilterPtr BilateralFilter::BilateralFilterImpl
	::Clone() const
{
	BilateralFilterPtr pFilter = BilateralFilterPtr(new BilateralFilter);
	//copy parameters
	pFilter->SetParams(m_halfSpatialSize, m_spatialSigma, m_depthSigma);
	pFilter->m_pImpl->m_useCurrentViewport = m_useCurrentViewport;

	return pFilter;
}
bool BilateralFilter::BilateralFilterImpl
	::Init(int width, int height, const QString& shadersPath, QString& error)
{
	//创建FBO
	if (!m_pFBO)
		m_pFBO = FrameBufferPtr(new FrameBuffer);

	//初始化FBO
	if (!m_pFBO->Initialize(width, height))
	{
		Reset();
		return false;
	}

	m_pFBO->Start();
	m_pFBO->InitTexture(0, GL_RGB/*GL_RGB32F*/, GL_RGB, GL_FLOAT);
	m_pFBO->Stop();

	if (!m_pShader)
		m_pShader = GLShaderPtr(new GLShader);

	if (!m_pShader->FromFile(shadersPath, "bilateral"))
	{
		//ccLog::Warning(QString("[Bilateral Filter] Can't load shader: %1").arg(error));
		Reset();
		return false;
	}

	m_width = width;
	m_height = height;

	return true;
}
void BilateralFilter::BilateralFilterImpl
	::Shade(unsigned texDepth, unsigned texColor, ViewportParameters& parameters)
{
	if (!m_pFBO || !m_pShader)
		return;

	glPushAttrib(GL_ALL_ATTRIB_BITS);

	if (!m_useCurrentViewport)
	{
		//we must use corner-based screen coordinates
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(0.0,(GLdouble)m_width, 0.0, (GLdouble)m_height, 0.0, 1.0);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
	}

	//	HORIZONTAL
	m_pFBO->Start();
	m_pFBO->SetDrawBuffers1();

	m_pShader->Start();
	m_pShader->SetUniform1i("s2_I",0);	// image to blur
	m_pShader->SetUniform1i("s2_D",1);	// image to modulate filter
	m_pShader->SetUniform1f("SX", static_cast<float>(m_width));
	m_pShader->SetUniform1f("SY", static_cast<float>(m_height));
	m_pShader->SetUniform1i("NHalf", m_halfSpatialSize);
	m_pShader->SetTabUniform1fv("DistCoefs", 64, m_dampingPixelDist);
	m_pShader->SetUniform1f("SigmaDepth", m_depthSigma);

	//Texture 1 --> 2D
	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,texDepth);

	//Texture 0 --> 2D
	glActiveTexture(GL_TEXTURE0);
	//glEnable(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D,texColor);

	GLToolkit::RenderTexture2DCorner(texColor, m_width, m_height);

	//Texture 0 --> 2D
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D,0);
	//glDisable(GL_TEXTURE_2D);

	//Texture 1 --> 2D
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D,0);
	glDisable(GL_TEXTURE_2D);

	m_pShader->Stop();
	m_pFBO->Stop();

	if (!m_useCurrentViewport)
	{
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
	}

	glPopAttrib();
}
unsigned BilateralFilter::BilateralFilterImpl
	::Texture() const
{
	return (m_pFBO ? m_pFBO->GetColorTexture(0) : 0);
}

//设置参数
void BilateralFilter::BilateralFilterImpl
	::SetParams(unsigned halfSpatialSize,
				float spatialSigma,
				float depthSigma )
{
	m_halfSpatialSize = std::min<unsigned>(halfSpatialSize, KERNEL_MAX_HALF_SIZE);
	m_spatialSigma = spatialSigma;
	m_depthSigma = depthSigma;

	UpdateDampingTable();
}

//! Sets whether to use the current context (OpenGL) viewport or not
void BilateralFilter::BilateralFilterImpl
	::UseExistingViewport(bool state)
{
	m_useCurrentViewport = state;
}

void BilateralFilter::BilateralFilterImpl
	::UpdateDampingTable()
{
	//constant quotient
	float q = static_cast<float>(m_halfSpatialSize) * m_spatialSigma;
	q = 2.0f * (q*q);

	for (unsigned c = 0; c <= m_halfSpatialSize; c++)
	{
		for (unsigned d = 0; d <= m_halfSpatialSize; d++)
		{
			//pixel distance based damping
			m_dampingPixelDist[c * (m_halfSpatialSize + 1) + d] = exp(-static_cast<float>(c * c + d * d) / q);
		}
	}
}