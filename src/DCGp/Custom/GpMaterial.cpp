#include "DCGp/Custom/GpMaterial.h"

//Qt
#include "QString"
#include "QtGui/QImage"

namespace DcGp
{
	DECLARE_SMARTPTR(DcGpMaterialImpl)

	class DcGpMaterialImpl
	{
		DECLARE_PHONY_CONSTRUCTOR(DcGpMaterialImpl)

	protected:
		DcGpMaterialImpl()
			: m_name("")
			, m_textureID(0)
		{}

	private:
		friend class DcGpMaterial;

		QString m_name;
		QImage m_texture;
		float m_diffuseFront[4];
		float m_diffuseBack[4];
		float m_ambient[4];
		float m_specular[4];
		float m_emission[4];
		float m_shininessFront;
		float m_shininessBack;

		unsigned m_textureID;
	};
}
//c++
#include <string.h>

#include "DCCore/DCGL.h"
#include "DCGp/GpBasicTypes.h"

DcGp::DcGpMaterial::DcGpMaterial()
{
	m_pDcGpMaterialImpl = DcGpMaterialImpl::CreateObject();

	m_pDcGpMaterialImpl->m_name = "";

	memcpy(m_pDcGpMaterialImpl->m_diffuseFront, Color::BRIGHT, sizeof(float)*4);
	memcpy(m_pDcGpMaterialImpl->m_diffuseBack, Color::BRIGHT, sizeof(float)*4);
	memcpy(m_pDcGpMaterialImpl->m_ambient, Color::NIGHT, sizeof(float)*4);
	memcpy(m_pDcGpMaterialImpl->m_specular, Color::NIGHT, sizeof(float)*4);
	memcpy(m_pDcGpMaterialImpl->m_emission, Color::NIGHT, sizeof(float)*4);
	SetShininess(50.0);
};

DcGp::DcGpMaterial::DcGpMaterial(QString name)
{
	m_pDcGpMaterialImpl = DcGpMaterialImpl::CreateObject();

	m_pDcGpMaterialImpl->m_name = name;

	memcpy(m_pDcGpMaterialImpl->m_diffuseFront, Color::BRIGHT, sizeof(float)*4);
	memcpy(m_pDcGpMaterialImpl->m_diffuseBack, Color::BRIGHT, sizeof(float)*4);
	memcpy(m_pDcGpMaterialImpl->m_ambient, Color::NIGHT, sizeof(float)*4);
	memcpy(m_pDcGpMaterialImpl->m_specular, Color::NIGHT, sizeof(float)*4);
	memcpy(m_pDcGpMaterialImpl->m_emission, Color::NIGHT, sizeof(float)*4);
	SetShininess(50.0);
};

DcGp::DcGpMaterial::DcGpMaterial(const DcGpMaterial& mtl)
{
	m_pDcGpMaterialImpl = DcGpMaterialImpl::CreateObject();

	m_pDcGpMaterialImpl->m_name = mtl.m_pDcGpMaterialImpl->m_name;
	m_pDcGpMaterialImpl->m_texture = mtl.m_pDcGpMaterialImpl->m_texture;
	m_pDcGpMaterialImpl->m_shininessFront = mtl.m_pDcGpMaterialImpl->m_shininessFront;
	m_pDcGpMaterialImpl->m_shininessBack = mtl.m_pDcGpMaterialImpl->m_shininessBack;
	m_pDcGpMaterialImpl->m_textureID = mtl.m_pDcGpMaterialImpl->m_textureID;

	memcpy(m_pDcGpMaterialImpl->m_diffuseFront, mtl.m_pDcGpMaterialImpl->m_diffuseFront, sizeof(float)*4);
	memcpy(m_pDcGpMaterialImpl->m_diffuseBack, mtl.m_pDcGpMaterialImpl->m_diffuseBack, sizeof(float)*4);
	memcpy(m_pDcGpMaterialImpl->m_ambient, mtl.m_pDcGpMaterialImpl->m_ambient, sizeof(float)*4);
	memcpy(m_pDcGpMaterialImpl->m_specular, mtl.m_pDcGpMaterialImpl->m_specular, sizeof(float)*4);
	memcpy(m_pDcGpMaterialImpl->m_emission, mtl.m_pDcGpMaterialImpl->m_emission, sizeof(float)*4);
}

void DcGp::DcGpMaterial::SetDiffuse(const float color[4])
{
	memcpy(m_pDcGpMaterialImpl->m_diffuseFront, color, sizeof(float)*4);
	memcpy(m_pDcGpMaterialImpl->m_diffuseBack, color, sizeof(float)*4);
}

void DcGp::DcGpMaterial::SetShininess(float val)
{
	m_pDcGpMaterialImpl->m_shininessFront = val;
	m_pDcGpMaterialImpl->m_shininessBack = 0.8f * val;
}

void DcGp::DcGpMaterial::SetTransparency(float val)
{
	m_pDcGpMaterialImpl->m_diffuseFront[3] = val;
	m_pDcGpMaterialImpl->m_diffuseBack[3] = val;
	m_pDcGpMaterialImpl->m_ambient[3] = val;
	m_pDcGpMaterialImpl->m_specular[3] = val;
	m_pDcGpMaterialImpl->m_emission[3] = val;
}

void DcGp::DcGpMaterial::ApplyGL(bool lightEnabled, bool skipDiffuse) const
{
	if (lightEnabled)
	{
		if (!skipDiffuse)
		{
			glMaterialfv(GL_FRONT, GL_DIFFUSE, m_pDcGpMaterialImpl->m_diffuseFront);
			glMaterialfv(GL_BACK, GL_DIFFUSE, m_pDcGpMaterialImpl->m_diffuseBack);
		}
		glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT, m_pDcGpMaterialImpl->m_ambient);
		glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR, m_pDcGpMaterialImpl->m_specular);
		glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION, m_pDcGpMaterialImpl->m_emission);
		glMaterialf(GL_FRONT, GL_SHININESS, m_pDcGpMaterialImpl->m_shininessFront);
		glMaterialf(GL_BACK, GL_SHININESS, m_pDcGpMaterialImpl->m_shininessBack);
	}
	else
	{
		glColor4fv(m_pDcGpMaterialImpl->m_diffuseFront);
	}
}

QString DcGp::DcGpMaterial::GetName() const
{
	return m_pDcGpMaterialImpl->m_name;
}

void DcGp::DcGpMaterial::SetName(QString name)
{
	m_pDcGpMaterialImpl->m_name = name;
}


//ÌùÍ¼
QImage DcGp::DcGpMaterial::GetTexture() const
{
	return m_pDcGpMaterialImpl->m_texture;
}

void DcGp::DcGpMaterial::SetTexture(QImage texture)
{
	m_pDcGpMaterialImpl->m_texture = texture;
}


float* DcGp::DcGpMaterial::GetDiffuseFront() const
{
	return m_pDcGpMaterialImpl->m_diffuseFront;
}

void DcGp::DcGpMaterial::SetDiffuseFront(const float diffuse[4])
{
	memcpy(m_pDcGpMaterialImpl->m_diffuseFront, diffuse, 4 * sizeof(float));
}


float* DcGp::DcGpMaterial::GetDiffuseBack() const
{
	return m_pDcGpMaterialImpl->m_diffuseBack;
}

void DcGp::DcGpMaterial::SetDiffuseBack(const float diffuse[4])
{
	memcpy(m_pDcGpMaterialImpl->m_diffuseBack, diffuse, 4 * sizeof(float));
}


float* DcGp::DcGpMaterial::GetAmbient() const
{
	return m_pDcGpMaterialImpl->m_ambient;
}

void DcGp::DcGpMaterial::SetAmbient(const float ambient[4])
{
	memcpy(m_pDcGpMaterialImpl->m_ambient, ambient, 4 * sizeof(float));
}


float* DcGp::DcGpMaterial::GetSpecular() const
{
	return m_pDcGpMaterialImpl->m_specular;
}

void DcGp::DcGpMaterial::SetSpecular(const float specular[4])
{
	memcpy(m_pDcGpMaterialImpl->m_specular, specular, 4 * sizeof(float));
}


float* DcGp::DcGpMaterial::GetEmission() const
{
	return m_pDcGpMaterialImpl->m_emission;
}

void DcGp::DcGpMaterial::SetEmission(const float emission[4])
{
	memcpy(m_pDcGpMaterialImpl->m_emission, emission, 4 * sizeof(float));
}


float DcGp::DcGpMaterial::GetsShininessFront() const
{
	return m_pDcGpMaterialImpl->m_shininessFront;
}

void DcGp::DcGpMaterial::SetShininessFront(float shininessFront)
{
	m_pDcGpMaterialImpl->m_shininessFront = shininessFront;
}


float DcGp::DcGpMaterial::GetsShininessBack() const
{
	return m_pDcGpMaterialImpl->m_shininessBack;
}

void DcGp::DcGpMaterial::SetShininessBack(float shininessBack)
{
	m_pDcGpMaterialImpl->m_shininessBack = shininessBack;
}


unsigned DcGp::DcGpMaterial::GetTextureId() const
{
	return m_pDcGpMaterialImpl->m_textureID;
}

void DcGp::DcGpMaterial::SetTextureId(unsigned id)
{
	m_pDcGpMaterialImpl->m_textureID = id;
}
