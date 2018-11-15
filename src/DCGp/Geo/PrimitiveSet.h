//----------------------------------------------------------------------
//author:wly
//time:
//dsc:封装opengl图元类
//
//----------------------------------------------------------------------

#ifndef MPGP_PRIMITIVESET_H
#define MPGP_PRIMITIVESET_H

#include "DCCore/DCGL.h"

#include "DCGp/DcGpDLL.h"

namespace DCGp
{
	class DCGP_API PrimitiveSet
	{
	public:
		enum Type
		{
			PrimitiveType,
			DrawArraysPrimitiveType,
			DrawElementsUIntPrimitiveType
		};

		enum Mode
		{
			POINTS = GL_POINTS,
			LINES = GL_LINES,
			LINE_STRIP = GL_LINE_STRIP,
			LINE_LOOP = GL_LINE_LOOP,
			TRIANGLES = GL_TRIANGLES,
			TRIANGLE_STRIP = GL_TRIANGLE_STRIP,
			TRIANGLE_FAN = GL_TRIANGLE_FAN,
			QUADS = GL_QUADS,
			QUAD_STRIP = GL_QUAD_STRIP,
			POLYGON = GL_POLYGON
		};

	public:
		PrimitiveSet(Type primType=PrimitiveType,GLenum mode=0, int numInstances=0)
			: m_primitiveType(PrimitiveType)
			, m_numInstances(numInstances)
			, m_mode(mode)
		{

		}
		virtual ~PrimitiveSet();


		//----------------------------Set/Get--------------------------------------
		Type                    GetType() const { return m_primitiveType; }
		virtual const GLvoid*   GetDataPointer() const { return 0; }
		virtual unsigned int    GetTotalDataSize() const { return 0; }
		virtual bool            SupportsBufferObject() const { return false; }

		//virtual DrawElements* getDrawElements() { return 0; }
		//virtual const DrawElements* getDrawElements() const { return 0; }
		virtual void Draw(bool useVertexBufferObjects) const = 0;

		void SetNumInstances(int n) { m_numInstances = n; }
		int GetNumInstances() const { return m_numInstances; }

		void SetMode(GLenum mode) { m_mode = mode; }
		GLenum GetMode() const { return m_mode; }
	private:

		//! 存储图元类型信息
		Type            m_primitiveType;
		int             m_numInstances;
		GLenum          m_mode;
	};


	class DCGP_API DrawArrays : public PrimitiveSet
	{
	public:

		DrawArrays(GLenum mode=0):
		  PrimitiveSet(DrawArraysPrimitiveType,mode),
			  m_first(0),
			  m_count(0) {}

		  DrawArrays(GLenum mode, GLint first, GLsizei count, int numInstances=0):
		  PrimitiveSet(DrawArraysPrimitiveType, mode, numInstances),
			  m_first(first),
			  m_count(count) {}


		  void Set(GLenum mode,GLint first, GLsizei count)
		  {
			  SetMode(mode);
			  m_first = first;
			  m_count = count;
		  }

		  void SetFirst(GLint first) { m_first = first; }
		  GLint GetFirst() const { return m_first; }

		  void SetCount(GLsizei count) { m_count = count; }
		  GLsizei GetCount() const { return m_count; }

		  virtual void Draw(bool useVertexBufferObjects) const;

		  virtual unsigned int GetNumIndices() const { return static_cast<unsigned int>(m_count); }
		  virtual unsigned int index(unsigned int pos) const { return static_cast<unsigned int>(m_first)+pos; }
		  virtual void OffsetIndices(int offset) { m_first += offset; }

	protected:

		virtual ~DrawArrays() {}

		GLint   m_first;
		GLsizei m_count;
	};
}

#endif // PRIMITIVESET_H
