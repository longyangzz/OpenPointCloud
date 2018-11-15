#include "PrimitiveSet.h"

using namespace DCGp;

PrimitiveSet::~PrimitiveSet()
{

}

void DrawArrays::Draw(bool useVertexBufferObjects) const
{
	GLenum mode = GetMode();

	GLint first = m_first;
	{
		glDrawArrays(mode,first,m_count);
	}
}