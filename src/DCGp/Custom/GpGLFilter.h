#ifndef DCGP_GL_FILTER_H
#define DCGP_GL_FILTER_H

#include "DCCore/DCGL.h"

namespace DcGp
{

class DCGP_API DcGpGLFilter
{
public:
		//
	DcGpGLFilter(QString description)
		: m_description(description)
	{}
		//! Default destructor
	virtual ~DcGpGLFilter() {}

	//! Cloning mechanism
	virtual DcGpGLFilter* Clone() const = 0;

	//! Initializes GL filter
	/** Must support reinit!
		\param width texture/screen width
		\param height texture/screen height
		\param shadersPath path where shader files are stored
		\param error error string (if an error occurred)
		\return success
		**/
	virtual bool Init(	int width,
						int height,
						QString shadersPath,
						QString& error) = 0;

	//! Minimal set of 3D viewport parameters that can be used by shaders
	struct ViewportParameters
	{
		//! Default constructor
		ViewportParameters()
			: zoom(1.0)
			, perspectiveMode(false)
			, zNear(0.0)
			, zFar(1.0)
		{}

		//! Zoom
		double zoom;
		//! Whether perspective mode is enabled or not
		bool perspectiveMode;
		//! Near clipping plane position (perspective mode only)
		double zNear;
		//! Far clipping plane position (perspective mode only)
		double zFar;
	};

	//! Applies filter to texture (depth + color)
	virtual void Shade(	GLuint texDepth,
						GLuint texColor,
						ViewportParameters& parameters) = 0;

	//! Returns resulting texture
	virtual GLuint GetTexture() = 0;

	//! Returns filter name
	inline virtual QString GetDescription() const { return m_description; }

protected:

	//! Filter description
	QString m_description;
	};

}
#endif
