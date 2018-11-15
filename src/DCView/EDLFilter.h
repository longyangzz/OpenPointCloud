#ifndef DC_VIEW_EDLFILTER_H
#define DC_VIEW_EDLFILTER_H

#include "ViewDLL.h"

//Self
#include "GLFilter.h"

namespace DCView
	{
		class DC_VIEW_API EDLFilter : public GLFilter
		{
		public:
			EDLFilter();
			~EDLFilter();

			virtual GLFilterPtr Clone() const;
			virtual bool Init(int width, int height, const QString& shadersPath, QString& error);
			virtual void Shade(unsigned texDepth, unsigned texColor, ViewportParameters& parameters);
			virtual unsigned Texture() const;

			void Reset();

			bool Init(	int width,
						int height,
						unsigned internalFormat,
						unsigned minMagFilter,
						const QString& shadersPath,
						QString& error);

			unsigned Texture(int index) const;

			void SetLightDirection(float theta_rad, float phi_rad);

			void SetStrength(float value);

		private:
			class EDLFilterImpl;
			EDLFilterImpl* m_pImpl;
		};
		typedef boost::shared_ptr<EDLFilter> EDLFilterPtr;
	}
#endif
