#include "State.h"
#include "Texture2D.h"

#include "PolygonMode.h"

using namespace DCGp;

State::State()
	: m_texture2D(nullptr)
{

}

State::~State()
{
	if (m_texture2D)
	{
		delete m_texture2D;
		m_texture2D = nullptr;
	}

	//! 清除mode和attribute容器
	//m_attributeList.clear();
	for (auto it = m_attributeList.begin(); it != m_attributeList.end(); ++it)
	{
		delete it->second.first;
		it->second.first = nullptr;
	}
	m_attributeList.clear();
}

//void State::SetPolygonMode(PolygonMode* polygonmode)
//{
//	m_stateAttribute = polygonmode;
//}
//
//PolygonMode* State::GetPolygonMode()
//{
//	return m_stateAttribute;
//}

void State::SetAttribute(StateAttribute *attribute, StateAttribute::OverrideValue value/* =StateAttribute::OFF */)
{
	SetAttribute(m_attributeList, attribute, value);
}

void State::SetTextureAttribute(Texture2D* textureAttribute, StateAttribute::OverrideValue value/* =StateAttribute::ON */)
{
	if (m_texture2D == textureAttribute)
	{
		return;
	}

	if (m_texture2D)
	{
		delete m_texture2D;
		m_texture2D = nullptr;
	}
	m_texture2D = textureAttribute;
}

void State::SetAttribute(AttributeList& attributeList,StateAttribute *attribute, const StateAttribute::OverrideValue value/* =StateAttribute::OFF */)
{
	//! 判断attribute是否已经存在
	if (attribute)
	{
		AttributeList::iterator itr=attributeList.find(attribute->GetTypeMemberPair());
		//! 不存在则添加
		if (itr==attributeList.end())
		{
			// new entry.
			attributeList[attribute->GetTypeMemberPair()] = PtrAttributePair(attribute,value&(StateAttribute::OVERRIDE|StateAttribute::PROTECTED));
		}
	}
}

StateAttribute* State::GetAttribute(StateAttribute::Type type, unsigned int member /* = 0 */)
{
	return GetAttribute(m_attributeList, type, member);
}

StateAttribute* State::GetAttribute(AttributeList& attributeList,const StateAttribute::Type type, unsigned int member)
{
	AttributeList::iterator itr = attributeList.find(StateAttribute::TypeMemberPair(type,member));
	if (itr!=attributeList.end())
	{
		return itr->second.first;
	}
	else
		return NULL;
}

bool State::ApplyModeOnTexUnit()
{
	if (m_texture2D && m_texture2D->GetTextureObject())
	{
		glEnable(GL_TEXTURE_2D);
		m_texture2D->Apply(*this);

		return true;
	}

	if (m_texture2D && !m_texture2D->GetTextureObject())
	{
		m_texture2D->Apply(*this);
		return true;
	}


	if (!m_texture2D)
	{
		glDisable(GL_TEXTURE_2D);
	}
	return true;
}

void State::SetMode(StateAttribute::GLMode mode, StateAttribute::GLModeValue value)
{
	//if (isTextureMode(mode))
	//{
	//	//setTextureMode(0,mode,value);
	//}
	{
		SetMode(m_modeList,mode,value);
	}
}

void State::SetMode(ModeList& modeList,StateAttribute::GLMode mode, StateAttribute::GLModeValue value)
{
	modeList[mode] = value;
}

StateAttribute::GLModeValue State::GetMode(StateAttribute::GLMode mode) const
{
	return GetMode(m_modeList,mode);
	/*{
		return getTextureMode(0,mode);
	}*/
}

StateAttribute::GLModeValue State::GetMode(const ModeList& modeList,StateAttribute::GLMode mode) const
{
	ModeList::const_iterator itr = modeList.find(mode);
	if (itr!=modeList.end())
	{
		return itr->second;
	}
	else
		return StateAttribute::INHERIT;
}

bool State::ApplyMode(StateAttribute::GLMode mode,bool enabled)
{
	if (enabled)
	{
		glEnable(mode);
	}
	else 
	{
		glDisable(mode);
	}

	return true;
}

void State::Apply()
{
	/*if (m_stateAttribute)
	{
		m_stateAttribute->Apply(*this);
	}*/

	//! 应用mode
	for(ModeList::iterator it = m_modeList.begin(); it != m_modeList.end(); ++it)
	{
		bool new_value = it->second & StateAttribute::ON;
		ApplyMode(it->first,new_value);
	}

	//! 应用属性
	for(AttributeList::iterator it = m_attributeList.begin(); it != m_attributeList.end(); ++it)
	{
		it->second.first->Apply(*this);
	}

	//! 应用纹理
	ApplyModeOnTexUnit();
}

void State::SetVertexPointer(std::vector<DCVector3D >& points)
{
	SetVertexPointer(3, GL_FLOAT, 0, &points[0][0]);
}

void State::SetVertexPointer( GLint size, GLenum type, GLsizei stride, const GLvoid *ptr )
{
	{
		if (!_vertexArray._enabled || _vertexArray._dirty)
		{
			_vertexArray._enabled = true;
			glEnableClientState(GL_VERTEX_ARRAY);
		}
		//if (_vertexArray._pointer!=ptr || _vertexArray._dirty)
		{
			_vertexArray._pointer=ptr;
			glVertexPointer( size, type, stride, ptr );
		}
		_vertexArray._lazy_disable = false;
		_vertexArray._dirty = false;
	}
}

void State::SetNormalPointer( GLenum type, GLsizei stride, const GLvoid *ptr )
{
	{
		if (!_normalArray._enabled || _normalArray._dirty)
		{
			_normalArray._enabled = true;
			glEnableClientState(GL_NORMAL_ARRAY);
		}
		//if (_normalArray._pointer!=ptr || _normalArray._dirty)
		{
			_normalArray._pointer=ptr;
			glNormalPointer( type, stride, ptr );
		}
		_normalArray._lazy_disable = false;
		_normalArray._dirty = false;
	}
}

void State::SetTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *ptr)
{
	{
		if (!_texCoordArray._enabled || _texCoordArray._dirty)
		{
			_texCoordArray._enabled = true;
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		}
		//if (_normalArray._pointer!=ptr || _normalArray._dirty)
		{
			_texCoordArray._pointer=ptr;
			glTexCoordPointer( size, type, stride, ptr );
		}
		_texCoordArray._lazy_disable = false;
		_texCoordArray._dirty = false;
	}
}

void State::ApplyDisablingOfVertexAttributes()
{
	{
		if (_vertexArray._lazy_disable) DisableVertexPointer();
		if (_normalArray._lazy_disable) DisableNormalPointer();
		if (_texCoordArray._lazy_disable) DisableTexCoordPointer();
	}
}

void State::DisableVertexPointer()
{
	{
		if (_vertexArray._enabled || _vertexArray._dirty)
		{
			_vertexArray._lazy_disable = false;
			_vertexArray._enabled = false;
			_vertexArray._dirty = false;
			glDisableClientState(GL_VERTEX_ARRAY);
		}
	}
}

void State::DisableNormalPointer()
{
	{
		if (_normalArray._enabled || _normalArray._dirty)
		{
			_normalArray._lazy_disable = false;
			_normalArray._enabled = false;
			_normalArray._dirty = false;
			glDisableClientState(GL_NORMAL_ARRAY);
		}
	}
}

void State::DisableTexCoordPointer()
{
	if (_texCoordArray._enabled || _texCoordArray._dirty)
	{
		_texCoordArray._lazy_disable = false;
		_texCoordArray._enabled = false;
		_texCoordArray._dirty = false;
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
}

void State::DisableColorPointer()
{
	{
		if (_colorArray._enabled || _colorArray._dirty)
		{
			_colorArray._lazy_disable = false;
			_colorArray._enabled = false;
			_colorArray._dirty = false;
			glDisableClientState(GL_COLOR_ARRAY);
		}
	}
}