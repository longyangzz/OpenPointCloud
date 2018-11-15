#ifndef DC_VIEW_BILATERALFILTER_H
#define DC_VIEW_BILATERALFILTER_H

#include "ViewDLL.h"

//Self
#include "GLFilter.h"

namespace DCView
	{
		class DC_VIEW_API BilateralFilter : public GLFilter
		{
		public:
			BilateralFilter();
			~BilateralFilter();

			void Reset();

			virtual GLFilterPtr Clone() const;
			virtual bool Init(int width, int height, const QString& shadersPath, QString& error);
			virtual void Shade(unsigned texDepth, unsigned texColor, ViewportParameters& parameters);
			virtual unsigned Texture() const;

			void SetParams(	unsigned halfSpatialSize,
							float spatialSigma,
							float depthSigma );

			void UseExistingViewport(bool state);

		protected:

			void UpdateDampingTable();

		private:
			class BilateralFilterImpl;
			BilateralFilterImpl* m_pImpl;
		};
		typedef boost::shared_ptr<BilateralFilter> BilateralFilterPtr;
	}

#endif // BILATERALFILTER_H
