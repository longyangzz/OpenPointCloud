#ifndef DC_VIEW_IMPL_BILATERALFILTER_H
#define DC_VIEW_IMPL_BILATERALFILTER_H

#include "DCView/BilateralFilter.h"

//Boost
#include "boost/shared_ptr.hpp"

	namespace DCView
	{
		//FBO
		class FrameBuffer;
		typedef boost::shared_ptr<FrameBuffer> FrameBufferPtr;
		//Shader
		class GLShader;
		typedef boost::shared_ptr<GLShader> GLShaderPtr;

		typedef BilateralFilter::ViewportParameters ViewportParameters;

		//BilateralFilter实现类
		class BilateralFilter::BilateralFilterImpl
		{
		public:
			BilateralFilterImpl(BilateralFilter* pInterface);
			~BilateralFilterImpl();

			void Reset();

			//inherited from ccGlFilter
			GLFilterPtr Clone() const;
			bool Init(int width, int height, const QString& shadersPath, QString& error);
			void Shade(unsigned texDepth, unsigned texColor, ViewportParameters& parameters);
			unsigned Texture() const;

			//! Set parameters
			/** \param halfSpatialSize half spatial kernel size (between 1 and 7 - total size will be 2*h+1)
				\param spatialSigma variance of the 'spatial' distribution (Euclidean distance of pixels)
				\param depthSigma variance of the 'depth' distribution (depth difference of pixels)
			**/
			void SetParams(	unsigned halfSpatialSize,
							float spatialSigma,
							float depthSigma );

			//! Sets whether to use the current context (OpenGL) viewport or not
			void UseExistingViewport(bool state);

			void UpdateDampingTable();

		private:
			//接口
			BilateralFilter* m_pInterface;

			int m_width;
			int	m_height;

			FrameBufferPtr m_pFBO;
			GLShaderPtr m_pShader;

			//! Half spatial size (kernel width will be 2*h+1)
			unsigned m_halfSpatialSize;
			//! Variance of the 'spatial' distribution (Euclidean distance of pixels)
			float m_spatialSigma;
			//! Variance of the 'depth' distribution (depth difference of pixels)
			float m_depthSigma;

			//! 'spatial' distribution (kernel values)
			float* m_dampingPixelDist;

			//! Whether to use the current context (OpenGL) viewport or not
			bool m_useCurrentViewport;
		};
	}

#endif