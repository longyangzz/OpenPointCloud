#ifndef DCGP_MATERIAL_H
#define DCGP_MATERIAL_H

#include "DCGp/DcGpDLL.h"

//Qt
#include "QString"
class QImage;

#include "DCCore/DCDefs.h"

namespace DcGp
{
	DECLARE_SMARTPTR(DcGpMaterial)
	DECLARE_SMARTPTR(DcGpMaterialImpl)

	class DCGP_API DcGpMaterial
	{
		DECLARE_IMPLEMENT(DcGpMaterial)

	public:
		DcGpMaterial();
		//构造函数
		DcGpMaterial(QString name);

		//!复制构造
		DcGpMaterial(const DcGpMaterial& mtl);

		//! 设置漫射光
		void SetDiffuse(const float color[4]);

		//! 设置光亮度(both front - 100% - and back - 80%)
		void SetShininess(float val);

		//! 设置透明度 (all colors)
		void SetTransparency(float val);

		//! 应用参数 (OpenGL)
		void ApplyGL(bool lightEnabled, bool skipDiffuse) const;

		QString GetName() const;
		void SetName(QString name);

		//贴图
		QImage GetTexture() const;
		void SetTexture(QImage texture);

		float* GetDiffuseFront() const;
		void SetDiffuseFront(const float diffuse[4]);

		float* GetDiffuseBack() const;
		void SetDiffuseBack(const float diffuse[4]);

		float* GetAmbient() const;
		void SetAmbient(const float ambient[4]);

		float* GetSpecular() const;
		void SetSpecular(const float specular[4]);

		float* GetEmission() const;
		void SetEmission(const float emission[4]);

		float GetsShininessFront() const;
		void SetShininessFront(float shininessFront);

		float GetsShininessBack() const;
		void SetShininessBack(float shininessBack);

		unsigned GetTextureId() const;
		void SetTextureId(unsigned id);
	};
}

#endif // DCMATERIAL_H
