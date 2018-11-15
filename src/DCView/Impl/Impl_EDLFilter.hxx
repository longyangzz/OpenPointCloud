#ifndef DC_VIEW_IMPL_EDLFILTER_H
#define DC_VIEW_IMPL_EDLFILTER_H

#include "DCView/EDLFilter.h"

//Self
#include "DCView/BilateralFilter.h"

namespace DCView
	{
		//FBO
		class FrameBuffer;
		typedef boost::shared_ptr<FrameBuffer> FrameBufferPtr;
		//Shader
		class GLShader;
		typedef boost::shared_ptr<GLShader> GLShaderPtr;

		typedef BilateralFilter::ViewportParameters ViewportParameters;

		//EDLFilter实现类
		class EDLFilter::EDLFilterImpl
		{
		public:
			EDLFilterImpl(EDLFilter* pInterface);
			~EDLFilterImpl();

			//inherited from ccGlFilter
			GLFilterPtr Clone() const;
			bool Init(int width, int height, const QString& shadersPath, QString& error);
			void Shade(unsigned texDepth, unsigned texColor, ViewportParameters& parameters);
			unsigned Texture() const;

			//! Resets filter
			void Reset();

			//! Inits filter
			bool Init(int width, int height, unsigned internalFormat, unsigned minMagFilter,
					const QString& shadersPath, QString& error);

			//! Returns given texture index
			unsigned Texture(int index) const;

			//! Sets light direction
			void SetLightDirection(float theta_rad, float phi_rad);

			//! Sets strength
			/** \param value strength value (default: 100)
			**/
			void SetStrength(float value);

		private:
			//接口
			EDLFilter* m_pInterface;

			//屏幕大小
			int	m_screenWidth;
			int	m_screenHeight;

			FrameBufferPtr m_pFBO_EDL0;
			FrameBufferPtr m_pFBO_EDL1;
			FrameBufferPtr m_pFBO_EDL2;
			GLShaderPtr	m_pShader_EDL;

			FrameBufferPtr m_pFBO_Mix;
			GLShaderPtr m_pShader_Mix;

			float m_neighbours[8 * 2];
			float m_expScale;

			// Light direction
			float m_lightDirection[3];

			//! Bilateral filter and its parameters
			struct BilateralFilter
			{
				BilateralFilterPtr pFilter;
				unsigned halfSize;
				float sigma;
				float sigmaZ;
				bool enabled;

				BilateralFilter()
					: halfSize(0)
					, sigma(0.0f)
					, sigmaZ(0.0f)
					, enabled(false)
				{
				}
			};

			//	Bilateral filtering
			BilateralFilter m_bilateralFilter0;
			BilateralFilter m_bilateralFilter1;
			BilateralFilter m_bilateralFilter2;
		};
	}

#endif