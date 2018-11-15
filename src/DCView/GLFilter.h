#ifndef DC_VIEW_GLFILTER_H
#define DC_VIEW_GLFILTER_H

#include "ViewDLL.h"

//boost
#include "boost/shared_ptr.hpp"

//Qt
#include "QtCore/QString"

namespace DCView
	{
		class GLFilter;
		typedef boost::shared_ptr<GLFilter> GLFilterPtr;

		class DC_VIEW_API GLFilter
		{
		public:
			struct ViewportParameters
			{
				//! Default constructor
				ViewportParameters()
					: zoom(1.0)
					, perspectiveMode(false)
					, zNear(0.0)
					, zFar(1.0)
				{}

				double zoom;
				//是否透视模式
				bool perspectiveMode;
				//近平面仅透视模式
				double zNear;
				//远平面仅透视模式
				double zFar;
			};
		public:
			//析构函数
			virtual ~GLFilter() {}

			//克隆机制
			virtual GLFilterPtr Clone() const = 0;

			//初始化GLFilter
			virtual bool Init(int width, int height, const QString& shadersPath, QString& error) = 0;

			//将filter应用到纹理中
			virtual void Shade(unsigned texDepth, unsigned texColor, ViewportParameters& parameters) = 0;

			//纹理ID
			virtual unsigned Texture() const = 0;

			//filter名称
			inline QString Name() const { return m_name; }

		protected:
			//构造函数
			GLFilter(const QString& strName)
				: m_name(strName){}

		private:
			//名称
			QString m_name;
		};
	}

#endif
