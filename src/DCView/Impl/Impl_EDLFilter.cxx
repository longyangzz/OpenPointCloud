#include "Impl_EDLFilter.hxx"
using namespace DCView;

//Self
#include "DCCore/DCGL.h"
#include "FrameBuffer.hxx"

#include "DCView/GLToolkit.h"
#include "DCView/GLShader.h"

#ifndef M_PI
#define M_PI 3.141592653589793238462643
#endif

EDLFilter::EDLFilterImpl
	::EDLFilterImpl(EDLFilter* pInterface)
	: m_pInterface(pInterface)
	, m_screenWidth(0)
	, m_screenHeight(0)
	, m_expScale(100.0f)
{
	//smoothing filter for full resolution
	m_bilateralFilter0.enabled	= false;
	m_bilateralFilter0.halfSize	= 1;
	m_bilateralFilter0.sigma	= 1.0f;
	m_bilateralFilter0.sigmaZ	= 0.2f;

	//smoothing filter for half resolution
	m_bilateralFilter1.enabled	= true;
	m_bilateralFilter1.halfSize	= 2;
	m_bilateralFilter1.sigma	= 2.0f;
	m_bilateralFilter1.sigmaZ	= 0.4f;

	//smoothing filter for quarter resolution
	m_bilateralFilter2.enabled	= true;
	m_bilateralFilter2.halfSize	= 2;
	m_bilateralFilter2.sigma	= 2.0f;
	m_bilateralFilter2.sigmaZ	= 0.4f;

	//设置光线方向
	SetLightDirection(static_cast<float>(M_PI*0.5),static_cast<float>(M_PI*0.5));

	memset(m_neighbours, 0, sizeof(float)*8*2);
	for (int c = 0; c<8; c++)
	{
		m_neighbours[2*c]		= static_cast<float>(cos(static_cast<double>(c)*M_PI/4.0));
		m_neighbours[2*c+1]	= static_cast<float>(sin(static_cast<double>(c)*M_PI/4.0));
	}
}

//析构
EDLFilter::EDLFilterImpl
	::~EDLFilterImpl()
{
	Reset();
}

//克隆
GLFilterPtr EDLFilter::EDLFilterImpl
	::Clone() const
{
	EDLFilterPtr pFilter = EDLFilterPtr(new EDLFilter);

	//copy parameters (only those that can be changed by the user!)
	pFilter->SetStrength(m_expScale);
	pFilter->m_pImpl->m_lightDirection[0] = m_lightDirection[0];
	pFilter->m_pImpl->m_lightDirection[1] = m_lightDirection[1];
	pFilter->m_pImpl->m_lightDirection[2] = m_lightDirection[2];

	return pFilter;
}

//初始化
bool EDLFilter::EDLFilterImpl
	::Init(int width, int height, const QString& shadersPath, QString& error)
{
	return Init(width, height, GL_RGBA, GL_LINEAR, shadersPath, error);
}

void EDLFilter::EDLFilterImpl
	::Shade(unsigned texDepth, unsigned texColor, ViewportParameters& parameters)
{
	if (!m_pFBO_EDL0)
	{
		//ccLog::Warning("[EDLFilter::shade] Internal error: structures not initialized!");
		return;
	}

	if (m_screenWidth < 4 || m_screenHeight < 4)
	{
		//ccLog::Warning("[EDLFilter::shade] Screen is too small!");
		return;
	}

	//perspective mode
	int perspectiveMode = parameters.perspectiveMode ? 1 : 0;
	//light-balancing based on the current zoom (for ortho. mode only)
	float lightMod = perspectiveMode ? 3.0f : static_cast<float>(sqrt(2.0*std::max<double>(parameters.zoom,0.7))); //1.41 ~ sqrt(2)

	glPushAttrib(GL_ALL_ATTRIB_BITS);

	//we must use corner-based screen coordinates
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.0,(GLdouble)m_screenWidth,0.0,(GLdouble)m_screenHeight,0.0,1.0/*parameters.zNear,parameters.zFar*/);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	/***	FULL SIZE	***/
	{
		m_pFBO_EDL0->Start();
		m_pFBO_EDL0->SetDrawBuffers1();

		m_pShader_EDL->Start();
		m_pShader_EDL->SetUniform1i("s1_color",1);
		m_pShader_EDL->SetUniform1i("s2_depth",0);

		m_pShader_EDL->SetUniform1f("Sx", static_cast<float>(m_screenWidth));
		m_pShader_EDL->SetUniform1f("Sy", static_cast<float>(m_screenHeight));
		m_pShader_EDL->SetUniform1f("Zoom", lightMod);
		m_pShader_EDL->SetUniform1i("PerspectiveMode", perspectiveMode);
		m_pShader_EDL->SetUniform1f("Pix_scale", 1.0f);
		m_pShader_EDL->SetUniform1f("Exp_scale", m_expScale);
		m_pShader_EDL->SetUniform1f("Zm",static_cast<float>(parameters.zNear));
		m_pShader_EDL->SetUniform1f("ZM",static_cast<float>(parameters.zFar));
		m_pShader_EDL->SetUniform3fv("Light_dir", m_lightDirection);
		m_pShader_EDL->SetTabUniform2fv("Neigh_pos_2D", 8, m_neighbours);

		glActiveTexture(GL_TEXTURE1);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texColor);

		glActiveTexture(GL_TEXTURE0);
		GLToolkit::RenderTexture2DCorner(texDepth, m_screenWidth, m_screenHeight);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_TEXTURE_2D);

		m_pShader_EDL->Stop();
		m_pFBO_EDL0->Stop();
	}
	/***	FULL SIZE	***/

	/***	HALF SIZE	***/
	{
		m_pFBO_EDL1->Start();
		m_pFBO_EDL1->SetDrawBuffers1();

		m_pShader_EDL->Start();
		m_pShader_EDL->SetUniform1i("s1_color",1);
		m_pShader_EDL->SetUniform1i("s2_depth",0);

		m_pShader_EDL->SetUniform1f("Sx",static_cast<float>(m_screenWidth>>1));
		m_pShader_EDL->SetUniform1f("Sy",static_cast<float>(m_screenHeight>>1));
		m_pShader_EDL->SetUniform1f("Zoom",lightMod);
		m_pShader_EDL->SetUniform1i("PerspectiveMode",perspectiveMode);
		m_pShader_EDL->SetUniform1f("Pix_scale",2.0f);
		m_pShader_EDL->SetUniform1f("Exp_scale",m_expScale);
		m_pShader_EDL->SetUniform1f("Zm",static_cast<float>(parameters.zNear));
		m_pShader_EDL->SetUniform1f("ZM",static_cast<float>(parameters.zFar));
		m_pShader_EDL->SetUniform3fv("Light_dir", m_lightDirection);
		m_pShader_EDL->SetTabUniform2fv("Neigh_pos_2D",8, m_neighbours);
		//*/

		glActiveTexture(GL_TEXTURE1);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texColor);

		glActiveTexture(GL_TEXTURE0);
		GLToolkit::RenderTexture2DCorner(texDepth,m_screenWidth/2,m_screenHeight/2);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_TEXTURE_2D);

		m_pShader_EDL->Stop();
		m_pFBO_EDL1->Stop();
	}
	/***	HALF SIZE	***/

	/***	QUARTER SIZE	***/
	{
		m_pFBO_EDL2->Start();
		m_pFBO_EDL2->SetDrawBuffers1();

		m_pShader_EDL->Start();
		m_pShader_EDL->SetUniform1i("s1_color",1);
		m_pShader_EDL->SetUniform1i("s2_depth",0);

		m_pShader_EDL->SetUniform1f("Sx",static_cast<float>(m_screenWidth>>2));
		m_pShader_EDL->SetUniform1f("Sy",static_cast<float>(m_screenHeight>>2));
		m_pShader_EDL->SetUniform1f("Zoom",lightMod);
		m_pShader_EDL->SetUniform1i("PerspectiveMode",perspectiveMode);
		m_pShader_EDL->SetUniform1f("Pix_scale",4.0f);
		m_pShader_EDL->SetUniform1f("Exp_scale",m_expScale);
		m_pShader_EDL->SetUniform1f("Zm",static_cast<float>(parameters.zNear));
		m_pShader_EDL->SetUniform1f("ZM",static_cast<float>(parameters.zFar));
		m_pShader_EDL->SetUniform3fv("Light_dir",m_lightDirection);
		m_pShader_EDL->SetTabUniform2fv("Neigh_pos_2D",8,m_neighbours);
		//*/

		glActiveTexture(GL_TEXTURE1);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texColor);

		glActiveTexture(GL_TEXTURE0);
		GLToolkit::RenderTexture2DCorner(texDepth,m_screenWidth/4,m_screenHeight/4);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_TEXTURE_2D);

		m_pShader_EDL->Stop();
		m_pFBO_EDL2->Stop();
	}
	/***	QUARTER SIZE	***/

	/***	SMOOTH RESULTS	***/
	{
		if (m_bilateralFilter0.pFilter)
		{
			m_bilateralFilter0.pFilter->SetParams(m_bilateralFilter0.halfSize,m_bilateralFilter0.sigma,m_bilateralFilter0.sigmaZ);
			m_bilateralFilter0.pFilter->Shade(texDepth,m_pFBO_EDL0->GetColorTexture(0),parameters);
		}
		if (m_bilateralFilter1.pFilter)
		{
			m_bilateralFilter1.pFilter->SetParams(m_bilateralFilter1.halfSize,m_bilateralFilter1.sigma,m_bilateralFilter1.sigmaZ);
			m_bilateralFilter1.pFilter->Shade(texDepth,m_pFBO_EDL1->GetColorTexture(0),parameters);
		}
		if (m_bilateralFilter2.pFilter)
		{
			m_bilateralFilter2.pFilter->SetParams(m_bilateralFilter2.halfSize,m_bilateralFilter2.sigma,m_bilateralFilter2.sigmaZ);
			m_bilateralFilter2.pFilter->Shade(texDepth,m_pFBO_EDL2->GetColorTexture(0),parameters);
		}
	}
	/***	SMOOTH RESULTS	***/

	//***	COMPOSITING		***/
	{
		m_pFBO_Mix->Start();
		m_pFBO_Mix->SetDrawBuffers1();

		m_pShader_Mix->Start();
		m_pShader_Mix->SetUniform1i("s2_I1",0);
		m_pShader_Mix->SetUniform1i("s2_I2",1);
		m_pShader_Mix->SetUniform1i("s2_I4",2);
		m_pShader_Mix->SetUniform1i("s2_D",3);
		m_pShader_Mix->SetUniform1f("A0",1.0f);
		m_pShader_Mix->SetUniform1f("A1",0.5f);
		m_pShader_Mix->SetUniform1f("A2",0.25f);
		m_pShader_Mix->SetUniform1i("absorb",1);

		glActiveTexture(GL_TEXTURE3);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,texDepth);

		glActiveTexture(GL_TEXTURE2);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, m_bilateralFilter2.pFilter 
			? m_bilateralFilter2.pFilter->Texture() : m_pFBO_EDL2->GetColorTexture(0));

		glActiveTexture(GL_TEXTURE1);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, m_bilateralFilter1.pFilter 
			? m_bilateralFilter1.pFilter->Texture() : m_pFBO_EDL1->GetColorTexture(0));

		glActiveTexture(GL_TEXTURE0);
		//glEnable(GL_TEXTURE_2D);

		GLToolkit::RenderTexture2DCorner(m_bilateralFilter0.pFilter 
			? m_bilateralFilter0.pFilter->Texture() : m_pFBO_EDL0->GetColorTexture(0),m_screenWidth,m_screenHeight);

		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D,0);
		//glDisable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D,0);
		glDisable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D,0);
		glDisable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D,0);
		glDisable(GL_TEXTURE_2D);

		m_pShader_Mix->Stop();
		m_pFBO_Mix->Stop();
	}

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glPopAttrib();
}

unsigned EDLFilter::EDLFilterImpl
	::Texture() const
{
	return Texture(0);
}

//! Resets filter
void EDLFilter::EDLFilterImpl
	::Reset()
{
	m_pFBO_EDL0 = FrameBufferPtr();
	m_pFBO_EDL1 = FrameBufferPtr();
	m_pFBO_EDL2 = FrameBufferPtr();
	m_pShader_EDL = GLShaderPtr();

	m_pFBO_Mix = FrameBufferPtr();
	m_pShader_Mix = GLShaderPtr();

	m_bilateralFilter0.pFilter = BilateralFilterPtr();
	m_bilateralFilter1.pFilter = BilateralFilterPtr();
	m_bilateralFilter2.pFilter = BilateralFilterPtr();

	m_screenWidth = m_screenHeight = 0;
}

//! Inits filter
bool EDLFilter::EDLFilterImpl
	::Init(int width, int height, unsigned internalFormat, unsigned minMagFilter,
		const QString& shadersPath, QString& error)
{
	if (!m_pFBO_EDL0)
	{
		m_pFBO_EDL0 = FrameBufferPtr(new FrameBuffer);
	}
	if (!m_pFBO_EDL0->Initialize(width, height))
	{
		error = "[EDL Filter] FBO 1:1 initialization failed!";
		Reset();
		return false;
	}

	if (!m_pFBO_EDL1)
	{
		m_pFBO_EDL1 = FrameBufferPtr(new FrameBuffer);
	}
	if (!m_pFBO_EDL1->Initialize(width / 2, height / 2))
	{
		error = "[EDL Filter] FBO 1:2 initialization failed!";
		Reset();
		return false;
	}

	if (!m_pFBO_EDL2)
	{
		m_pFBO_EDL2 = FrameBufferPtr(new FrameBuffer);
	}
	if (!m_pFBO_EDL2->Initialize(width / 4, height / 4))
	{
		error = "[EDL Filter] FBO 1:4 initialization failed!";
		Reset();
		return false;
	}

	m_pFBO_EDL0->InitTexture(0,internalFormat,GL_RGBA,GL_FLOAT,minMagFilter);
	m_pFBO_EDL1->InitTexture(0,internalFormat,GL_RGBA,GL_FLOAT,minMagFilter);
	m_pFBO_EDL2->InitTexture(0,internalFormat,GL_RGBA,GL_FLOAT,minMagFilter);

	if (!m_pFBO_Mix)
	{
		m_pFBO_Mix = FrameBufferPtr(new FrameBuffer);
	}
	if (!m_pFBO_Mix->Initialize(width, height))
	{
		error = "[EDL Filter] FBO 'mix' initialization failed!";
		Reset();
		return false;
	}
	m_pFBO_Mix->InitTexture(0,internalFormat,GL_RGBA,GL_FLOAT);

	if (!m_pShader_EDL)
	{
		m_pShader_EDL = GLShaderPtr(new GLShader);
		if (!m_pShader_EDL->FromFile(shadersPath,"EDL/edl_shade"))
		{
			Reset();
			return false;
		}
	}

	if (!m_pShader_Mix)
	{
		m_pShader_Mix = GLShaderPtr(new GLShader);
		if (!m_pShader_Mix->FromFile(shadersPath,"EDL/edl_mix"))
		{
			Reset();
			return false;
		}
	}

	if (m_bilateralFilter0.enabled)
	{
		if (!m_bilateralFilter0.pFilter)
		{
			m_bilateralFilter0.pFilter = BilateralFilterPtr(new DCView::BilateralFilter);
		}
		if (!m_bilateralFilter0.pFilter->Init(width,height,shadersPath,error))
		{
			m_bilateralFilter0.pFilter = BilateralFilterPtr();
			m_bilateralFilter0.enabled = false;
		}
		else
		{
			m_bilateralFilter0.pFilter->UseExistingViewport(true);
		}
	}
	else if (m_bilateralFilter0.pFilter)
	{
		m_bilateralFilter0.pFilter = BilateralFilterPtr();
	}

	if (m_bilateralFilter1.enabled)
	{
		if (!m_bilateralFilter1.pFilter)
		{
			m_bilateralFilter1.pFilter = BilateralFilterPtr(new DCView::BilateralFilter);
		}
		if (!m_bilateralFilter1.pFilter->Init(width/2,height/2,shadersPath,error))
		{
			m_bilateralFilter1.pFilter = BilateralFilterPtr();
			m_bilateralFilter1.enabled = false;
		}
		else
		{
			m_bilateralFilter1.pFilter->UseExistingViewport(true);
		}
	}
	else if (m_bilateralFilter1.pFilter)
	{
		m_bilateralFilter1.pFilter = BilateralFilterPtr();
	}

	if (m_bilateralFilter2.enabled)
	{
		if (!m_bilateralFilter2.pFilter)
		{
			m_bilateralFilter2.pFilter = BilateralFilterPtr(new DCView::BilateralFilter);
		}
		if (!m_bilateralFilter2.pFilter->Init(width/4,height/4,shadersPath,error))
		{
			m_bilateralFilter2.pFilter = BilateralFilterPtr();
			m_bilateralFilter2.enabled = false;
		}
		else
		{
			m_bilateralFilter2.pFilter->UseExistingViewport(true);
		}
	}
	else if (m_bilateralFilter2.pFilter)
	{
		m_bilateralFilter2.pFilter = BilateralFilterPtr();
	}

	m_screenWidth = width;
	m_screenHeight = height;

	return true;
}

//! Returns given texture index
unsigned EDLFilter::EDLFilterImpl
	::Texture(int index) const
{
	switch (index)
	{
	case 0:
		return (m_pFBO_Mix ? m_pFBO_Mix->GetColorTexture(0) : 0);
	case 1:
		return (m_pFBO_EDL0 ? m_pFBO_EDL0->GetColorTexture(0) : 0);
	case 2:
		return (m_pFBO_EDL1 ? m_pFBO_EDL1->GetColorTexture(0) : 0);
	case 3:
		return (m_pFBO_EDL2 ? m_pFBO_EDL2->GetColorTexture(0) : 0);
	case 4:
		return (m_bilateralFilter0.pFilter ? m_bilateralFilter0.pFilter->Texture() : 0);
	case 5:
		return (m_bilateralFilter1.pFilter ? m_bilateralFilter1.pFilter->Texture() : 0);
	case 6:
		return (m_bilateralFilter2.pFilter ? m_bilateralFilter2.pFilter->Texture() : 0);
	}

	//ccLog::Warning("[EDLFilter::getTexture] Internal error: bad argument");
	return 0;
}

//! Sets light direction
void EDLFilter::EDLFilterImpl
	::SetLightDirection(float theta_rad, float phi_rad)
{
	m_lightDirection[0]	= sin(phi_rad) * cos(theta_rad);
	m_lightDirection[1]	= cos(phi_rad);
	m_lightDirection[2]	= sin(phi_rad) * sin(theta_rad);
}

//! Sets strength
/** \param value strength value (default: 100)
**/
void EDLFilter::EDLFilterImpl
	::SetStrength(float value)
{
	m_expScale = value;
}